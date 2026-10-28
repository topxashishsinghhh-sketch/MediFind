#include "services/FefoAllocator.hpp"
#include "utils/DateUtils.hpp"
#include <algorithm>

namespace medifind::services {

FefoAllocationPlan FefoAllocator::planAllocation(int medicineId, int requestedQuantity, const std::string& currentDate) {
    FefoAllocationPlan plan;
    plan.medicineId = medicineId;
    plan.requestedQuantity = requestedQuantity;

    if (requestedQuantity <= 0) {
        plan.isSuccessful = false;
        plan.message = "Requested quantity must be greater than zero.";
        return plan;
    }

    // 1. Fetch eligible batches ordered by expiry_date ASC, id ASC
    auto eligibleBatches = batchRepo_.findEligibleBatchesForSale(medicineId, currentDate);

    int remainingNeeded = requestedQuantity;
    int totalAvailable = 0;

    for (const auto& batch : eligibleBatches) {
        // Double-check safety constraints:
        // Do not allocate expired or blocked stock
        if (utils::DateUtils::isExpired(batch.expiryDate, currentDate)) {
            continue;
        }
        if (batch.status == models::BatchStatus::EXPIRED ||
            batch.status == models::BatchStatus::BLOCKED ||
            batch.quantity <= 0) {
            continue;
        }

        totalAvailable += batch.quantity;

        int toTake = std::min(remainingNeeded, batch.quantity);
        if (toTake > 0) {
            BatchAllocationItem item;
            item.batchId = batch.id;
            item.batchNumber = batch.batchNumber;
            item.expiryDate = batch.expiryDate;
            item.allocatedQuantity = toTake;
            item.unitPrice = batch.sellingPrice;
            item.subtotal = toTake * batch.sellingPrice;
            item.remainingBatchStock = batch.quantity - toTake;

            plan.items.push_back(item);
            plan.totalAllocatedQuantity += toTake;
            plan.totalAmount += item.subtotal;

            remainingNeeded -= toTake;
            if (remainingNeeded == 0) {
                break;
            }
        }
    }

    if (remainingNeeded > 0) {
        plan.isSuccessful = false;
        plan.message = "Insufficient unexpired stock. Requested: " + std::to_string(requestedQuantity) +
                       ", Available in eligible batches: " + std::to_string(totalAvailable) + ".";
        return plan;
    }

    plan.isSuccessful = true;
    plan.message = "FEFO allocation planned successfully across " + std::to_string(plan.items.size()) + " batch(es).";
    return plan;
}

} // namespace medifind::services
