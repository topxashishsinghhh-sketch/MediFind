#pragma once

#include <string>
#include <vector>

namespace medifind::models {

enum class OrderStatus {
    DRAFT,
    SUBMITTED,
    APPROVED,
    REJECTED,
    PARTIALLY_RECEIVED,
    RECEIVED,
    CANCELLED
};

inline std::string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::DRAFT:              return "DRAFT";
        case OrderStatus::SUBMITTED:          return "SUBMITTED";
        case OrderStatus::APPROVED:           return "APPROVED";
        case OrderStatus::REJECTED:           return "REJECTED";
        case OrderStatus::PARTIALLY_RECEIVED: return "PARTIALLY_RECEIVED";
        case OrderStatus::RECEIVED:           return "RECEIVED";
        case OrderStatus::CANCELLED:          return "CANCELLED";
    }
    return "DRAFT";
}

inline OrderStatus stringToOrderStatus(const std::string& str) {
    if (str == "SUBMITTED")          return OrderStatus::SUBMITTED;
    if (str == "APPROVED")           return OrderStatus::APPROVED;
    if (str == "REJECTED")           return OrderStatus::REJECTED;
    if (str == "PARTIALLY_RECEIVED") return OrderStatus::PARTIALLY_RECEIVED;
    if (str == "RECEIVED")           return OrderStatus::RECEIVED;
    if (str == "CANCELLED")          return OrderStatus::CANCELLED;
    return OrderStatus::DRAFT;
}

struct PurchaseOrderItem {
    int id{0};
    int purchaseOrderId{0};
    int medicineId{0};
    int requestedQuantity{0};
    int receivedQuantity{0};
    double unitCost{0.0};
};

struct PurchaseOrder {
    int id{0};
    std::string orderRef;        // e.g. "PO-2026-0001"
    int supplierId{0};
    std::string orderDate;       // YYYY-MM-DD
    std::string expectedDeliveryDate; // YYYY-MM-DD
    OrderStatus status{OrderStatus::DRAFT};
    double totalCost{0.0};
    std::string notes;
    int createdByUserId{0};
    std::string createdAt;
    std::vector<PurchaseOrderItem> items;
};

} // namespace medifind::models
