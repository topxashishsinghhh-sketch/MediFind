#pragma once
#include "product.h"
#include "repositories/csv_repository.h"
#include <string>
#include <vector>
#include <mutex>

class OrderRepository : public CsvRepository<Order> {
public:
    OrderRepository(const std::string& filePath) : CsvRepository<Order>(filePath) {}

    std::vector<Order> findByCustomer(int customerId) {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Order> result;
        for (auto& [id, order] : cache) {
            if (order.customerId == customerId) {
                result.push_back(order);
            }
        }
        return result;
    }

    std::vector<Order> findByStatus(const std::string& status) {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Order> result;
        for (auto& [id, order] : cache) {
            if (order.status == status) {
                result.push_back(order);
            }
        }
        return result;
    }
};

class OrderItemRepository : public CsvRepository<OrderItem> {
public:
    OrderItemRepository(const std::string& filePath) : CsvRepository<OrderItem>(filePath) {}

    std::vector<OrderItem> findByOrder(int orderId) {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<OrderItem> result;
        for (auto& [id, item] : cache) {
            if (item.orderId == orderId) {
                result.push_back(item);
            }
        }
        return result;
    }
};
