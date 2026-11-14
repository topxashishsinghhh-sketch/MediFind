#include "product.h"
#include "repositories/csv_repository.h"
#include <sstream>

std::string Product::csvHeader() {
    return "id,name,brand,category,manufacturer,strength,dosageForm,prescriptionRequired,description,imageUrl,isArchived,createdAt";
}

std::string Product::toCsvRow() const {
    std::ostringstream oss;
    oss << _id << ","
        << csv::escapeField(name) << ","
        << csv::escapeField(brand) << ","
        << categoryToString(category) << ","
        << csv::escapeField(manufacturer) << ","
        << csv::escapeField(strength) << ","
        << csv::escapeField(dosageForm) << ","
        << (prescriptionRequired ? "1" : "0") << ","
        << csv::escapeField(description) << ","
        << csv::escapeField(imageUrl) << ","
        << (isArchived ? "1" : "0") << ","
        << csv::escapeField(createdAt);
    return oss.str();
}

Product Product::fromCsvRow(const std::vector<std::string>& fields) {
    Product p;
    p._id = std::stoi(fields[0]);
    p.name = fields[1];
    p.brand = fields[2];
    p.category = stringToCategory(fields[3]);
    p.manufacturer = fields[4];
    p.strength = fields[5];
    p.dosageForm = fields[6];
    p.prescriptionRequired = (fields[7] == "1");
    p.description = fields[8];
    p.imageUrl = fields[9];
    p.isArchived = (fields[10] == "1");
    p.createdAt = fields.size() > 11 ? fields[11] : "";
    return p;
}

json Product::toJson() const {
    json j;
    j["id"] = _id;
    j["name"] = name;
    j["brand"] = brand;
    j["category"] = categoryToString(category);
    j["manufacturer"] = manufacturer;
    j["strength"] = strength;
    j["dosageForm"] = dosageForm;
    j["prescriptionRequired"] = prescriptionRequired;
    j["description"] = description;
    j["imageUrl"] = imageUrl;
    j["isArchived"] = isArchived;
    j["createdAt"] = createdAt;
    return j;
}

std::string MedicineDetail::csvHeader() {
    return "productId,genericName,therapeuticClass,sideEffects,storageInstructions";
}

std::string MedicineDetail::toCsvRow() const {
    std::ostringstream oss;
    oss << productId << ","
        << csv::escapeField(genericName) << ","
        << csv::escapeField(therapeuticClass) << ","
        << csv::escapeField(sideEffects) << ","
        << csv::escapeField(storageInstructions);
    return oss.str();
}

MedicineDetail MedicineDetail::fromCsvRow(const std::vector<std::string>& fields) {
    MedicineDetail d;
    d.productId = std::stoi(fields[0]);
    d.genericName = fields[1];
    d.therapeuticClass = fields[2];
    d.sideEffects = fields[3];
    d.storageInstructions = fields[4];
    return d;
}

std::string HealthcareDetail::csvHeader() {
    return "productId,subcategory,usageInstructions,warrantyPeriod";
}

std::string HealthcareDetail::toCsvRow() const {
    std::ostringstream oss;
    oss << productId << ","
        << csv::escapeField(subcategory) << ","
        << csv::escapeField(usageInstructions) << ","
        << csv::escapeField(warrantyPeriod);
    return oss.str();
}

HealthcareDetail HealthcareDetail::fromCsvRow(const std::vector<std::string>& fields) {
    HealthcareDetail d;
    d.productId = std::stoi(fields[0]);
    d.subcategory = fields[1];
    d.usageInstructions = fields[2];
    d.warrantyPeriod = fields.size() > 3 ? fields[3] : "";
    return d;
}

std::string Batch::csvHeader() {
    return "id,productId,manufacturingDate,expiryDate,purchasePrice,sellingPrice,physicalQuantity,reservedQuantity,blockedQuantity,supplierId,isExpired,isBlocked";
}

std::string Batch::toCsvRow() const {
    std::ostringstream oss;
    oss << _id << ","
        << productId << ","
        << manufacturingDate << ","
        << expiryDate << ","
        << purchasePrice << ","
        << sellingPrice << ","
        << physicalQuantity << ","
        << reservedQuantity << ","
        << blockedQuantity << ","
        << supplierId << ","
        << (isExpired ? "1" : "0") << ","
        << (isBlocked ? "1" : "0");
    return oss.str();
}

