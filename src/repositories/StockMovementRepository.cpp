#include "repositories/StockMovementRepository.hpp"

namespace medifind::repositories {

int StockMovementRepository::recordMovement(const models::StockMovement& m) {
    auto stmt = db_.prepare(R"(
        INSERT INTO stock_movements (medicine_id, batch_id, type, quantity, balance_after, reference_type, reference_id, reason, user_id, created_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindInt(1, m.medicineId);
    stmt.bindInt(2, m.batchId);
    stmt.bindText(3, models::movementTypeToString(m.type));
    stmt.bindInt(4, m.quantity);
    stmt.bindInt(5, m.balanceAfter);
    stmt.bindText(6, m.referenceType);
    stmt.bindText(7, m.referenceId);
    stmt.bindText(8, m.reason);
    if (m.userId <= 0) stmt.bindNull(9);
    else stmt.bindInt(9, m.userId);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::vector<models::StockMovement> StockMovementRepository::listRecent(int limit) {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_id, type, quantity, balance_after, reference_type, reference_id, reason, user_id, created_at FROM stock_movements ORDER BY id DESC LIMIT ?;");
    stmt.bindInt(1, limit);
    std::vector<models::StockMovement> list;
    while (stmt.step()) {
        models::StockMovement m;
        m.id = stmt.getInt(0);
        m.medicineId = stmt.getInt(1);
        m.batchId = stmt.getInt(2);
        m.type = models::stringToMovementType(stmt.getText(3));
        m.quantity = stmt.getInt(4);
        m.balanceAfter = stmt.getInt(5);
        m.referenceType = stmt.getText(6);
        m.referenceId = stmt.getText(7);
        m.reason = stmt.getText(8);
        m.userId = stmt.isNull(9) ? 0 : stmt.getInt(9);
        m.createdAt = stmt.getText(10);
        list.push_back(m);
    }
    return list;
}

std::vector<models::StockMovement> StockMovementRepository::listByMedicine(int medicineId, int limit) {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_id, type, quantity, balance_after, reference_type, reference_id, reason, user_id, created_at FROM stock_movements WHERE medicine_id = ? ORDER BY id DESC LIMIT ?;");
    stmt.bindInt(1, medicineId);
    stmt.bindInt(2, limit);
    std::vector<models::StockMovement> list;
    while (stmt.step()) {
        models::StockMovement m;
        m.id = stmt.getInt(0);
        m.medicineId = stmt.getInt(1);
        m.batchId = stmt.getInt(2);
        m.type = models::stringToMovementType(stmt.getText(3));
        m.quantity = stmt.getInt(4);
        m.balanceAfter = stmt.getInt(5);
        m.referenceType = stmt.getText(6);
        m.referenceId = stmt.getText(7);
        m.reason = stmt.getText(8);
        m.userId = stmt.isNull(9) ? 0 : stmt.getInt(9);
        m.createdAt = stmt.getText(10);
        list.push_back(m);
    }
    return list;
}

std::vector<models::StockMovement> StockMovementRepository::listByBatch(int batchId) {
    auto stmt = db_.prepare("SELECT id, medicine_id, batch_id, type, quantity, balance_after, reference_type, reference_id, reason, user_id, created_at FROM stock_movements WHERE batch_id = ? ORDER BY id DESC;");
    stmt.bindInt(1, batchId);
    std::vector<models::StockMovement> list;
    while (stmt.step()) {
        models::StockMovement m;
        m.id = stmt.getInt(0);
        m.medicineId = stmt.getInt(1);
        m.batchId = stmt.getInt(2);
        m.type = models::stringToMovementType(stmt.getText(3));
        m.quantity = stmt.getInt(4);
        m.balanceAfter = stmt.getInt(5);
        m.referenceType = stmt.getText(6);
        m.referenceId = stmt.getText(7);
        m.reason = stmt.getText(8);
        m.userId = stmt.isNull(9) ? 0 : stmt.getInt(9);
        m.createdAt = stmt.getText(10);
        list.push_back(m);
    }
    return list;
}

} // namespace medifind::repositories
