#pragma once
#include "product.h"
#include "repositories/csv_repository.h"
#include <string>

class ProductRepository : public CsvRepository<Product> {
public:
    ProductRepository(const std::string& filePath) : CsvRepository<Product>(filePath) {}

    std::vector<Product> findByCategory(ProductCategory category) {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Product> result;
        for (auto& [id, product] : cache) {
            if (product.category == category && !product.isArchived) {
                result.push_back(product);
            }
        }
        return result;
    }

    std::vector<Product> findArchived() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Product> result;
        for (auto& [id, product] : cache) {
            if (product.isArchived) {
                result.push_back(product);
            }
        }
        return result;
    }

    bool existsByNameAndBrand(const std::string& name, const std::string& brand) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& [id, product] : cache) {
            if (product.name == name && product.brand == brand) {
                return true;
            }
        }
        return false;
    }
};

class BatchRepository : public CsvRepository<Batch> {
public:
    BatchRepository(const std::string& filePath) : CsvRepository<Batch>(filePath) {}

    std::vector<Batch> findByProduct(int productId) {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Batch> result;
        for (auto& [id, batch] : cache) {
            if (batch.productId == productId) {
                result.push_back(batch);
            }
        }
        return result;
    }

    std::vector<Batch> findExpired() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Batch> result;
        for (auto& [id, batch] : cache) {
            if (batch.isExpired) {
                result.push_back(batch);
            }
        }
        return result;
    }

    std::vector<Batch> findBlocked() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Batch> result;
        for (auto& [id, batch] : cache) {
            if (batch.isBlocked) {
                result.push_back(batch);
            }
        }
        return result;
    }

    std::vector<Batch> findLowStock(int threshold) {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Batch> result;
        for (auto& [id, batch] : cache) {
            if (batch.availableStock() > 0 && batch.availableStock() <= threshold) {
                result.push_back(batch);
            }
        }
        return result;
    }

    bool reserveStock(int batchId, int quantity) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache.find(batchId);
        if (it == cache.end()) return false;
        if (it->second.availableStock() < quantity) return false;
        it->second.reservedQuantity += quantity;
        persist();
        return true;
    }

    bool releaseStock(int batchId, int quantity) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache.find(batchId);
        if (it == cache.end()) return false;
        if (it->second.reservedQuantity < quantity) return false;
        it->second.reservedQuantity -= quantity;
        persist();
        return true;
    }

    bool deductStock(int batchId, int quantity) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache.find(batchId);
        if (it == cache.end()) return false;
        if (it->second.reservedQuantity < quantity) return false;
        it->second.reservedQuantity -= quantity;
        it->second.physicalQuantity -= quantity;
        persist();
        return true;
    }

    std::optional<Batch> findById(int id) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache.find(id);
        if (it == cache.end()) return std::nullopt;
        return it->second;
    }

    bool updateBatch(int id, const Batch& batch) {
        std::lock_guard<std::mutex> lock(mutex);
        if (cache.find(id) == cache.end()) return false;
        cache[id] = batch;
        persist();
        return true;
    }
};
