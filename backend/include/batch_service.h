#pragma once
#include "product.h"
#include "repositories/product_repository.h"
#include <string>
#include <vector>
#include <optional>

// BatchService - manages batch CRUD, stock operations
class BatchService {
private:
    BatchRepository& batchRepo;

public:
    explicit BatchService(BatchRepository& repo) : batchRepo(repo) {}

    std::vector<Batch> listBatchesByProduct(int productId) {
        return batchRepo.findByProduct(productId);
    }

    std::vector<Batch> listExpiredBatches() {
        return batchRepo.findExpired();
    }

    std::vector<Batch> listBlockedBatches() {
        return batchRepo.findBlocked();
    }

    std::vector<Batch> listLowStockBatches(int threshold = 10) {
        return batchRepo.findLowStock(threshold);
    }

    int createBatch(int productId, const std::string& manufacturingDate,
                    const std::string& expiryDate, double purchasePrice,
                    double sellingPrice, int physicalQuantity, int supplierId) {
        if (productId <= 0 || manufacturingDate.empty() || expiryDate.empty()) {
            return -1;
        }
        if (sellingPrice < purchasePrice) {
            return -2;
        }
        if (physicalQuantity < 0) {
            return -3;
        }
        Batch b;
        b.setId(0);
        b.productId = productId;
        b.manufacturingDate = manufacturingDate;
        b.expiryDate = expiryDate;
        b.purchasePrice = purchasePrice;
        b.sellingPrice = sellingPrice;
        b.physicalQuantity = physicalQuantity;
        b.reservedQuantity = 0;
        b.blockedQuantity = 0;
        b.supplierId = supplierId;
        b.isExpired = false;
        b.isBlocked = false;
        return batchRepo.create(b);
    }

    bool markExpired(int batchId) {
        auto opt = batchRepo.findById(batchId);
        if (!opt.has_value()) return false;
        Batch b = opt.value();
        b.isExpired = true;
        return batchRepo.updateBatch(batchId, b);
    }

    bool markBlocked(int batchId) {
        auto opt = batchRepo.findById(batchId);
        if (!opt.has_value()) return false;
        Batch b = opt.value();
        b.isBlocked = true;
        return batchRepo.updateBatch(batchId, b);
    }

    bool unblockBatch(int batchId) {
        auto opt = batchRepo.findById(batchId);
        if (!opt.has_value()) return false;
        Batch b = opt.value();
        b.isBlocked = false;
        return batchRepo.updateBatch(batchId, b);
    }

    bool adjustStock(int batchId, int newPhysicalQuantity) {
        auto opt = batchRepo.findById(batchId);
        if (!opt.has_value()) return false;
        if (newPhysicalQuantity < 0) return false;
        Batch b = opt.value();
        b.physicalQuantity = newPhysicalQuantity;
        return batchRepo.updateBatch(batchId, b);
    }

    bool reserveStock(int batchId, int quantity) {
        return batchRepo.reserveStock(batchId, quantity);
    }

    bool releaseStock(int batchId, int quantity) {
        return batchRepo.releaseStock(batchId, quantity);
    }

    bool deductStock(int batchId, int quantity) {
        return batchRepo.deductStock(batchId, quantity);
    }

    std::optional<Batch> getBatch(int batchId) {
        return batchRepo.findById(batchId);
    }
};
