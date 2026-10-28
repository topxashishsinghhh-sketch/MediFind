#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/StockMovement.hpp"
#include <vector>

namespace medifind::repositories {

class StockMovementRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit StockMovementRepository(database::DatabaseConnection& db) : db_(db) {}

    int recordMovement(const models::StockMovement& m);
    std::vector<models::StockMovement> listRecent(int limit = 100);
    std::vector<models::StockMovement> listByMedicine(int medicineId, int limit = 50);
    std::vector<models::StockMovement> listByBatch(int batchId);
};

} // namespace medifind::repositories
