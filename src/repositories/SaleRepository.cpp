#include "repositories/SaleRepository.hpp"
#include <iomanip>
#include <sstream>

namespace medifind::repositories {

int SaleRepository::createSaleWithItems(const models::Sale& sale) {
    // NOTE: No TransactionGuard here — the caller (SalesService) owns the
    // transaction. Opening a second BEGIN TRANSACTION would cause SQLite to
    // throw "cannot start a transaction within a transaction".

    auto stmt = db_.prepare(R"(
        INSERT INTO sales (invoice_number, sale_date, sold_by_user_id, total_amount, payment_method, customer_name, notes)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )");
    stmt.bindText(1, sale.invoiceNumber);
    stmt.bindText(2, sale.saleDate.empty() ? "datetime('now')" : sale.saleDate);
    stmt.bindInt(3, sale.soldByUserId);
    stmt.bindDouble(4, sale.totalAmount);
    stmt.bindText(5, sale.paymentMethod);
    stmt.bindText(6, sale.customerName);
    stmt.bindText(7, sale.notes);
    stmt.step();

    int saleId = static_cast<int>(db_.lastInsertRowId());

    for (const auto& item : sale.items) {
        auto itemStmt = db_.prepare(R"(
            INSERT INTO sale_items (sale_id, medicine_id, batch_id, quantity, unit_price, subtotal)
            VALUES (?, ?, ?, ?, ?, ?);
        )");
        itemStmt.bindInt(1, saleId);
        itemStmt.bindInt(2, item.medicineId);
        itemStmt.bindInt(3, item.batchId);
        itemStmt.bindInt(4, item.quantity);
        itemStmt.bindDouble(5, item.unitPrice);
        itemStmt.bindDouble(6, item.subtotal);
        itemStmt.step();
    }

    return saleId;
}

std::optional<models::Sale> SaleRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, invoice_number, sale_date, sold_by_user_id, total_amount, payment_method, customer_name, notes FROM sales WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::Sale s;
        s.id = stmt.getInt(0);
        s.invoiceNumber = stmt.getText(1);
        s.saleDate = stmt.getText(2);
        s.soldByUserId = stmt.getInt(3);
        s.totalAmount = stmt.getDouble(4);
        s.paymentMethod = stmt.getText(5);
        s.customerName = stmt.getText(6);
        s.notes = stmt.getText(7);
        s.items = getSaleItems(s.id);
        return s;
    }
    return std::nullopt;
}

std::optional<models::Sale> SaleRepository::findByInvoice(const std::string& invoiceNumber) {
    auto stmt = db_.prepare("SELECT id, invoice_number, sale_date, sold_by_user_id, total_amount, payment_method, customer_name, notes FROM sales WHERE invoice_number = ?;");
    stmt.bindText(1, invoiceNumber);
    if (stmt.step()) {
        models::Sale s;
        s.id = stmt.getInt(0);
        s.invoiceNumber = stmt.getText(1);
        s.saleDate = stmt.getText(2);
        s.soldByUserId = stmt.getInt(3);
        s.totalAmount = stmt.getDouble(4);
        s.paymentMethod = stmt.getText(5);
        s.customerName = stmt.getText(6);
        s.notes = stmt.getText(7);
        s.items = getSaleItems(s.id);
        return s;
    }
    return std::nullopt;
}

std::vector<models::Sale> SaleRepository::listSales(const std::string& startDate, const std::string& endDate, int limit, int offset) {
    std::string sql = "SELECT id, invoice_number, sale_date, sold_by_user_id, total_amount, payment_method, customer_name, notes FROM sales WHERE 1=1";
    if (!startDate.empty()) {
        sql += " AND sale_date >= ?";
    }
    if (!endDate.empty()) {
        sql += " AND sale_date <= ?";
    }
    sql += " ORDER BY id DESC LIMIT ? OFFSET ?;";

    auto stmt = db_.prepare(sql);
    int pIdx = 1;
    if (!startDate.empty()) stmt.bindText(pIdx++, startDate);
    if (!endDate.empty())   stmt.bindText(pIdx++, endDate);
    stmt.bindInt(pIdx++, limit);
    stmt.bindInt(pIdx++, offset);

    std::vector<models::Sale> list;
    while (stmt.step()) {
        models::Sale s;
        s.id = stmt.getInt(0);
        s.invoiceNumber = stmt.getText(1);
        s.saleDate = stmt.getText(2);
        s.soldByUserId = stmt.getInt(3);
        s.totalAmount = stmt.getDouble(4);
        s.paymentMethod = stmt.getText(5);
        s.customerName = stmt.getText(6);
        s.notes = stmt.getText(7);
        s.items = getSaleItems(s.id);
        list.push_back(s);
    }
    return list;
}

std::vector<models::SaleItem> SaleRepository::getSaleItems(int saleId) {
    auto stmt = db_.prepare("SELECT id, sale_id, medicine_id, batch_id, quantity, unit_price, subtotal FROM sale_items WHERE sale_id = ?;");
    stmt.bindInt(1, saleId);
    std::vector<models::SaleItem> items;
    while (stmt.step()) {
        models::SaleItem it;
        it.id = stmt.getInt(0);
        it.saleId = stmt.getInt(1);
        it.medicineId = stmt.getInt(2);
        it.batchId = stmt.getInt(3);
        it.quantity = stmt.getInt(4);
        it.unitPrice = stmt.getDouble(5);
        it.subtotal = stmt.getDouble(6);
        items.push_back(it);
    }
    return items;
}

std::string SaleRepository::generateNextInvoiceNumber() {
    auto stmt = db_.prepare("SELECT COALESCE(MAX(id), 0) + 1 FROM sales;");
    int nextId = 1;
    if (stmt.step()) {
        nextId = stmt.getInt(0);
    }
    std::ostringstream oss;
    oss << "INV-2026-" << std::setw(5) << std::setfill('0') << nextId;
    return oss.str();
}

double SaleRepository::getTotalSalesRevenue(const std::string& startDate, const std::string& endDate) {
    std::string sql = "SELECT COALESCE(SUM(total_amount), 0.0) FROM sales WHERE 1=1";
    if (!startDate.empty()) sql += " AND sale_date >= ?";
    if (!endDate.empty())   sql += " AND sale_date <= ?";
    sql += ";";

    auto stmt = db_.prepare(sql);
    int pIdx = 1;
    if (!startDate.empty()) stmt.bindText(pIdx++, startDate);
    if (!endDate.empty())   stmt.bindText(pIdx++, endDate);

    if (stmt.step()) {
        return stmt.getDouble(0);
    }
    return 0.0;
}

size_t SaleRepository::countSales() {
    auto stmt = db_.prepare("SELECT COUNT(*) FROM sales;");
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
