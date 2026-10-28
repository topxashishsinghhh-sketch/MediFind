#pragma once

#include "models/Batch.hpp"
#include "repositories/BatchRepository.hpp"
#include <vector>
#include <string>

namespace medifind::services {

struct BatchAllocationItem {
    int batchId{0};
    std::string batchNumber;
    std::string expiryDate;
    int allocatedQuantity{0};
    double unitPrice{0.0};
    double subtotal{0.0};
    int remainingBatchStock{0};
};

struct FefoAllocationPlan {
    bool isSuccessful{false};
    std::string message;
    int medicineId{0};
    int requestedQuantity{0};
    int totalAllocatedQuantity{0};
    double totalAmount{0.0};
    std::vector<BatchAllocationItem> items;
};

class FefoAllocator {
private:
    repositories::BatchRepository& batchRepo_;

public:
    explicit FefoAllocator(repositories::BatchRepository& batchRepo) : batchRepo_(batchRepo) {}

    /**
     * @brief Plans FEFO (First-Expired, First-Out) batch allocation for a medicine sale.
     * Guarantees earliest expiring eligible stock is consumed first.
     * Prevents selling expired, blocked, or depleted batches.
     * Deterministic handling of duplicate expiry dates via unique batch ID order.
     */
    FefoAllocationPlan planAllocation(int medicineId, int requestedQuantity, const std::string& currentDate);
};

} // namespace medifind::services
