#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class ProductCategory {
    Medicine,
    Healthcare
};

inline ProductCategory stringToCategory(const std::string& s) {
    if (s == "medicine") return ProductCategory::Medicine;
    return ProductCategory::Healthcare;
}

inline std::string categoryToString(ProductCategory c) {
    switch (c) {
        case ProductCategory::Medicine: return "medicine";
        default: return "healthcare";
    }
}

struct Product {
private:
    int _id;

public:
    std::string name;
    std::string brand;
    ProductCategory category;
    std::string manufacturer;
    std::string strength;        // medicines only
    std::string dosageForm;      // medicines only
    bool prescriptionRequired;
    std::string description;
    std::string imageUrl;
    bool isArchived;
    std::string createdAt;

    Product() : _id(0), category(ProductCategory::Healthcare), prescriptionRequired(false), isArchived(false) {}

    void setId(int newId) { _id = newId; }
    int id() const { return _id; }

    static std::string csvHeader();
    std::string toCsvRow() const;
    static Product fromCsvRow(const std::vector<std::string>& fields);

    json toJson() const;
};

struct MedicineDetail {
    int productId;
    std::string genericName;
    std::string therapeuticClass;
    std::string sideEffects;
    std::string storageInstructions;

    static std::string csvHeader();
    std::string toCsvRow() const;
    static MedicineDetail fromCsvRow(const std::vector<std::string>& fields);
};

struct HealthcareDetail {
    int productId;
    std::string subcategory;     // personal care, wellness, device
    std::string usageInstructions;
    std::string warrantyPeriod;

    static std::string csvHeader();
    std::string toCsvRow() const;
    static HealthcareDetail fromCsvRow(const std::vector<std::string>& fields);
};

struct Batch {
private:
    int _id;

public:
    int productId;
    std::string manufacturingDate;
    std::string expiryDate;
    double purchasePrice;
    double sellingPrice;
    int physicalQuantity;
    int reservedQuantity;
    int blockedQuantity;
    int supplierId;
    bool isExpired;
    bool isBlocked;

    Batch() : _id(0), purchasePrice(0), sellingPrice(0), physicalQuantity(0),
              reservedQuantity(0), blockedQuantity(0), supplierId(0),
              isExpired(false), isBlocked(false) {}

    void setId(int newId) { _id = newId; }
    int id() const { return _id; }

    int availableStock() const {
        return physicalQuantity - reservedQuantity - blockedQuantity;
    }

    static std::string csvHeader();
    std::string toCsvRow() const;
    static Batch fromCsvRow(const std::vector<std::string>& fields);

    json toJson() const;
};

struct OrderItem {
private:
    int _id;

public:
    int orderId;
    int productId;
    int batchId;                 // FEFO allocated batch
    int quantity;
    double unitPrice;
    double totalPrice;

    OrderItem() : _id(0), orderId(0), productId(0), batchId(0), quantity(0), unitPrice(0), totalPrice(0) {}

    void setId(int newId) { _id = newId; }
    int id() const { return _id; }

    static std::string csvHeader();
    std::string toCsvRow() const;
    static OrderItem fromCsvRow(const std::vector<std::string>& fields);
};

struct Order {
private:
    int _id;

public:
    int customerId;
    std::string status;          // pending, payment_failed, paid, processing, packed, dispatched, delivered, cancelled, returned
    double totalAmount;
    std::string paymentStatus;   // pending, success, failed, refunded
    std::string createdAt;
    std::string updatedAt;
    std::string deliveryAddress;
    std::vector<OrderItem> items;

    Order() : _id(0), customerId(0), status("pending"), totalAmount(0), paymentStatus("pending") {}

    void setId(int newId) { _id = newId; }
    int id() const { return _id; }

    static std::string csvHeader();
    std::string toCsvRow() const;
    static Order fromCsvRow(const std::vector<std::string>& fields);

    json toJson() const;
};
