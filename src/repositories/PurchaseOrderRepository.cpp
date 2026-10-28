#include "repositories/PurchaseOrderRepository.hpp"
#include <iomanip>
#include <sstream>

namespace medifind::repositories {

int PurchaseOrderRepository::createPurchaseOrder(const models::PurchaseOrder& po) {
    database::TransactionGuard tx(db_);

    auto stmt = db_.prepare(R"(
        INSERT INTO purchase_orders (order_ref, supplier_id, order_date, expected_delivery_date, status, total_cost, notes, created_by_user_id, created_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindText(1, po.orderRef);
    stmt.bindInt(2, po.supplierId);
    stmt.bindText(3, po.orderDate);
    stmt.bindText(4, po.expectedDeliveryDate);
    stmt.bindText(5, models::orderStatusToString(po.status));
    stmt.bindDouble(6, po.totalCost);
    stmt.bindText(7, po.notes);
    if (po.createdByUserId <= 0) stmt.bindNull(8);
    else stmt.bindInt(8, po.createdByUserId);

    stmt.step();
    int poId = static_cast<int>(db_.lastInsertRowId());

    for (const auto& item : po.items) {
        auto itemStmt = db_.prepare(R"(
            INSERT INTO purchase_order_items (purchase_order_id, medicine_id, requested_quantity, received_quantity, unit_cost)
            VALUES (?, ?, ?, ?, ?);
        )");
        itemStmt.bindInt(1, poId);
        itemStmt.bindInt(2, item.medicineId);
        itemStmt.bindInt(3, item.requestedQuantity);
        itemStmt.bindInt(4, item.receivedQuantity);
        itemStmt.bindDouble(5, item.unitCost);
        itemStmt.step();
    }

    tx.commit();
    return poId;
}

std::optional<models::PurchaseOrder> PurchaseOrderRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, order_ref, supplier_id, order_date, expected_delivery_date, status, total_cost, notes, created_by_user_id, created_at FROM purchase_orders WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::PurchaseOrder po;
        po.id = stmt.getInt(0);
        po.orderRef = stmt.getText(1);
        po.supplierId = stmt.getInt(2);
        po.orderDate = stmt.getText(3);
        po.expectedDeliveryDate = stmt.getText(4);
        po.status = models::stringToOrderStatus(stmt.getText(5));
        po.totalCost = stmt.getDouble(6);
        po.notes = stmt.getText(7);
        po.createdByUserId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        po.createdAt = stmt.getText(9);
        po.items = getOrderItems(po.id);
        return po;
    }
    return std::nullopt;
}

std::optional<models::PurchaseOrder> PurchaseOrderRepository::findByRef(const std::string& orderRef) {
    auto stmt = db_.prepare("SELECT id, order_ref, supplier_id, order_date, expected_delivery_date, status, total_cost, notes, created_by_user_id, created_at FROM purchase_orders WHERE order_ref = ?;");
    stmt.bindText(1, orderRef);
    if (stmt.step()) {
        models::PurchaseOrder po;
        po.id = stmt.getInt(0);
        po.orderRef = stmt.getText(1);
        po.supplierId = stmt.getInt(2);
        po.orderDate = stmt.getText(3);
        po.expectedDeliveryDate = stmt.getText(4);
        po.status = models::stringToOrderStatus(stmt.getText(5));
        po.totalCost = stmt.getDouble(6);
        po.notes = stmt.getText(7);
        po.createdByUserId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        po.createdAt = stmt.getText(9);
        po.items = getOrderItems(po.id);
        return po;
    }
    return std::nullopt;
}

std::vector<models::PurchaseOrder> PurchaseOrderRepository::listAll() {
    auto stmt = db_.prepare("SELECT id, order_ref, supplier_id, order_date, expected_delivery_date, status, total_cost, notes, created_by_user_id, created_at FROM purchase_orders ORDER BY id DESC;");
    std::vector<models::PurchaseOrder> list;
    while (stmt.step()) {
        models::PurchaseOrder po;
        po.id = stmt.getInt(0);
        po.orderRef = stmt.getText(1);
        po.supplierId = stmt.getInt(2);
        po.orderDate = stmt.getText(3);
        po.expectedDeliveryDate = stmt.getText(4);
        po.status = models::stringToOrderStatus(stmt.getText(5));
        po.totalCost = stmt.getDouble(6);
        po.notes = stmt.getText(7);
        po.createdByUserId = stmt.isNull(8) ? 0 : stmt.getInt(8);
        po.createdAt = stmt.getText(9);
        po.items = getOrderItems(po.id);
        list.push_back(po);
    }
    return list;
}

bool PurchaseOrderRepository::updateStatus(int poId, models::OrderStatus newStatus) {
    auto stmt = db_.prepare("UPDATE purchase_orders SET status = ? WHERE id = ?;");
    stmt.bindText(1, models::orderStatusToString(newStatus));
    stmt.bindInt(2, poId);
    stmt.step();
    return db_.changes() > 0;
}

bool PurchaseOrderRepository::updateItemReceivedQuantity(int itemId, int additionalReceived) {
    auto stmt = db_.prepare("UPDATE purchase_order_items SET received_quantity = received_quantity + ? WHERE id = ?;");
    stmt.bindInt(1, additionalReceived);
    stmt.bindInt(2, itemId);
    stmt.step();
    return db_.changes() > 0;
}

std::vector<models::PurchaseOrderItem> PurchaseOrderRepository::getOrderItems(int poId) {
    auto stmt = db_.prepare("SELECT id, purchase_order_id, medicine_id, requested_quantity, received_quantity, unit_cost FROM purchase_order_items WHERE purchase_order_id = ?;");
    stmt.bindInt(1, poId);
    std::vector<models::PurchaseOrderItem> items;
    while (stmt.step()) {
        models::PurchaseOrderItem it;
        it.id = stmt.getInt(0);
        it.purchaseOrderId = stmt.getInt(1);
        it.medicineId = stmt.getInt(2);
        it.requestedQuantity = stmt.getInt(3);
        it.receivedQuantity = stmt.getInt(4);
        it.unitCost = stmt.getDouble(5);
        items.push_back(it);
    }
    return items;
}

std::string PurchaseOrderRepository::generateNextOrderRef() {
    auto stmt = db_.prepare("SELECT COALESCE(MAX(id), 0) + 1 FROM purchase_orders;");
    int nextId = 1;
    if (stmt.step()) {
        nextId = stmt.getInt(0);
    }
    std::ostringstream oss;
    oss << "PO-2026-" << std::setw(4) << std::setfill('0') << nextId;
    return oss.str();
}

} // namespace medifind::repositories
