#include "repositories/SupplierRepository.hpp"

namespace medifind::repositories {

int SupplierRepository::createSupplier(const models::Supplier& supplier) {
    auto stmt = db_.prepare(R"(
        INSERT INTO suppliers (name, contact_person, email, phone, address, is_active, created_at)
        VALUES (?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindText(1, supplier.name);
    stmt.bindText(2, supplier.contactPerson);
    stmt.bindText(3, supplier.email);
    stmt.bindText(4, supplier.phone);
    stmt.bindText(5, supplier.address);
    stmt.bindInt(6, supplier.isActive ? 1 : 0);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<models::Supplier> SupplierRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, name, contact_person, email, phone, address, is_active, created_at FROM suppliers WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::Supplier s;
        s.id = stmt.getInt(0);
        s.name = stmt.getText(1);
        s.contactPerson = stmt.getText(2);
        s.email = stmt.getText(3);
        s.phone = stmt.getText(4);
        s.address = stmt.getText(5);
        s.isActive = (stmt.getInt(6) != 0);
        s.createdAt = stmt.getText(7);
        return s;
    }
    return std::nullopt;
}

std::optional<models::Supplier> SupplierRepository::findByName(const std::string& name) {
    auto stmt = db_.prepare("SELECT id, name, contact_person, email, phone, address, is_active, created_at FROM suppliers WHERE name = ?;");
    stmt.bindText(1, name);
    if (stmt.step()) {
        models::Supplier s;
        s.id = stmt.getInt(0);
        s.name = stmt.getText(1);
        s.contactPerson = stmt.getText(2);
        s.email = stmt.getText(3);
        s.phone = stmt.getText(4);
        s.address = stmt.getText(5);
        s.isActive = (stmt.getInt(6) != 0);
        s.createdAt = stmt.getText(7);
        return s;
    }
    return std::nullopt;
}

std::vector<models::Supplier> SupplierRepository::listAll(bool includeInactive) {
    std::string sql = "SELECT id, name, contact_person, email, phone, address, is_active, created_at FROM suppliers";
    if (!includeInactive) {
        sql += " WHERE is_active = 1";
    }
    sql += " ORDER BY name ASC;";

    auto stmt = db_.prepare(sql);
    std::vector<models::Supplier> list;
    while (stmt.step()) {
        models::Supplier s;
        s.id = stmt.getInt(0);
        s.name = stmt.getText(1);
        s.contactPerson = stmt.getText(2);
        s.email = stmt.getText(3);
        s.phone = stmt.getText(4);
        s.address = stmt.getText(5);
        s.isActive = (stmt.getInt(6) != 0);
        s.createdAt = stmt.getText(7);
        list.push_back(s);
    }
    return list;
}

bool SupplierRepository::updateSupplier(const models::Supplier& supplier) {
    auto stmt = db_.prepare(R"(
        UPDATE suppliers
        SET name = ?, contact_person = ?, email = ?, phone = ?, address = ?, is_active = ?
        WHERE id = ?;
    )");
    stmt.bindText(1, supplier.name);
    stmt.bindText(2, supplier.contactPerson);
    stmt.bindText(3, supplier.email);
    stmt.bindText(4, supplier.phone);
    stmt.bindText(5, supplier.address);
    stmt.bindInt(6, supplier.isActive ? 1 : 0);
    stmt.bindInt(7, supplier.id);
    stmt.step();
    return db_.changes() > 0;
}

bool SupplierRepository::setActiveStatus(int id, bool isActive) {
    auto stmt = db_.prepare("UPDATE suppliers SET is_active = ? WHERE id = ?;");
    stmt.bindInt(1, isActive ? 1 : 0);
    stmt.bindInt(2, id);
    stmt.step();
    return db_.changes() > 0;
}

size_t SupplierRepository::count() {
    auto stmt = db_.prepare("SELECT COUNT(*) FROM suppliers WHERE is_active = 1;");
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
