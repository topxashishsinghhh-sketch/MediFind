#pragma once

#include "database/DatabaseConnection.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "repositories/StockMovementRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "models/Medicine.hpp"
#include "models/Batch.hpp"
#include "models/StockMovement.hpp"
#include "dsa/HashTable.hpp"
#include "dsa/BST.hpp"
#include "dsa/LinkedList.hpp"
#include <string>
#include <vector>
#include <optional>

namespace medifind::services {

struct StockAdjustmentResult {
    bool success{false};
    std::string message;
    int oldQuantity{0};
    int newQuantity{0};
    int difference{0};
};

struct ReconciliationDiscrepancy {
    int medicineId{0};
    std::string medicineName;
    int batchId{0};
    std::string batchNumber;
    int systemQuantity{0};
    int physicalQuantity{0};
    int difference{0}; // physical - system
};

class InventoryService {
private:
    database::DatabaseConnection& db_;
    repositories::MedicineRepository& medRepo_;
    repositories::BatchRepository& batchRepo_;
    repositories::StockMovementRepository& movementRepo_;
    repositories::AuditLogRepository& auditRepo_;

    // DSA in-memory structures for demonstration & fast querying
    dsa::HashTable<int, models::Medicine> medicineCache_;
    dsa::BinarySearchTree<std::string, int> nameIndex_;

public:
    InventoryService(database::DatabaseConnection& db,
                     repositories::MedicineRepository& medRepo,
                     repositories::BatchRepository& batchRepo,
                     repositories::StockMovementRepository& movementRepo,
                     repositories::AuditLogRepository& auditRepo);

    void refreshDsaIndexes();

    // Medicine Operations
    int addMedicine(const models::Medicine& medicine, int actorUserId, std::string& outError);
    bool updateMedicine(const models::Medicine& medicine, int actorUserId, std::string& outError);
    std::optional<models::Medicine> getMedicineById(int id);
    std::vector<models::Medicine> searchMedicines(const std::string& query, const std::string& category = "");
    std::vector<std::pair<std::string, int>> searchMedicinesByPrefixBst(const std::string& prefix);

    // Batch & Stock-In Operations
    int stockInBatch(const models::Batch& batch, int actorUserId, const std::string& referenceType, const std::string& referenceId, std::string& outError);
    std::optional<models::Batch> getBatchById(int id);
    std::vector<models::Batch> getBatchesForMedicine(int medicineId);

    // Stock Adjustments
    StockAdjustmentResult adjustStock(int batchId, int newQuantity, const std::string& reason, int actorUserId, bool isAdmin);

    // Reconciliation
    std::vector<ReconciliationDiscrepancy> calculateReconciliation(const std::vector<std::pair<int, int>>& physicalCounts); // batchId -> count
    bool applyReconciliationAdjustment(int batchId, int physicalCount, const std::string& reason, int actorUserId, std::string& outError);

    // Expiry & Alerts
    int refreshExpiryStatuses(const std::string& currentDate, int warningDays = 90);
    std::vector<models::Medicine> getLowStockMedicines();
    std::vector<models::Batch> getExpiringBatches(const std::string& currentDate, int warningDays = 90);
    std::vector<models::Batch> getExpiredBatches(const std::string& currentDate);

    // Valuation
    double getTotalInventoryCostValue();
    double getTotalInventoryRetailValue();
    int getTotalUnitsInStock();

    // DSA Traversal
    std::vector<std::pair<std::string, int>> getMedicinesInOrderTraversal();
};

} // namespace medifind::services
