#pragma once

#include <string>
#include <vector>

namespace medifind::models {

struct SaleItem {
    int id{0};
    int saleId{0};
    int medicineId{0};
    int batchId{0};
    int quantity{0};
    double unitPrice{0.0};
    double subtotal{0.0};
};

struct Sale {
    int id{0};
    std::string invoiceNumber;    // e.g. "INV-2026-0001"
    std::string saleDate;         // YYYY-MM-DD HH:MM:SS
    int soldByUserId{0};
    double totalAmount{0.0};
    std::string paymentMethod;    // "CASH", "CARD", "UPI"
    std::string customerName;
    std::string notes;
    std::vector<SaleItem> items;
};

} // namespace medifind::models
