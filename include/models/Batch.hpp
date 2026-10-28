#pragma once

#include <string>

namespace medifind::models {

enum class BatchStatus {
    AVAILABLE,
    NEAR_EXPIRY,
    EXPIRED,
    DEPLETED,
    BLOCKED
};

inline std::string batchStatusToString(BatchStatus status) {
    switch (status) {
        case BatchStatus::AVAILABLE:   return "AVAILABLE";
        case BatchStatus::NEAR_EXPIRY: return "NEAR_EXPIRY";
        case BatchStatus::EXPIRED:     return "EXPIRED";
        case BatchStatus::DEPLETED:    return "DEPLETED";
        case BatchStatus::BLOCKED:     return "BLOCKED";
    }
    return "AVAILABLE";
}

inline BatchStatus stringToBatchStatus(const std::string& str) {
    if (str == "NEAR_EXPIRY") return BatchStatus::NEAR_EXPIRY;
    if (str == "EXPIRED")     return BatchStatus::EXPIRED;
    if (str == "DEPLETED")    return BatchStatus::DEPLETED;
    if (str == "BLOCKED")     return BatchStatus::BLOCKED;
    return BatchStatus::AVAILABLE;
}

struct Batch {
    int id{0};
    int medicineId{0};
    std::string batchNumber;
    std::string mfgDate;        // YYYY-MM-DD
    std::string expiryDate;     // YYYY-MM-DD
    int quantity{0};
    double purchaseCost{0.0};   // Cost price per unit
    double sellingPrice{0.0};   // Retail price per unit
    int supplierId{0};
    std::string storageLocation;// e.g. "Shelf A-3", "Cold Storage Box 2"
    BatchStatus status{BatchStatus::AVAILABLE};
    std::string createdAt;
};

} // namespace medifind::models
