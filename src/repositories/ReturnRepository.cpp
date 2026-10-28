#include "repositories/ReturnRepository.hpp"
#include <iomanip>
#include <sstream>

namespace medifind::repositories {

int ReturnRepository::createReturn(const models::ReturnRecord& ret) {
    auto stmt = db_.prepare(R"(
        INSERT INTO returns (return_ref, sale_id, sale_item_id, medicine_id, batch_id, quantity, reason, return_to_sellable_stock, refund_amount, processed_by_user_id, return_date)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindText(1, ret.returnRef);
    stmt.bindInt(2, ret.saleId);
    stmt.bindInt(3, ret.saleItemId);
    stmt.bindInt(4, ret.medicineId);
    stmt.bindInt(5, ret.batchId);
    stmt.bindInt(6, ret.quantity);
    stmt.bindText(7, ret.reason);
    stmt.bindInt(8, ret.returnToSellableStock ? 1 : 0);
    stmt.bindDouble(9, ret.refundAmount);
    stmt.bindInt(10, ret.processedByUserId);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<models::ReturnRecord> ReturnRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, return_ref, sale_id, sale_item_id, medicine_id, batch_id, quantity, reason, return_to_sellable_stock, refund_amount, processed_by_user_id, return_date FROM returns WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::ReturnRecord r;
        r.id = stmt.getInt(0);
        r.returnRef = stmt.getText(1);
        r.saleId = stmt.getInt(2);
        r.saleItemId = stmt.getInt(3);
        r.medicineId = stmt.getInt(4);
        r.batchId = stmt.getInt(5);
        r.quantity = stmt.getInt(6);
        r.reason = stmt.getText(7);
        r.returnToSellableStock = (stmt.getInt(8) != 0);
        r.refundAmount = stmt.getDouble(9);
        r.processedByUserId = stmt.getInt(10);
        r.returnDate = stmt.getText(11);
        return r;
    }
    return std::nullopt;
}

std::vector<models::ReturnRecord> ReturnRepository::listReturns(int limit) {
    auto stmt = db_.prepare("SELECT id, return_ref, sale_id, sale_item_id, medicine_id, batch_id, quantity, reason, return_to_sellable_stock, refund_amount, processed_by_user_id, return_date FROM returns ORDER BY id DESC LIMIT ?;");
    stmt.bindInt(1, limit);
    std::vector<models::ReturnRecord> list;
    while (stmt.step()) {
        models::ReturnRecord r;
        r.id = stmt.getInt(0);
        r.returnRef = stmt.getText(1);
        r.saleId = stmt.getInt(2);
        r.saleItemId = stmt.getInt(3);
        r.medicineId = stmt.getInt(4);
        r.batchId = stmt.getInt(5);
        r.quantity = stmt.getInt(6);
        r.reason = stmt.getText(7);
        r.returnToSellableStock = (stmt.getInt(8) != 0);
        r.refundAmount = stmt.getDouble(9);
        r.processedByUserId = stmt.getInt(10);
        r.returnDate = stmt.getText(11);
        list.push_back(r);
    }
    return list;
}

std::string ReturnRepository::generateNextReturnRef() {
    auto stmt = db_.prepare("SELECT COALESCE(MAX(id), 0) + 1 FROM returns;");
    int nextId = 1;
    if (stmt.step()) {
        nextId = stmt.getInt(0);
    }
    std::ostringstream oss;
    oss << "RET-2026-" << std::setw(4) << std::setfill('0') << nextId;
    return oss.str();
}

} // namespace medifind::repositories
