#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/Supplier.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class SupplierRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit SupplierRepository(database::DatabaseConnection& db) : db_(db) {}

    int createSupplier(const models::Supplier& supplier);
    std::optional<models::Supplier> findById(int id);
    std::optional<models::Supplier> findByName(const std::string& name);
    std::vector<models::Supplier> listAll(bool includeInactive = false);
    bool updateSupplier(const models::Supplier& supplier);
    bool setActiveStatus(int id, bool isActive);
    size_t count();
};

} // namespace medifind::repositories
