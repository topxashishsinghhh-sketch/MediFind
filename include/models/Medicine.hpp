#pragma once

#include <string>

namespace medifind::models {

struct Medicine {
    int id{0};
    std::string name;
    std::string brand;
    std::string strength;     // e.g. "500 mg", "10 ml"
    std::string dosageForm;   // e.g. "Tablet", "Capsule", "Syrup", "Injection"
    std::string category;     // e.g. "Antibiotics", "Analgesics", "Antihistamines"
    std::string barcode;      // e.g. "8901234567890"
    std::string description;
    int minStockThreshold{10};
    bool isActive{true};
    std::string createdAt;
};

} // namespace medifind::models
