#include "services/PurchasingService.hpp"
#include "utils/DateUtils.hpp"
#include <algorithm>

namespace medifind::services {

PurchasingService::PurchasingService(database::DatabaseConnection& db,
                                     repositories::SupplierRepository& supplierRepo,
                                     repositories::PurchaseOrderRepository& poRepo,
                                     repositories::MedicineRepository& medRepo,
                                     repositories::BatchRepository& batchRepo,
                                     repositories::StockMovementRepository& movementRepo,
                                     repositories::AuditLogRepository& auditRepo)
    : db_(db),
      supplierRepo_(supplierRepo),
      poRepo_(poRepo),
      medRepo_(medRepo),
      batchRepo_(batchRepo),
      movementRepo_(movementRepo),
      auditRepo_(auditRepo) {}

int PurchasingService::addSupplier(const models::Supplier& supplier, int actorUserId, std::string& outError) {
    if (supplier.name.empty()) {
        outError = "Supplier name cannot be empty.";
        return -1;
    }
    if (supplierRepo_.findByName(supplier.name)) {
        outError = "Supplier '" + supplier.name + "' already exists.";
        return -1;
    }

    int id = supplierRepo_.createSupplier(supplier);
    if (id > 0) {
        models::AuditLog log;
        log.userId = actorUserId;
        log.username = "Actor #" + std::to_string(actorUserId);
        log.action = "SUPPLIER_CREATED";
        log.entityType = "SUPPLIER";
        log.entityId = std::to_string(id);
        log.changeSummary = "Created supplier: " + supplier.name;
        auditRepo_.logEvent(log);
        return id;
    }

    outError = "Database error inserting supplier.";
    return -1;
}

bool PurchasingService::updateSupplier(const models::Supplier& supplier, int actorUserId, std::string& outError) {
    if (supplier.name.empty()) {
        outError = "Supplier name cannot be empty.";
        return false;
    }
    bool ok = supplierRepo_.updateSupplier(supplier);
    if (ok) {
        models::AuditLog log;
        log.userId = actorUserId;
        log.username = "Actor #" + std::to_string(actorUserId);
        log.action = "SUPPLIER_UPDATED";
        log.entityType = "SUPPLIER";
        log.entityId = std::to_string(supplier.id);
        log.changeSummary = "Updated supplier: " + supplier.name;
        auditRepo_.logEvent(log);
        return true;
    }
    outError = "Database error updating supplier.";
    return false;
}

std::vector<models::Supplier> PurchasingService::listSuppliers() {
    return supplierRepo_.listAll();
}

int PurchasingService::createPurchaseOrder(int supplierId,
                                           const std::string& expectedDeliveryDate,
                                           const std::string& notes,
                                           const std::vector<std::pair<int, std::pair<int, double>>>& items,
                                           int actorUserId,
                                           std::string& outError) {
    if (items.empty()) {
        outError = "Purchase order must contain at least one item.";
        return -1;
    }

    auto sup = supplierRepo_.findById(supplierId);
    if (!sup) {
        outError = "Supplier ID " + std::to_string(supplierId) + " not found.";
        return -1;
    }

    models::PurchaseOrder po;
    po.orderRef = poRepo_.generateNextOrderRef();
    po.supplierId = supplierId;
    po.orderDate = utils::DateUtils::getCurrentDate();
    po.expectedDeliveryDate = expectedDeliveryDate;
    po.status = models::OrderStatus::SUBMITTED;
    po.notes = notes;
    po.createdByUserId = actorUserId;

    double totalCost = 0.0;
    for (const auto& it : items) {
        int medId = it.first;
        int qty = it.second.first;
        double cost = it.second.second;

        if (qty <= 0 || cost < 0.0) {
            outError = "Invalid quantity or cost in purchase items.";
            return -1;
        }

        models::PurchaseOrderItem poi;
        poi.medicineId = medId;
        poi.requestedQuantity = qty;
        poi.receivedQuantity = 0;
        poi.unitCost = cost;
        po.items.push_back(poi);
        totalCost += (qty * cost);
    }
    po.totalCost = totalCost;

    int poId = poRepo_.createPurchaseOrder(po);
    if (poId > 0) {
        models::AuditLog log;
        log.userId = actorUserId;
        log.username = "Actor #" + std::to_string(actorUserId);
        log.action = "PO_CREATED";
        log.entityType = "PURCHASE_ORDER";
        log.entityId = po.orderRef;
        log.changeSummary = "Created purchase order " + po.orderRef + " for total $" + std::to_string(totalCost);
        auditRepo_.logEvent(log);
        return poId;
    }

    outError = "Database error creating purchase order.";
    return -1;
}

bool PurchasingService::updateOrderStatus(int poId, models::OrderStatus newStatus, int actorUserId, std::string& outError) {
    auto poOpt = poRepo_.findById(poId);
    if (!poOpt) {
        outError = "Purchase order not found.";
        return false;
    }

    models::OrderStatus current = poOpt->status;
    if (current == models::OrderStatus::RECEIVED || current == models::OrderStatus::CANCELLED) {
        outError = "Cannot change status of a completed or cancelled purchase order.";
        return false;
    }

    bool ok = poRepo_.updateStatus(poId, newStatus);
    if (ok) {
        models::AuditLog log;
        log.userId = actorUserId;
        log.username = "Actor #" + std::to_string(actorUserId);
        log.action = "PO_STATUS_CHANGED";
        log.entityType = "PURCHASE_ORDER";
        log.entityId = poOpt->orderRef;
        log.changeSummary = "Status changed from " + models::orderStatusToString(current) + " to " + models::orderStatusToString(newStatus);
        auditRepo_.logEvent(log);
        return true;
    }

    outError = "Database error updating purchase order status.";
    return false;
}

std::vector<models::PurchaseOrder> PurchasingService::listOrders() {
    return poRepo_.listAll();
}

std::optional<models::PurchaseOrder> PurchasingService::getOrderById(int poId) {
    return poRepo_.findById(poId);
}

bool PurchasingService::receiveShipment(int poId,
                                        const std::vector<ReceiveShipmentItem>& receivedItems,
                                        int actorUserId,
                                        std::string& outError) {
    auto poOpt = poRepo_.findById(poId);
    if (!poOpt) {
        outError = "Purchase order not found.";
        return false;
    }

    if (poOpt->status != models::OrderStatus::APPROVED &&
        poOpt->status != models::OrderStatus::PARTIALLY_RECEIVED &&
        poOpt->status != models::OrderStatus::SUBMITTED) {
        outError = "Purchase order must be approved or submitted to receive shipments.";
        return false;
    }

    if (receivedItems.empty()) {
        outError = "No items specified to receive.";
        return false;
    }

    database::TransactionGuard tx(db_);

    for (const auto& item : receivedItems) {
        if (item.quantityToReceive <= 0) continue;
        if (item.batchNumber.empty()) {
            outError = "Batch number required for received shipment.";
            return false;
        }
        if (!utils::DateUtils::isValidDate(item.expiryDate)) {
            outError = "Valid expiry date required for received batch.";
            return false;
        }

        // 1. Check if batch already exists or create new batch
        auto existingBatch = batchRepo_.findByMedicineAndNumber(item.medicineId, item.batchNumber);
        int targetBatchId = 0;
        int newBalance = 0;

        if (existingBatch) {
            targetBatchId = existingBatch->id;
            newBalance = existingBatch->quantity + item.quantityToReceive;
            batchRepo_.updateQuantity(targetBatchId, newBalance);
        } else {
            models::Batch nb;
            nb.medicineId = item.medicineId;
            nb.batchNumber = item.batchNumber;
            nb.mfgDate = item.mfgDate.empty() ? utils::DateUtils::getCurrentDate() : item.mfgDate;
            nb.expiryDate = item.expiryDate;
            nb.quantity = item.quantityToReceive;
            nb.supplierId = poOpt->supplierId;
            nb.storageLocation = item.storageLocation.empty() ? "Receiving Dock" : item.storageLocation;
            nb.status = models::BatchStatus::AVAILABLE;

            // Find unitCost from po items
            double cost = 0.0;
            for (const auto& poi : poOpt->items) {
                if (poi.id == item.orderItemId || poi.medicineId == item.medicineId) {
                    cost = poi.unitCost;
                    break;
                }
            }
            nb.purchaseCost = cost;
            nb.sellingPrice = (item.sellingPrice > 0.0) ? item.sellingPrice : (cost * 1.35); // 35% margin default

            targetBatchId = batchRepo_.createBatch(nb);
            newBalance = item.quantityToReceive;
        }

        // 2. Update received quantity on PO item
        poRepo_.updateItemReceivedQuantity(item.orderItemId, item.quantityToReceive);

        // 3. Record stock movement
        models::StockMovement sm;
        sm.medicineId = item.medicineId;
        sm.batchId = targetBatchId;
        sm.type = models::MovementType::PURCHASE_RECEIVE;
        sm.quantity = item.quantityToReceive;
        sm.balanceAfter = newBalance;
        sm.referenceType = "PURCHASE_ORDER";
        sm.referenceId = poOpt->orderRef;
        sm.reason = "Shipment received against " + poOpt->orderRef;
        sm.userId = actorUserId;
        movementRepo_.recordMovement(sm);
    }

    // 4. Check if all items are fully received
    auto updatedItems = poRepo_.getOrderItems(poId);
    bool allFulfilled = true;
    for (const auto& it : updatedItems) {
        if (it.receivedQuantity < it.requestedQuantity) {
            allFulfilled = false;
            break;
        }
    }

    models::OrderStatus finalStatus = allFulfilled ? models::OrderStatus::RECEIVED : models::OrderStatus::PARTIALLY_RECEIVED;
    poRepo_.updateStatus(poId, finalStatus);

    models::AuditLog log;
    log.userId = actorUserId;
    log.username = "Actor #" + std::to_string(actorUserId);
    log.action = "SHIPMENT_RECEIVED";
    log.entityType = "PURCHASE_ORDER";
    log.entityId = poOpt->orderRef;
    log.changeSummary = "Received shipment for " + poOpt->orderRef + ". New status: " + models::orderStatusToString(finalStatus);
    auditRepo_.logEvent(log);

    tx.commit();
    return true;
}

std::vector<ReorderRecommendation> PurchasingService::generateReorderRecommendations() {
    std::vector<ReorderRecommendation> recs;
    auto allMeds = medRepo_.listAll(false);
    auto suppliers = supplierRepo_.listAll(false);

    for (const auto& med : allMeds) {
        int currentStock = medRepo_.getTotalStockQuantity(med.id);
        if (currentStock <= med.minStockThreshold) {
            ReorderRecommendation r;
            r.medicineId = med.id;
            r.medicineName = med.name;
            r.category = med.category;
            r.currentStock = currentStock;
            r.minThreshold = med.minStockThreshold;

            // Target stock: 2.5x min threshold; Reorder needed to reach target stock
            int targetStock = std::max(med.minStockThreshold * 2, 20);
            r.recommendedReorderQuantity = targetStock - currentStock;
            if (r.recommendedReorderQuantity <= 0) r.recommendedReorderQuantity = 10;

            // Estimate cost from latest batch or default
            auto batches = batchRepo_.findByMedicineId(med.id);
            if (!batches.empty()) {
                r.estimatedCost = r.recommendedReorderQuantity * batches.back().purchaseCost;
                r.suggestedSupplierId = batches.back().supplierId;
                for (const auto& s : suppliers) {
                    if (s.id == r.suggestedSupplierId) {
                        r.suggestedSupplierName = s.name;
                        break;
                    }
                }
            } else {
                r.estimatedCost = r.recommendedReorderQuantity * 5.0; // fallback default
                if (!suppliers.empty()) {
                    r.suggestedSupplierId = suppliers[0].id;
                    r.suggestedSupplierName = suppliers[0].name;
                }
            }

            recs.push_back(r);
        }
    }
    return recs;
}

} // namespace medifind::services
