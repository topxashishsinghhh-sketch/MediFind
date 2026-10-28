#include "services/InventoryService.hpp"
#include "utils/DateUtils.hpp"
#include <algorithm>

namespace medifind::services {

InventoryService::InventoryService(database::DatabaseConnection& db,
                                   repositories::MedicineRepository& medRepo,
                                   repositories::BatchRepository& batchRepo,
                                   repositories::StockMovementRepository& movementRepo,
                                   repositories::AuditLogRepository& auditRepo)
    : db_(db), medRepo_(medRepo), batchRepo_(batchRepo), movementRepo_(movementRepo), auditRepo_(auditRepo) {
    refreshDsaIndexes();
}

void InventoryService::refreshDsaIndexes() {
    medicineCache_.clear();
    nameIndex_.clear();

    auto allMeds = medRepo_.listAll(true);
    for (const auto& m : allMeds) {
        medicineCache_.insert(m.id, m);
        nameIndex_.insert(m.name, m.id);
    }
}

int InventoryService::addMedicine(const models::Medicine& medicine, int actorUserId, std::string& outError) {
    if (medicine.name.empty()) {
        outError = "Medicine name cannot be empty.";
        return -1;
    }
    if (!medicine.barcode.empty()) {
        auto existing = medRepo_.findByBarcode(medicine.barcode);
        if (existing) {
            outError = "Medicine with barcode '" + medicine.barcode + "' already exists (" + existing->name + ").";
            return -1;
        }
    }

    int medId = medRepo_.createMedicine(medicine);
    if (medId > 0) {
        auto saved = medRepo_.findById(medId);
        if (saved) {
            medicineCache_.insert(medId, *saved);
            nameIndex_.insert(saved->name, medId);
        }

        models::AuditLog log;
        log.userId = actorUserId;
        log.username = "Actor #" + std::to_string(actorUserId);
        log.action = "MEDICINE_CREATED";
        log.entityType = "MEDICINE";
        log.entityId = std::to_string(medId);
        log.changeSummary = "Created medicine: " + medicine.name + " (" + medicine.strength + ")";
        auditRepo_.logEvent(log);
        return medId;
    }

    outError = "Database error inserting medicine.";
    return -1;
}

bool InventoryService::updateMedicine(const models::Medicine& medicine, int actorUserId, std::string& outError) {
    if (medicine.name.empty()) {
        outError = "Medicine name cannot be empty.";
        return false;
    }

    auto existing = medRepo_.findById(medicine.id);
    if (!existing) {
        outError = "Medicine ID " + std::to_string(medicine.id) + " not found.";
        return false;
    }

    if (!medicine.barcode.empty()) {
        auto withBarcode = medRepo_.findByBarcode(medicine.barcode);
        if (withBarcode && withBarcode->id != medicine.id) {
            outError = "Barcode '" + medicine.barcode + "' is already assigned to " + withBarcode->name + ".";
            return false;
        }
    }

    bool ok = medRepo_.updateMedicine(medicine);
    if (ok) {
        medicineCache_.insert(medicine.id, medicine);
        nameIndex_.insert(medicine.name, medicine.id);

        models::AuditLog log;
        log.userId = actorUserId;
        log.username = "Actor #" + std::to_string(actorUserId);
        log.action = "MEDICINE_UPDATED";
        log.entityType = "MEDICINE";
        log.entityId = std::to_string(medicine.id);
        log.changeSummary = "Updated details for: " + medicine.name;
        auditRepo_.logEvent(log);
        return true;
    }

    outError = "Database error updating medicine.";
    return false;
}

std::optional<models::Medicine> InventoryService::getMedicineById(int id) {
    auto cached = medicineCache_.find(id);
    if (cached) return cached;
    return medRepo_.findById(id);
}

std::vector<models::Medicine> InventoryService::searchMedicines(const std::string& query, const std::string& category) {
    return medRepo_.search(query, category);
}

std::vector<std::pair<std::string, int>> InventoryService::searchMedicinesByPrefixBst(const std::string& prefix) {
    return nameIndex_.findByPrefix(prefix);
}

int InventoryService::stockInBatch(const models::Batch& batch, int actorUserId, const std::string& referenceType, const std::string& referenceId, std::string& outError) {
    if (batch.batchNumber.empty()) {
        outError = "Batch number cannot be empty.";
        return -1;
    }
    if (!utils::DateUtils::isValidDate(batch.mfgDate)) {
        outError = "Manufacturing date format must be valid YYYY-MM-DD.";
        return -1;
    }
    if (!utils::DateUtils::isValidDate(batch.expiryDate)) {
        outError = "Expiry date format must be valid YYYY-MM-DD.";
        return -1;
    }
    if (utils::DateUtils::compareDates(batch.mfgDate, batch.expiryDate) > 0) {
        outError = "Manufacturing date cannot be after expiry date.";
        return -1;
    }
    if (batch.quantity <= 0) {
        outError = "Stock-in quantity must be greater than zero.";
        return -1;
    }
    if (batch.purchaseCost < 0.0 || batch.sellingPrice < 0.0) {
        outError = "Cost and selling prices cannot be negative.";
        return -1;
    }

    auto med = medRepo_.findById(batch.medicineId);
    if (!med) {
        outError = "Medicine ID " + std::to_string(batch.medicineId) + " does not exist.";
        return -1;
    }

    auto existingBatch = batchRepo_.findByMedicineAndNumber(batch.medicineId, batch.batchNumber);
    if (existingBatch) {
        outError = "Batch number '" + batch.batchNumber + "' already exists for this medicine.";
        return -1;
    }

    database::TransactionGuard tx(db_);

    models::Batch toInsert = batch;
    std::string today = utils::DateUtils::getCurrentDate();
    if (utils::DateUtils::isExpired(toInsert.expiryDate, today)) {
        toInsert.status = models::BatchStatus::EXPIRED;
    } else if (utils::DateUtils::isNearExpiry(toInsert.expiryDate, today, 90)) {
        toInsert.status = models::BatchStatus::NEAR_EXPIRY;
    } else {
        toInsert.status = models::BatchStatus::AVAILABLE;
    }

    int batchId = batchRepo_.createBatch(toInsert);
    if (batchId <= 0) {
        outError = "Failed to create batch in database.";
        return -1;
    }

    models::StockMovement sm;
    sm.medicineId = batch.medicineId;
    sm.batchId = batchId;
    sm.type = models::MovementType::STOCK_IN;
    sm.quantity = batch.quantity;
    sm.balanceAfter = batch.quantity;
    sm.referenceType = referenceType.empty() ? "STOCK_RECEIPT" : referenceType;
    sm.referenceId = referenceId.empty() ? ("BAT-" + std::to_string(batchId)) : referenceId;
    sm.reason = "Initial stock-in of batch " + batch.batchNumber;
    sm.userId = actorUserId;
    movementRepo_.recordMovement(sm);

    models::AuditLog log;
    log.userId = actorUserId;
    log.username = "Actor #" + std::to_string(actorUserId);
    log.action = "STOCK_IN";
    log.entityType = "BATCH";
    log.entityId = std::to_string(batchId);
    log.changeSummary = "Stock-in: " + std::to_string(batch.quantity) + " units of " + med->name + " (Batch " + batch.batchNumber + ")";
    auditRepo_.logEvent(log);

    tx.commit();
    return batchId;
}

std::optional<models::Batch> InventoryService::getBatchById(int id) {
    return batchRepo_.findById(id);
}

std::vector<models::Batch> InventoryService::getBatchesForMedicine(int medicineId) {
    return batchRepo_.findByMedicineId(medicineId);
}

StockAdjustmentResult InventoryService::adjustStock(int batchId, int newQuantity, const std::string& reason, int actorUserId, bool isAdmin) {
    StockAdjustmentResult res;
    if (!isAdmin) {
        res.success = false;
        res.message = "Permission denied: Only administrators can authorize manual stock adjustments.";
        return res;
    }
    if (newQuantity < 0) {
        res.success = false;
        res.message = "Batch quantity cannot be negative.";
        return res;
    }
    if (reason.empty()) {
        res.success = false;
        res.message = "A valid reason is required for manual stock adjustment.";
        return res;
    }

    auto batchOpt = batchRepo_.findById(batchId);
    if (!batchOpt) {
        res.success = false;
        res.message = "Batch not found.";
        return res;
    }

    const auto& batch = *batchOpt;
    res.oldQuantity = batch.quantity;
    res.newQuantity = newQuantity;
    res.difference = newQuantity - batch.quantity;

    if (res.difference == 0) {
        res.success = true;
        res.message = "Quantity unchanged.";
        return res;
    }

    database::TransactionGuard tx(db_);

    bool ok = batchRepo_.updateQuantity(batchId, newQuantity);
    if (!ok) {
        res.success = false;
        res.message = "Database error updating batch quantity.";
        return res;
    }

    models::StockMovement sm;
    sm.medicineId = batch.medicineId;
    sm.batchId = batch.id;
    sm.type = (res.difference > 0) ? models::MovementType::ADJUSTMENT_POSITIVE : models::MovementType::ADJUSTMENT_NEGATIVE;
    sm.quantity = std::abs(res.difference);
    sm.balanceAfter = newQuantity;
    sm.referenceType = "MANUAL_ADJUSTMENT";
    sm.referenceId = "ADJ-" + std::to_string(batchId);
    sm.reason = reason;
    sm.userId = actorUserId;
    movementRepo_.recordMovement(sm);

    models::AuditLog log;
    log.userId = actorUserId;
    log.username = "Actor #" + std::to_string(actorUserId);
    log.action = "STOCK_ADJUSTMENT";
    log.entityType = "BATCH";
    log.entityId = std::to_string(batchId);
    log.changeSummary = "Adjusted batch " + batch.batchNumber + " quantity from " + std::to_string(batch.quantity) +
                        " to " + std::to_string(newQuantity) + " (Diff: " + std::to_string(res.difference) + "). Reason: " + reason;
    auditRepo_.logEvent(log);

    tx.commit();

    res.success = true;
    res.message = "Stock adjusted successfully.";
    return res;
}

std::vector<ReconciliationDiscrepancy> InventoryService::calculateReconciliation(const std::vector<std::pair<int, int>>& physicalCounts) {
    std::vector<ReconciliationDiscrepancy> discrepancies;
    for (const auto& item : physicalCounts) {
        int batchId = item.first;
        int physical = item.second;
        auto batchOpt = batchRepo_.findById(batchId);
        if (batchOpt) {
            int systemQty = batchOpt->quantity;
            int diff = physical - systemQty;
            if (diff != 0) {
                ReconciliationDiscrepancy disc;
                disc.batchId = batchId;
                disc.batchNumber = batchOpt->batchNumber;
                disc.medicineId = batchOpt->medicineId;
                auto med = getMedicineById(batchOpt->medicineId);
                disc.medicineName = med ? med->name : "Unknown";
                disc.systemQuantity = systemQty;
                disc.physicalQuantity = physical;
                disc.difference = diff;
                discrepancies.push_back(disc);
            }
        }
    }
    return discrepancies;
}

bool InventoryService::applyReconciliationAdjustment(int batchId, int physicalCount, const std::string& reason, int actorUserId, std::string& outError) {
    auto res = adjustStock(batchId, physicalCount, "Stock Reconciliation: " + reason, actorUserId, true);
    if (!res.success) {
        outError = res.message;
        return false;
    }
    return true;
}

int InventoryService::refreshExpiryStatuses(const std::string& currentDate, int warningDays) {
    auto allBatches = batchRepo_.listAll();
    int updatedCount = 0;

    for (const auto& b : allBatches) {
        models::BatchStatus newStatus = b.status;
        if (utils::DateUtils::isExpired(b.expiryDate, currentDate)) {
            newStatus = models::BatchStatus::EXPIRED;
        } else if (b.quantity == 0) {
            newStatus = models::BatchStatus::DEPLETED;
        } else if (utils::DateUtils::isNearExpiry(b.expiryDate, currentDate, warningDays)) {
            newStatus = models::BatchStatus::NEAR_EXPIRY;
        } else if (b.status != models::BatchStatus::BLOCKED) {
            newStatus = models::BatchStatus::AVAILABLE;
        }

        if (newStatus != b.status) {
            batchRepo_.updateStatus(b.id, newStatus);
            ++updatedCount;
        }
    }
    return updatedCount;
}

std::vector<models::Medicine> InventoryService::getLowStockMedicines() {
    auto allMeds = medRepo_.listAll(false);
    std::vector<models::Medicine> lowStock;
    for (const auto& m : allMeds) {
        int totalQty = medRepo_.getTotalStockQuantity(m.id);
        if (totalQty <= m.minStockThreshold) {
            lowStock.push_back(m);
        }
    }
    return lowStock;
}

std::vector<models::Batch> InventoryService::getExpiringBatches(const std::string& currentDate, int warningDays) {
    return batchRepo_.findExpiringBatches(currentDate, warningDays);
}

std::vector<models::Batch> InventoryService::getExpiredBatches(const std::string& currentDate) {
    return batchRepo_.findExpiredBatches(currentDate);
}

double InventoryService::getTotalInventoryCostValue() {
    return batchRepo_.getInventoryCostValue();
}

double InventoryService::getTotalInventoryRetailValue() {
    return batchRepo_.getInventoryRetailValue();
}

int InventoryService::getTotalUnitsInStock() {
    return batchRepo_.getTotalUnitsInStock();
}

std::vector<std::pair<std::string, int>> InventoryService::getMedicinesInOrderTraversal() {
    return nameIndex_.inOrder();
}

} // namespace medifind::services
