#pragma once
#include "product.h"
#include "repositories/product_repository.h"
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

// InventoryService - manages available stock calculation, FEFO allocation
class InventoryService {
private:
    ProductRepository& productRepo;
    BatchRepository& batchRepo;

public:
    InventoryService(ProductRepository& prodRepo, BatchRepository& batRepo)
        : productRepo(prodRepo), batchRepo(batRepo) {}

    // Calculate available stock for a product across all batches
    int getAvailableStock(int productId) {
        auto batches = batchRepo.findByProduct(productId);
        int total = 0;
        for (auto& b : batches) {
            total += b.availableStock();
        }
        return total;
    }

    // Check if sufficient stock exists for a product
    bool hasSufficientStock(int productId, int quantity) {
        return getAvailableStock(productId) >= quantity;
    }

    // FEFO allocation: allocate batches by earliest expiry first
    struct BatchAllocation {
        int batchId;
        int quantity;
    };

    std::vector<BatchAllocation> allocateStockFEFO(int productId, int quantity) {
        std::vector<BatchAllocation> allocations;
        if (quantity <= 0) return allocations;

        auto batches = batchRepo.findByProduct(productId);

        // Sort by expiry date ascending (FEFO - First Expired, First Out)
        std::sort(batches.begin(), batches.end(),
            [](const Batch& a, const Batch& b) {
                return a.expiryDate < b.expiryDate;
            });

        int remaining = quantity;
        for (auto& batch : batches) {
            if (remaining <= 0) break;
            if (batch.isExpired || batch.isBlocked) continue;
            int avail = batch.availableStock();
            if (avail <= 0) continue;
            int take = std::min(avail, remaining);
            allocations.push_back({batch.id(), take});
            remaining -= take;
        }

        return allocations;
    }

    // Reserve stock using FEFO allocation. Returns allocations on success.
    bool reserveStockFEFO(int productId, int quantity,
                          std::vector<BatchAllocation>& outAllocations) {
        if (!hasSufficientStock(productId, quantity)) return false;

        auto allocations = allocateStockFEFO(productId, quantity);
        int totalAllocated = 0;
        for (auto& a : allocations) {
            totalAllocated += a.quantity;
        }

        if (totalAllocated < quantity) return false;

        // Reserve stock from each batch
        for (auto& a : allocations) {
            if (!batchRepo.reserveStock(a.batchId, a.quantity)) {
                // Rollback previous reservations
                for (auto& rolled : allocations) {
                    if (rolled.batchId == a.batchId) break;
                    batchRepo.releaseStock(rolled.batchId, rolled.quantity);
                }
                return false;
            }
        }

        outAllocations = allocations;
        return true;
    }

    // Release reserved stock (e.g., after failed payment or cancellation)
    void releaseReservedStock(const std::vector<BatchAllocation>& allocations) {
        for (auto& a : allocations) {
            batchRepo.releaseStock(a.batchId, a.quantity);
        }
    }

    // Deduct stock after successful payment (move from reserved to sold)
    bool deductStockAfterPayment(const std::vector<BatchAllocation>& allocations) {
        for (auto& a : allocations) {
            if (!batchRepo.deductStock(a.batchId, a.quantity)) {
                return false;
            }
        }
        return true;
    }

    // Mark batches as expired based on current date (YYYY-MM-DD)
    void checkExpiry(const std::string& currentDate) {
        auto allBatches = batchRepo.findAll();
        for (auto& batch : allBatches) {
            if (!batch.isExpired && batch.expiryDate < currentDate) {
                batch.isExpired = true;
                batchRepo.updateBatch(batch.id(), batch);
            }
        }
    }

    // Get all batches for a product sorted by expiry (FEFO order)
    std::vector<Batch> getBatchesFEFO(int productId) {
        auto batches = batchRepo.findByProduct(productId);
        std::sort(batches.begin(), batches.end(),
            [](const Batch& a, const Batch& b) {
                return a.expiryDate < b.expiryDate;
            });
        return batches;
    }

    // Get total inventory value (selling price * available stock)
    double getInventoryValue(int productId = -1) {
        double total = 0.0;
        auto allBatches = batchRepo.findAll();
        for (auto& batch : allBatches) {
            if (productId == -1 || batch.productId == productId) {
                total += batch.sellingPrice * batch.availableStock();
            }
        }
        return total;
    }
};
