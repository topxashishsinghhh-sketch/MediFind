#pragma once
#include "product.h"
#include "repositories/order_repository.h"
#include "repositories/product_repository.h"
#include "inventory_service.h"
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

struct CartItemRequest {
    int productId;
    int quantity;
};

class OrderService {
private:
    OrderRepository& orderRepo;
    OrderItemRepository& itemRepo;
    BatchRepository& batchRepo;
    ProductRepository& productRepo;
    InventoryService& inventoryService;

    static std::string currentTimeIso() {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&timeT), "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

public:
    OrderService(OrderRepository& ordRepo, OrderItemRepository& itmRepo,
                 BatchRepository& batRepo, ProductRepository& prodRepo,
                 InventoryService& invService)
        : orderRepo(ordRepo), itemRepo(itmRepo), batchRepo(batRepo),
          productRepo(prodRepo), inventoryService(invService) {}

    int createOrder(int customerId, const std::vector<CartItemRequest>& items,
                    const std::string& deliveryAddress) {
        if (items.empty()) return -1;
        if (deliveryAddress.empty()) return -2;

        // Validate stock for all items first (FEFO allocation check)
        std::vector<std::pair<int, std::vector<InventoryService::BatchAllocation>>> allAllocations;
        double totalAmount = 0.0;

        for (auto& item : items) {
            if (item.productId <= 0 || item.quantity <= 0) return -3;

            auto opt = productRepo.findById(item.productId);
            if (!opt.has_value()) return -4; // product not found

            std::vector<InventoryService::BatchAllocation> allocations;
            if (!inventoryService.reserveStockFEFO(item.productId, item.quantity, allocations)) {
                // Rollback any previously allocated items
                for (auto& [pid, allocs] : allAllocations) {
                    inventoryService.releaseReservedStock(allocs);
                }
                return -5; // insufficient stock
            }

            int totalAllocated = 0;
            for (auto& a : allocations) totalAllocated += a.quantity;
            if (totalAllocated < item.quantity) {
                inventoryService.releaseReservedStock(allocations);
                for (auto& [pid, allocs] : allAllocations) {
                    inventoryService.releaseReservedStock(allocs);
                }
                return -5;
            }

            // Calculate item price from batch selling prices
            double itemTotal = 0.0;
            for (auto& a : allocations) {
                auto batchOpt = batchRepo.findById(a.batchId);
                if (batchOpt.has_value()) {
                    itemTotal += batchOpt.value().sellingPrice * a.quantity;
                }
            }
            totalAmount += itemTotal;

            allAllocations.push_back({item.productId, allocations});
        }

        // Create order
        Order order;
        order.setId(0);
        order.customerId = customerId;
        order.status = "pending";
        order.totalAmount = totalAmount;
        order.paymentStatus = "pending";
        order.createdAt = currentTimeIso();
        order.updatedAt = order.createdAt;
        order.deliveryAddress = deliveryAddress;

        int orderId = orderRepo.create(order);

        // Create order items with allocated batches
        int itemIdx = 0;
        for (auto& item : items) {
            auto& allocations = allAllocations[itemIdx].second;
            for (auto& alloc : allocations) {
                OrderItem oi;
                oi.setId(0);
                oi.orderId = orderId;
                oi.productId = item.productId;
                oi.batchId = alloc.batchId;
                oi.quantity = alloc.quantity;
                auto batchOpt = batchRepo.findById(alloc.batchId);
                oi.unitPrice = batchOpt.has_value() ? batchOpt.value().sellingPrice : 0.0;
                oi.totalPrice = oi.unitPrice * oi.quantity;
                itemRepo.create(oi);
            }
            itemIdx++;
        }

        return orderId;
    }

    std::optional<Order> getOrder(int orderId) {
        auto opt = orderRepo.findById(orderId);
        if (!opt.has_value()) return std::nullopt;
        Order order = opt.value();
        order.items = itemRepo.findByOrder(orderId);
        return order;
    }

    std::vector<Order> listOrders(int customerId = -1) {
        if (customerId >= 0) {
            return orderRepo.findByCustomer(customerId);
        }
        return orderRepo.findAll();
    }

    bool cancelOrder(int orderId) {
        auto opt = orderRepo.findById(orderId);
        if (!opt.has_value()) return false;
        Order order = opt.value();
        if (order.status == "cancelled") return false;

        // Release reserved stock
        order.items = itemRepo.findByOrder(orderId);
        std::vector<InventoryService::BatchAllocation> releaseAllocs;
        for (auto& item : order.items) {
            releaseAllocs.push_back({item.batchId, item.quantity});
        }

        // Deduct reserved stock (convert reserved -> released)
        for (auto& item : order.items) {
            batchRepo.releaseStock(item.batchId, item.quantity);
        }

        order.status = "cancelled";
        order.paymentStatus = "refunded";
        order.updatedAt = currentTimeIso();
        return orderRepo.update(orderId, order);
    }

    bool markPaid(int orderId) {
        auto opt = orderRepo.findById(orderId);
        if (!opt.has_value()) return false;
        Order order = opt.value();
        if (order.status != "pending") return false;

        order.status = "paid";
        order.paymentStatus = "success";
        order.updatedAt = currentTimeIso();

        // Convert reserved stock to sold (deduct physical quantity)
        order.items = itemRepo.findByOrder(orderId);
        for (auto& item : order.items) {
            batchRepo.deductStock(item.batchId, item.quantity);
        }

        return orderRepo.update(orderId, order);
    }

    bool markProcessing(int orderId) {
        auto opt = orderRepo.findById(orderId);
        if (!opt.has_value()) return false;
        Order order = opt.value();
        if (order.status != "paid") return false;
        order.status = "processing";
        order.updatedAt = currentTimeIso();
        return orderRepo.update(orderId, order);
    }

    bool markDispatched(int orderId) {
        auto opt = orderRepo.findById(orderId);
        if (!opt.has_value()) return false;
        Order order = opt.value();
        if (order.status != "processing") return false;
        order.status = "dispatched";
        order.updatedAt = currentTimeIso();
        return orderRepo.update(orderId, order);
    }

    bool markDelivered(int orderId) {
        auto opt = orderRepo.findById(orderId);
        if (!opt.has_value()) return false;
        Order order = opt.value();
        if (order.status != "dispatched") return false;
        order.status = "delivered";
        order.updatedAt = currentTimeIso();
        return orderRepo.update(orderId, order);
    }
};
