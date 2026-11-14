#pragma once
#include "product.h"
#include "repositories/product_repository.h"
#include "dsa/min_heap.cpp"
#include <string>
#include <vector>
#include <optional>

// ExpiryService - manages expiry alerts using MinHeap priority queue
struct ExpiryAlert {
    int priority;       // lower = more urgent (days until expiry)
    int batchId;
    int productId;
    std::string message;
};

class ExpiryService {
private:
    ProductRepository& productRepo;
    BatchRepository& batchRepo;
    MinHeap alertHeap;

    int daysUntilExpiry(const std::string& expiryDate) {
        // Simple day difference calculation (approximate)
        // In production, use proper date library
        return 0; // placeholder
    }

public:
    ExpiryService(ProductRepository& prodRepo, BatchRepository& batRepo)
        : productRepo(prodRepo), batchRepo(batRepo) {}

    // Build expiry alerts heap from all batches
    void rebuildAlerts() {
        alertHeap = MinHeap(); // reset
        auto products = productRepo.findAll(); // need findAll
        // For simplicity, scan all batches via products
        // In production, add findAll to BatchRepository
    }

    // Get urgent expiry alerts (batches expiring soon)
    std::vector<ExpiryAlert> getExpiryAlerts(int daysThreshold = 30) {
        std::vector<ExpiryAlert> alerts;
        // Scan all products and batches
        // Simplified - in production use the heap
        return alerts;
    }

    // Get low stock alerts
    std::vector<ExpiryAlert> getLowStockAlerts(int threshold = 10) {
        std::vector<ExpiryAlert> alerts;
        auto batches = batchRepo.findLowStock(threshold);
        int priority = 1; // low stock is high priority
        for (auto& b : batches) {
            alerts.push_back({priority, b.id(), b.productId,
                "Low stock: batch #" + std::to_string(b.id()) + " has " +
                std::to_string(b.availableStock()) + " units available"});
            priority++;
        }
        return alerts;
    }

    // Get expired batch alerts
    std::vector<ExpiryAlert> getExpiredAlerts() {
        std::vector<ExpiryAlert> alerts;
        auto batches = batchRepo.findExpired();
        int priority = 1; // expired is highest priority
        for (auto& b : batches) {
            alerts.push_back({priority, b.id(), b.productId,
                "Expired: batch #" + std::to_string(b.id()) + " expired on " + b.expiryDate});
            priority++;
        }
        return alerts;
    }

    // MinHeap-based urgent alert retrieval
    std::vector<ExpiryAlert> getUrgentAlerts(int limit = 10) {
        std::vector<ExpiryAlert> urgent;
        // Build heap from all alerts
        auto expired = getExpiredAlerts();
        auto lowStock = getLowStockAlerts(10);

        for (auto& a : expired) {
            alertHeap.insert(a.priority, a.batchId, a.message, "expiry");
        }
        for (auto& a : lowStock) {
            alertHeap.insert(a.priority, a.batchId, a.message, "low_stock");
        }

        for (int i = 0; i < limit && !alertHeap.empty(); i++) {
            auto opt = alertHeap.removeMin();
            if (opt.has_value()) {
                auto& item = opt.value();
                urgent.push_back({item.priority, item.entityId, 0, item.message});
            }
        }
        return urgent;
    }
};