Batch Batch::fromCsvRow(const std::vector<std::string>& fields) {
    Batch b;
    b._id = std::stoi(fields[0]);
    b.productId = std::stoi(fields[1]);
    b.manufacturingDate = fields[2];
    b.expiryDate = fields[3];
    b.purchasePrice = std::stod(fields[4]);
    b.sellingPrice = std::stod(fields[5]);
    b.physicalQuantity = std::stoi(fields[6]);
    b.reservedQuantity = std::stoi(fields[7]);
    b.blockedQuantity = std::stoi(fields[8]);
    b.supplierId = std::stoi(fields[9]);
    b.isExpired = (fields[10] == "1");
    b.isBlocked = (fields[11] == "1");
    return b;
}

json Batch::toJson() const {
    json j;
    j["id"] = _id;
    j["productId"] = productId;
    j["manufacturingDate"] = manufacturingDate;
    j["expiryDate"] = expiryDate;
    j["purchasePrice"] = purchasePrice;
    j["sellingPrice"] = sellingPrice;
    j["physicalQuantity"] = physicalQuantity;
    j["reservedQuantity"] = reservedQuantity;
    j["blockedQuantity"] = blockedQuantity;
    j["supplierId"] = supplierId;
    j["availableStock"] = availableStock();
    j["isExpired"] = isExpired;
    j["isBlocked"] = isBlocked;
    return j;
}

std::string OrderItem::csvHeader() {
    return "id,orderId,productId,batchId,quantity,unitPrice,totalPrice";
}

std::string OrderItem::toCsvRow() const {
    std::ostringstream oss;
    oss << _id << ","
        << orderId << ","
        << productId << ","
        << batchId << ","
        << quantity << ","
        << unitPrice << ","
        << totalPrice;
    return oss.str();
}

OrderItem OrderItem::fromCsvRow(const std::vector<std::string>& fields) {
    OrderItem item;
    item._id = std::stoi(fields[0]);
    item.orderId = std::stoi(fields[1]);
    item.productId = std::stoi(fields[2]);
    item.batchId = std::stoi(fields[3]);
    item.quantity = std::stoi(fields[4]);
    item.unitPrice = std::stod(fields[5]);
    item.totalPrice = std::stod(fields[6]);
    return item;
}

std::string Order::csvHeader() {
    return "id,customerId,status,totalAmount,paymentStatus,createdAt,updatedAt,deliveryAddress";
}

std::string Order::toCsvRow() const {
    std::ostringstream oss;
    oss << _id << ","
        << customerId << ","
        << csv::escapeField(status) << ","
        << totalAmount << ","
        << csv::escapeField(paymentStatus) << ","
        << csv::escapeField(createdAt) << ","
        << csv::escapeField(updatedAt) << ","
        << csv::escapeField(deliveryAddress);
    return oss.str();
}

Order Order::fromCsvRow(const std::vector<std::string>& fields) {
    Order o;
    o._id = std::stoi(fields[0]);
    o.customerId = std::stoi(fields[1]);
    o.status = fields[2];
    o.totalAmount = std::stod(fields[3]);
    o.paymentStatus = fields[4];
    o.createdAt = fields[5];
    o.updatedAt = fields[6];
    o.deliveryAddress = fields.size() > 7 ? fields[7] : "";
    return o;
}

json Order::toJson() const {
    json j;
    j["id"] = _id;
    j["customerId"] = customerId;
    j["status"] = status;
    j["totalAmount"] = totalAmount;
    j["paymentStatus"] = paymentStatus;
    j["createdAt"] = createdAt;
    j["updatedAt"] = updatedAt;
    j["deliveryAddress"] = deliveryAddress;
    json itemsArray = json::array();
    for (auto& item : items) {
        json iObj;
        iObj["id"] = item.id();
        iObj["orderId"] = item.orderId;
        iObj["productId"] = item.productId;
        iObj["batchId"] = item.batchId;
        iObj["quantity"] = item.quantity;
        iObj["unitPrice"] = item.unitPrice;
        iObj["totalPrice"] = item.totalPrice;
        itemsArray.push_back(iObj);
    }
    j["items"] = itemsArray;
    return j;
}
