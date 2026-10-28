#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/Medicine.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class MedicineRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit MedicineRepository(database::DatabaseConnection& db) : db_(db) {}

    int createMedicine(const models::Medicine& medicine);
    std::optional<models::Medicine> findById(int id);
    std::optional<models::Medicine> findByBarcode(const std::string& barcode);
    std::vector<models::Medicine> findByName(const std::string& name);
    std::vector<models::Medicine> search(const std::string& query, const std::string& category = "", bool includeInactive = false);
    std::vector<models::Medicine> listAll(bool includeInactive = false);
    bool updateMedicine(const models::Medicine& medicine);
    bool setActiveStatus(int id, bool isActive);
    int getTotalStockQuantity(int medicineId);
    std::vector<std::string> listCategories();
    size_t count(bool includeInactive = false);
};

} // namespace medifind::repositories
