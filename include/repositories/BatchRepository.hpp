#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/Batch.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class BatchRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit BatchRepository(database::DatabaseConnection& db) : db_(db) {}

    int createBatch(const models::Batch& batch);
    std::optional<models::Batch> findById(int id);
    std::optional<models::Batch> findByMedicineAndNumber(int medicineId, const std::string& batchNumber);
    std::vector<models::Batch> findByMedicineId(int medicineId);
    std::vector<models::Batch> listAll();

    /**
     * @brief Core query for FEFO allocation:
     * Returns all unexpired batches with quantity > 0, ordered by expiry_date ASC.
     */
    std::vector<models::Batch> findEligibleBatchesForSale(int medicineId, const std::string& currentDate);

    std::vector<models::Batch> findExpiringBatches(const std::string& currentDate, int warningDays);
    std::vector<models::Batch> findExpiredBatches(const std::string& currentDate);

    bool updateQuantity(int batchId, int newQuantity);
    bool updateStatus(int batchId, models::BatchStatus newStatus);
    bool updateBatch(const models::Batch& batch);

    double getInventoryCostValue();
    double getInventoryRetailValue();
    int getTotalUnitsInStock();
    size_t countActiveBatches();
};

} // namespace medifind::repositories
