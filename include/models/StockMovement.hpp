#pragma once

#include <string>

namespace medifind::models {

enum class MovementType {
    STOCK_IN,
    SALE,
    RETURN_RESTOCK,
    RETURN_DISPOSAL,
    ADJUSTMENT_POSITIVE,
    ADJUSTMENT_NEGATIVE,
    PURCHASE_RECEIVE
};

inline std::string movementTypeToString(MovementType type) {
    switch (type) {
        case MovementType::STOCK_IN:            return "STOCK_IN";
        case MovementType::SALE:                return "SALE";
        case MovementType::RETURN_RESTOCK:      return "RETURN_RESTOCK";
        case MovementType::RETURN_DISPOSAL:     return "RETURN_DISPOSAL";
        case MovementType::ADJUSTMENT_POSITIVE: return "ADJUSTMENT_POSITIVE";
        case MovementType::ADJUSTMENT_NEGATIVE: return "ADJUSTMENT_NEGATIVE";
        case MovementType::PURCHASE_RECEIVE:    return "PURCHASE_RECEIVE";
    }
    return "STOCK_IN";
}

inline MovementType stringToMovementType(const std::string& str) {
    if (str == "SALE")                return MovementType::SALE;
    if (str == "RETURN_RESTOCK")      return MovementType::RETURN_RESTOCK;
    if (str == "RETURN_DISPOSAL")     return MovementType::RETURN_DISPOSAL;
    if (str == "ADJUSTMENT_POSITIVE") return MovementType::ADJUSTMENT_POSITIVE;
    if (str == "ADJUSTMENT_NEGATIVE") return MovementType::ADJUSTMENT_NEGATIVE;
    if (str == "PURCHASE_RECEIVE")    return MovementType::PURCHASE_RECEIVE;
    return MovementType::STOCK_IN;
}

struct StockMovement {
    int id{0};
    int medicineId{0};
    int batchId{0};
    MovementType type{MovementType::STOCK_IN};
    int quantity{0};            // Positive number moved
    int balanceAfter{0};        // Batch balance immediately after
    std::string referenceType;  // "SALE", "PURCHASE_ORDER", "MANUAL_ADJUSTMENT", "RETURN"
    std::string referenceId;    // e.g. "INV-2026-0001", "PO-2026-0001"
    std::string reason;
    int userId{0};
    std::string createdAt;      // YYYY-MM-DD HH:MM:SS
};

} // namespace medifind::models
