#include "repositories/MedicineRepository.hpp"

namespace medifind::repositories {

int MedicineRepository::createMedicine(const models::Medicine& medicine) {
    auto stmt = db_.prepare(R"(
        INSERT INTO medicines (name, brand, strength, dosage_form, category, barcode, description, min_stock_threshold, is_active, created_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindText(1, medicine.name);
    stmt.bindText(2, medicine.brand);
    stmt.bindText(3, medicine.strength);
    stmt.bindText(4, medicine.dosageForm);
    stmt.bindText(5, medicine.category);
    if (medicine.barcode.empty()) {
        stmt.bindNull(6);
    } else {
        stmt.bindText(6, medicine.barcode);
    }
    stmt.bindText(7, medicine.description);
    stmt.bindInt(8, medicine.minStockThreshold);
    stmt.bindInt(9, medicine.isActive ? 1 : 0);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<models::Medicine> MedicineRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, name, brand, strength, dosage_form, category, barcode, description, min_stock_threshold, is_active, created_at FROM medicines WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::Medicine m;
        m.id = stmt.getInt(0);
        m.name = stmt.getText(1);
        m.brand = stmt.getText(2);
        m.strength = stmt.getText(3);
        m.dosageForm = stmt.getText(4);
        m.category = stmt.getText(5);
        m.barcode = stmt.isNull(6) ? "" : stmt.getText(6);
        m.description = stmt.getText(7);
        m.minStockThreshold = stmt.getInt(8);
        m.isActive = (stmt.getInt(9) != 0);
        m.createdAt = stmt.getText(10);
        return m;
    }
    return std::nullopt;
}

std::optional<models::Medicine> MedicineRepository::findByBarcode(const std::string& barcode) {
    if (barcode.empty()) return std::nullopt;
    auto stmt = db_.prepare("SELECT id, name, brand, strength, dosage_form, category, barcode, description, min_stock_threshold, is_active, created_at FROM medicines WHERE barcode = ?;");
    stmt.bindText(1, barcode);
    if (stmt.step()) {
        models::Medicine m;
        m.id = stmt.getInt(0);
        m.name = stmt.getText(1);
        m.brand = stmt.getText(2);
        m.strength = stmt.getText(3);
        m.dosageForm = stmt.getText(4);
        m.category = stmt.getText(5);
        m.barcode = stmt.getText(6);
        m.description = stmt.getText(7);
        m.minStockThreshold = stmt.getInt(8);
        m.isActive = (stmt.getInt(9) != 0);
        m.createdAt = stmt.getText(10);
        return m;
    }
    return std::nullopt;
}

std::vector<models::Medicine> MedicineRepository::findByName(const std::string& name) {
    auto stmt = db_.prepare("SELECT id, name, brand, strength, dosage_form, category, barcode, description, min_stock_threshold, is_active, created_at FROM medicines WHERE name LIKE ? ORDER BY name ASC;");
    stmt.bindText(1, "%" + name + "%");
    std::vector<models::Medicine> result;
    while (stmt.step()) {
        models::Medicine m;
        m.id = stmt.getInt(0);
        m.name = stmt.getText(1);
        m.brand = stmt.getText(2);
        m.strength = stmt.getText(3);
        m.dosageForm = stmt.getText(4);
        m.category = stmt.getText(5);
        m.barcode = stmt.isNull(6) ? "" : stmt.getText(6);
        m.description = stmt.getText(7);
        m.minStockThreshold = stmt.getInt(8);
        m.isActive = (stmt.getInt(9) != 0);
        m.createdAt = stmt.getText(10);
        result.push_back(m);
    }
    return result;
}

std::vector<models::Medicine> MedicineRepository::search(const std::string& query, const std::string& category, bool includeInactive) {
    std::string sql = "SELECT id, name, brand, strength, dosage_form, category, barcode, description, min_stock_threshold, is_active, created_at FROM medicines WHERE 1=1";
    if (!includeInactive) {
        sql += " AND is_active = 1";
    }
    if (!category.empty()) {
        sql += " AND category = ?";
    }
    if (!query.empty()) {
        sql += " AND (name LIKE ? OR brand LIKE ? OR barcode = ?)";
    }
    sql += " ORDER BY name ASC;";

    auto stmt = db_.prepare(sql);
    int pIdx = 1;
    if (!category.empty()) {
        stmt.bindText(pIdx++, category);
    }
    if (!query.empty()) {
        stmt.bindText(pIdx++, "%" + query + "%");
        stmt.bindText(pIdx++, "%" + query + "%");
        stmt.bindText(pIdx++, query);
    }

    std::vector<models::Medicine> result;
    while (stmt.step()) {
        models::Medicine m;
        m.id = stmt.getInt(0);
        m.name = stmt.getText(1);
        m.brand = stmt.getText(2);
        m.strength = stmt.getText(3);
        m.dosageForm = stmt.getText(4);
        m.category = stmt.getText(5);
        m.barcode = stmt.isNull(6) ? "" : stmt.getText(6);
        m.description = stmt.getText(7);
        m.minStockThreshold = stmt.getInt(8);
        m.isActive = (stmt.getInt(9) != 0);
        m.createdAt = stmt.getText(10);
        result.push_back(m);
    }
    return result;
}

std::vector<models::Medicine> MedicineRepository::listAll(bool includeInactive) {
    std::string sql = "SELECT id, name, brand, strength, dosage_form, category, barcode, description, min_stock_threshold, is_active, created_at FROM medicines";
    if (!includeInactive) {
        sql += " WHERE is_active = 1";
    }
    sql += " ORDER BY name ASC;";

    auto stmt = db_.prepare(sql);
    std::vector<models::Medicine> list;
    while (stmt.step()) {
        models::Medicine m;
        m.id = stmt.getInt(0);
        m.name = stmt.getText(1);
        m.brand = stmt.getText(2);
        m.strength = stmt.getText(3);
        m.dosageForm = stmt.getText(4);
        m.category = stmt.getText(5);
        m.barcode = stmt.isNull(6) ? "" : stmt.getText(6);
        m.description = stmt.getText(7);
        m.minStockThreshold = stmt.getInt(8);
        m.isActive = (stmt.getInt(9) != 0);
        m.createdAt = stmt.getText(10);
        list.push_back(m);
    }
    return list;
}

bool MedicineRepository::updateMedicine(const models::Medicine& medicine) {
    auto stmt = db_.prepare(R"(
        UPDATE medicines
        SET name = ?, brand = ?, strength = ?, dosage_form = ?, category = ?, barcode = ?, description = ?, min_stock_threshold = ?, is_active = ?
        WHERE id = ?;
    )");
    stmt.bindText(1, medicine.name);
    stmt.bindText(2, medicine.brand);
    stmt.bindText(3, medicine.strength);
    stmt.bindText(4, medicine.dosageForm);
    stmt.bindText(5, medicine.category);
    if (medicine.barcode.empty()) {
        stmt.bindNull(6);
    } else {
        stmt.bindText(6, medicine.barcode);
    }
    stmt.bindText(7, medicine.description);
    stmt.bindInt(8, medicine.minStockThreshold);
    stmt.bindInt(9, medicine.isActive ? 1 : 0);
    stmt.bindInt(10, medicine.id);
    stmt.step();
    return db_.changes() > 0;
}

bool MedicineRepository::setActiveStatus(int id, bool isActive) {
    auto stmt = db_.prepare("UPDATE medicines SET is_active = ? WHERE id = ?;");
    stmt.bindInt(1, isActive ? 1 : 0);
    stmt.bindInt(2, id);
    stmt.step();
    return db_.changes() > 0;
}

int MedicineRepository::getTotalStockQuantity(int medicineId) {
    auto stmt = db_.prepare("SELECT COALESCE(SUM(quantity), 0) FROM batches WHERE medicine_id = ? AND status != 'EXPIRED' AND status != 'BLOCKED';");
    stmt.bindInt(1, medicineId);
    if (stmt.step()) {
        return stmt.getInt(0);
    }
    return 0;
}

std::vector<std::string> MedicineRepository::listCategories() {
    auto stmt = db_.prepare("SELECT DISTINCT category FROM medicines WHERE category IS NOT NULL AND category != '' ORDER BY category ASC;");
    std::vector<std::string> cats;
    while (stmt.step()) {
        cats.push_back(stmt.getText(0));
    }
    return cats;
}

size_t MedicineRepository::count(bool includeInactive) {
    std::string sql = "SELECT COUNT(*) FROM medicines";
    if (!includeInactive) {
        sql += " WHERE is_active = 1";
    }
    sql += ";";
    auto stmt = db_.prepare(sql);
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
