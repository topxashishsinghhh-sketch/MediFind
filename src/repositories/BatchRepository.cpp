#include "repositories/BatchRepository.hpp"
#include "utils/DateUtils.hpp"

namespace medifind::repositories {

int BatchRepository::createBatch(const models::Batch& batch) {
    auto stmt = db_.prepare(R"(
        INSERT INTO batches (medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, supplier_id, storage_location, status, created_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindInt(1, batch.medicineId);
    stmt.bindText(2, batch.batchNumber);
    stmt.bindText(3, batch.mfgDate);
    stmt.bindText(4, batch.expiryDate);
    stmt.bindInt(5, batch.quantity);
    stmt.bindDouble(6, batch.purchaseCost);
    stmt.bindDouble(7, batch.sellingPrice);
    if (batch.supplierId <= 0) {
        stmt.bindNull(8);
    } else {
        stmt.bindInt(8, batch.supplierId);
    }
    stmt.bindText(9, batch.storageLocation);
    stmt.bindText(10, models::batchStatusToString(batch.status));
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<models::Batch> BatchRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, supplier_id, storage_location, status, created_at FROM batches WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::Batch b;
        b.id = stmt.getInt(0);
        b.medicineId = stmt.getInt(1);
        b.batchNumber = stmt.getText(2);
        b.mfgDate = stmt.getText(3);
        b.expiryDate = stmt.getText(4);
        b.quantity = stmt.getInt(5);
        b.purchaseCost = stmt.getDouble(6);
        b.sellingPrice = stmt.getDouble(7);
        b.supplierId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        b.storageLocation = stmt.getText(9);
        b.status = models::stringToBatchStatus(stmt.getText(10));
        b.createdAt = stmt.getText(11);
        return b;
    }
    return std::nullopt;
}

std::optional<models::Batch> BatchRepository::findByMedicineAndNumber(int medicineId, const std::string& batchNumber) {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, supplier_id, storage_location, status, created_at FROM batches WHERE medicine_id = ? AND batch_number = ?;");
    stmt.bindInt(1, medicineId);
    stmt.bindText(2, batchNumber);
    if (stmt.step()) {
        models::Batch b;
        b.id = stmt.getInt(0);
        b.medicineId = stmt.getInt(1);
        b.batchNumber = stmt.getText(2);
        b.mfgDate = stmt.getText(3);
        b.expiryDate = stmt.getText(4);
        b.quantity = stmt.getInt(5);
        b.purchaseCost = stmt.getDouble(6);
        b.sellingPrice = stmt.getDouble(7);
        b.supplierId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        b.storageLocation = stmt.getText(9);
        b.status = models::stringToBatchStatus(stmt.getText(10));
        b.createdAt = stmt.getText(11);
        return b;
    }
    return std::nullopt;
}

std::vector<models::Batch> BatchRepository::findByMedicineId(int medicineId) {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, supplier_id, storage_location, status, created_at FROM batches WHERE medicine_id = ? ORDER BY expiry_date ASC, id ASC;");
    stmt.bindInt(1, medicineId);
    std::vector<models::Batch> list;
    while (stmt.step()) {
        models::Batch b;
        b.id = stmt.getInt(0);
        b.medicineId = stmt.getInt(1);
        b.batchNumber = stmt.getText(2);
        b.mfgDate = stmt.getText(3);
        b.expiryDate = stmt.getText(4);
        b.quantity = stmt.getInt(5);
        b.purchaseCost = stmt.getDouble(6);
        b.sellingPrice = stmt.getDouble(7);
        b.supplierId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        b.storageLocation = stmt.getText(9);
        b.status = models::stringToBatchStatus(stmt.getText(10));
        b.createdAt = stmt.getText(11);
        list.push_back(b);
    }
    return list;
}

std::vector<models::Batch> BatchRepository::listAll() {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, supplier_id, storage_location, status, created_at FROM batches ORDER BY expiry_date ASC;");
    std::vector<models::Batch> list;
    while (stmt.step()) {
        models::Batch b;
        b.id = stmt.getInt(0);
        b.medicineId = stmt.getInt(1);
        b.batchNumber = stmt.getText(2);
        b.mfgDate = stmt.getText(3);
        b.expiryDate = stmt.getText(4);
        b.quantity = stmt.getInt(5);
        b.purchaseCost = stmt.getDouble(6);
        b.sellingPrice = stmt.getDouble(7);
        b.supplierId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        b.storageLocation = stmt.getText(9);
        b.status = models::stringToBatchStatus(stmt.getText(10));
        b.createdAt = stmt.getText(11);
        list.push_back(b);
    }
    return list;
}

std::vector<models::Batch> BatchRepository::findEligibleBatchesForSale(int medicineId, const std::string& currentDate) {
    // FEFO order: earliest expiry date first, positive quantity, not expired, not blocked
    auto stmt = db_.prepare(R"(
        SELECT id, medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, supplier_id, storage_location, status, created_at
        FROM batches
        WHERE medicine_id = ?
          AND quantity > 0
          AND expiry_date >= ?
          AND status != 'EXPIRED'
          AND status != 'BLOCKED'
        ORDER BY expiry_date ASC, id ASC;
    )");
    stmt.bindInt(1, medicineId);
    stmt.bindText(2, currentDate);

    std::vector<models::Batch> list;
    while (stmt.step()) {
        models::Batch b;
        b.id = stmt.getInt(0);
        b.medicineId = stmt.getInt(1);
        b.batchNumber = stmt.getText(2);
        b.mfgDate = stmt.getText(3);
        b.expiryDate = stmt.getText(4);
        b.quantity = stmt.getInt(5);
        b.purchaseCost = stmt.getDouble(6);
        b.sellingPrice = stmt.getDouble(7);
        b.supplierId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        b.storageLocation = stmt.getText(9);
        b.status = models::stringToBatchStatus(stmt.getText(10));
        b.createdAt = stmt.getText(11);
        list.push_back(b);
    }
    return list;
}

std::vector<models::Batch> BatchRepository::findExpiringBatches(const std::string& currentDate, int warningDays) {
    auto all = listAll();
    std::vector<models::Batch> expiring;
    for (const auto& b : all) {
        if (b.quantity > 0 && utils::DateUtils::isNearExpiry(b.expiryDate, currentDate, warningDays)) {
            expiring.push_back(b);
        }
    }
    return expiring;
}

std::vector<models::Batch> BatchRepository::findExpiredBatches(const std::string& currentDate) {
    auto all = listAll();
    std::vector<models::Batch> expired;
    for (const auto& b : all) {
        if (b.quantity > 0 && utils::DateUtils::isExpired(b.expiryDate, currentDate)) {
            expired.push_back(b);
        }
    }
    return expired;
}

bool BatchRepository::updateQuantity(int batchId, int newQuantity) {
    auto stmt = db_.prepare("UPDATE batches SET quantity = ?, status = CASE WHEN ? = 0 THEN 'DEPLETED' ELSE status END WHERE id = ?;");
    stmt.bindInt(1, newQuantity);
    stmt.bindInt(2, newQuantity);
    stmt.bindInt(3, batchId);
    stmt.step();
    return db_.changes() > 0;
}

bool BatchRepository::updateStatus(int batchId, models::BatchStatus newStatus) {
    auto stmt = db_.prepare("UPDATE batches SET status = ? WHERE id = ?;");
    stmt.bindText(1, models::batchStatusToString(newStatus));
    stmt.bindInt(2, batchId);
    stmt.step();
    return db_.changes() > 0;
}

bool BatchRepository::updateBatch(const models::Batch& batch) {
    auto stmt = db_.prepare(R"(
        UPDATE batches
        SET mfg_date = ?, expiry_date = ?, quantity = ?, purchase_cost = ?, selling_price = ?, supplier_id = ?, storage_location = ?, status = ?
        WHERE id = ?;
    )");
    stmt.bindText(1, batch.mfgDate);
    stmt.bindText(2, batch.expiryDate);
    stmt.bindInt(3, batch.quantity);
    stmt.bindDouble(4, batch.purchaseCost);
    stmt.bindDouble(5, batch.sellingPrice);
    if (batch.supplierId <= 0) {
        stmt.bindNull(6);
    } else {
        stmt.bindInt(6, batch.supplierId);
    }
    stmt.bindText(7, batch.storageLocation);
    stmt.bindText(8, models::batchStatusToString(batch.status));
    stmt.bindInt(9, batch.id);
    stmt.step();
    return db_.changes() > 0;
}

double BatchRepository::getInventoryCostValue() {
    auto stmt = db_.prepare("SELECT COALESCE(SUM(quantity * purchase_cost), 0.0) FROM batches WHERE status != 'EXPIRED' AND status != 'BLOCKED';");
    if (stmt.step()) {
        return stmt.getDouble(0);
    }
    return 0.0;
}

double BatchRepository::getInventoryRetailValue() {
    auto stmt = db_.prepare("SELECT COALESCE(SUM(quantity * selling_price), 0.0) FROM batches WHERE status != 'EXPIRED' AND status != 'BLOCKED';");
    if (stmt.step()) {
        return stmt.getDouble(0);
    }
    return 0.0;
}

int BatchRepository::getTotalUnitsInStock() {
    auto stmt = db_.prepare("SELECT COALESCE(SUM(quantity), 0) FROM batches WHERE status != 'EXPIRED' AND status != 'BLOCKED';");
    if (stmt.step()) {
        return stmt.getInt(0);
    }
    return 0;
}

size_t BatchRepository::countActiveBatches() {
    auto stmt = db_.prepare("SELECT COUNT(*) FROM batches WHERE quantity > 0 AND status != 'EXPIRED' AND status != 'BLOCKED';");
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
