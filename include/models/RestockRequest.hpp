#pragma once

#include <string>

namespace medifind::models {

enum class RestockStatus {
    PENDING,
    APPROVED,
    REJECTED,
    ORDERED,
    RECEIVED,
    CANCELLED
};

inline std::string restockStatusToString(RestockStatus status) {
    switch (status) {
        case RestockStatus::PENDING:   return "PENDING";
        case RestockStatus::APPROVED:  return "APPROVED";
        case RestockStatus::REJECTED:  return "REJECTED";
        case RestockStatus::ORDERED:   return "ORDERED";
        case RestockStatus::RECEIVED:  return "RECEIVED";
        case RestockStatus::CANCELLED: return "CANCELLED";
    }
    return "PENDING";
}

inline RestockStatus stringToRestockStatus(const std::string& str) {
    if (str == "APPROVED")  return RestockStatus::APPROVED;
    if (str == "REJECTED")  return RestockStatus::REJECTED;
    if (str == "ORDERED")   return RestockStatus::ORDERED;
    if (str == "RECEIVED")  return RestockStatus::RECEIVED;
    if (str == "CANCELLED") return RestockStatus::CANCELLED;
    return RestockStatus::PENDING;
}

struct RestockRequest {
    int id{0};
    std::string requestRef;      // e.g. "REQ-2026-0001"
    int medicineId{0};
    int requestedQuantity{0};
    std::string reason;          // e.g. "Threshold reached (stock=2, min=10)"
    RestockStatus status{RestockStatus::PENDING};
    int requestedByUserId{0};
    int approvedByUserId{0};
    std::string reviewNotes;
    std::string createdAt;
    std::string updatedAt;
};

} // namespace medifind::models
