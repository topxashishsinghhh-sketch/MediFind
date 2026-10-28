#pragma once

#include <string>

namespace medifind::models {

struct ReturnRecord {
    int id{0};
    std::string returnRef;       // e.g. "RET-2026-0001"
    int saleId{0};
    int saleItemId{0};
    int medicineId{0};
    int batchId{0};
    int quantity{0};
    std::string reason;          // e.g. "Customer returned unopened", "Damaged packaging", "Wrong medicine"
    bool returnToSellableStock{true};
    double refundAmount{0.0};
    int processedByUserId{0};
    std::string returnDate;      // YYYY-MM-DD HH:MM:SS
};

} // namespace medifind::models
