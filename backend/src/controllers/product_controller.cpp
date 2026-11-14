#include "crow.h"
#include "product_service.h"
#include "medicine_service.h"
#include "batch_service.h"
#include "inventory_service.h"
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Crow's request body sometimes loses JSON quotes (e.g. {productId:1,quantity:50}).
static std::string extractBodyField(const std::string& body, const std::string& key) {
    std::string quotedKey = "\"" + key + "\":\"";
    size_t pos = body.find(quotedKey);
    if (pos != std::string::npos) {
        size_t start = pos + quotedKey.size();
        size_t end = body.find("\"", start);
        if (end != std::string::npos) return body.substr(start, end - start);
    }
    std::string plainKey = key + ":";
    pos = body.find(plainKey);
    if (pos != std::string::npos) {
        size_t start = pos + plainKey.size();
        size_t end = body.find(",", start);
        if (end == std::string::npos) end = body.find("}", start);
        if (end != std::string::npos) {
            std::string val = body.substr(start, end - start);
            size_t first = val.find_first_not_of(" \t");
            size_t last = val.find_last_not_of(" \t");
            if (first != std::string::npos && last != std::string::npos)
                return val.substr(first, last - first + 1);
        }
    }
    return "";
}

static int extractBodyInt(const std::string& body, const std::string& key, int defaultVal = 0) {
    std::string val = extractBodyField(body, key);
    if (val.empty()) return defaultVal;
    try { return std::stoi(val); } catch (...) { return defaultVal; }
}

static double extractBodyDouble(const std::string& body, const std::string& key, double defaultVal = 0.0) {
    std::string val = extractBodyField(body, key);
    if (val.empty()) return defaultVal;
    try { return std::stod(val); } catch (...) { return defaultVal;
    }
}

static bool extractBodyBool(const std::string& body, const std::string& key, bool defaultVal = false) {
    std::string val = extractBodyField(body, key);
    if (val.empty()) return defaultVal;
    return val == "true" || val == "1";
}

// Forward declaration
void setupProductRoutes(crow::SimpleApp& app, ProductService& productService,
                        MedicineService& medicineService, BatchService& batchService,
                        InventoryService& inventoryService);

// POST /api/products - create product
// GET /api/products - list products (query: category)
// GET /api/products/:id - get product with batches
// POST /api/batches - create batch
// GET /api/batches/product/:id - list batches for product
// GET /api/inventory/stock/:id - get available stock
// POST /api/inventory/reserve - reserve stock (FEFO)
// GET /api/inventory/expiry-alerts - expiry alerts
// GET /api/inventory/low-stock - low stock alerts

void setupProductRoutes(crow::SimpleApp& app, ProductService& productService,
                               MedicineService& medicineService, BatchService& batchService,
                               InventoryService& inventoryService) {

    // GET /api/products
    CROW_ROUTE(app, "/api/products").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            auto category = req.url_params.get("category");
            json products = json::array();

            if (category && std::string(category) == "medicine") {
                auto meds = productService.listAvailableProducts();
                for (auto& p : meds) products.push_back(p.toJson());
            } else if (category && std::string(category) == "healthcare") {
                auto hcs = productService.listHealthcareProducts();
                for (auto& p : hcs) products.push_back(p.toJson());
            } else {
                auto meds = productService.listAvailableProducts();
                auto hcs = productService.listHealthcareProducts();
                for (auto& p : meds) products.push_back(p.toJson());
                for (auto& p : hcs) products.push_back(p.toJson());
            }

            json body;
            body["products"] = products;
            return crow::response(200, body.dump());
        }
    );

    // GET /api/products/:id
    CROW_ROUTE(app, "/api/products/<int>").methods(crow::HTTPMethod::Get)(
        [&](int id) {
            auto opt = productService.getProduct(id);
            if (!opt.has_value()) {
                json body; body["error"] = "Product not found";
                return crow::response(404, body.dump());
            }
            json body;
            body["product"] = opt.value().toJson();

            // Add batches
            auto batches = batchService.listBatchesByProduct(id);
            json batchArray = json::array();
            for (auto& b : batches) batchArray.push_back(b.toJson());
            body["batches"] = batchArray;

            return crow::response(200, body.dump());
        }
    );

    // POST /api/products
    CROW_ROUTE(app, "/api/products").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            std::string name = extractBodyField(req.body, "name");
            std::string brand = extractBodyField(req.body, "brand");
            std::string category = extractBodyField(req.body, "category");
            std::string manufacturer = extractBodyField(req.body, "manufacturer");
            std::string strength = extractBodyField(req.body, "strength");
            std::string dosageForm = extractBodyField(req.body, "dosageForm");
            bool prescriptionRequired = extractBodyBool(req.body, "prescriptionRequired", false);
            std::string description = extractBodyField(req.body, "description");
            std::string imageUrl = extractBodyField(req.body, "imageUrl");

            if (name.empty() || brand.empty() || manufacturer.empty()) {
                json errorBody;
                errorBody["error"] = "Name, brand, and manufacturer are required";
                return crow::response(400, errorBody.dump());
            }
            ProductCategory cat = (category == "medicine") ? ProductCategory::Medicine : ProductCategory::Healthcare;

            int result = productService.createProduct(name, brand, cat, manufacturer,
                strength, dosageForm, prescriptionRequired, description, imageUrl);

            if (result == -1) {
                json errorBody;
                errorBody["error"] = "Name, brand, and manufacturer are required";
                return crow::response(400, errorBody.dump());
            }
            if (result == -2) {
                json errorBody;
                errorBody["error"] = "Product with this name and brand already exists";
                return crow::response(409, errorBody.dump());
            }

            json responseBody;
            responseBody["id"] = result;
            responseBody["message"] = "Product created successfully";
            return crow::response(201, responseBody.dump());
        }
    );

    // POST /api/batches
    CROW_ROUTE(app, "/api/batches").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            int productId = extractBodyInt(req.body, "productId", 0);
            std::string manufacturingDate = extractBodyField(req.body, "manufacturingDate");
            std::string expiryDate = extractBodyField(req.body, "expiryDate");
            double purchasePrice = extractBodyDouble(req.body, "purchasePrice", 0.0);
            double sellingPrice = extractBodyDouble(req.body, "sellingPrice", 0.0);
            int physicalQuantity = extractBodyInt(req.body, "physicalQuantity", 0);
            int supplierId = extractBodyInt(req.body, "supplierId", 0);

            int result = batchService.createBatch(productId, manufacturingDate, expiryDate,
                purchasePrice, sellingPrice, physicalQuantity, supplierId);

                if (result == -1) {
                    json errorBody; errorBody["error"] = "Invalid batch data";
                    return crow::response(400, errorBody.dump());
                }
                if (result == -2) {
                    json errorBody; errorBody["error"] = "Selling price must be >= purchase price";
                    return crow::response(400, errorBody.dump());
                }
                if (result == -3) {
                    json errorBody; errorBody["error"] = "Physical quantity cannot be negative";
                    return crow::response(400, errorBody.dump());
                }

                json responseBody;
                responseBody["id"] = result;
                responseBody["message"] = "Batch created successfully";
                return crow::response(201, responseBody.dump());
        }
    );

    // GET /api/batches/product/:id
    CROW_ROUTE(app, "/api/batches/product/<int>").methods(crow::HTTPMethod::Get)(
        [&](int productId) {
            auto batches = batchService.listBatchesByProduct(productId);
            json batchArray = json::array();
            for (auto& b : batches) batchArray.push_back(b.toJson());
            json body;
            body["batches"] = batchArray;
            return crow::response(200, body.dump());
        }
    );

    // GET /api/inventory/stock/:id
    CROW_ROUTE(app, "/api/inventory/stock/<int>").methods(crow::HTTPMethod::Get)(
        [&](int productId) {
            int stock = inventoryService.getAvailableStock(productId);
            json body;
            body["productId"] = productId;
            body["availableStock"] = stock;
            return crow::response(200, body.dump());
        }
    );

    // POST /api/inventory/reserve - reserve stock using FEFO
    CROW_ROUTE(app, "/api/inventory/reserve").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            int productId = extractBodyInt(req.body, "productId", 0);
            int quantity = extractBodyInt(req.body, "quantity", 0);

            if (productId <= 0 || quantity <= 0) {
                json errorBody;
                errorBody["error"] = "Invalid productId or quantity";
                return crow::response(400, errorBody.dump());
            }

            std::vector<InventoryService::BatchAllocation> allocations;
            bool success = inventoryService.reserveStockFEFO(productId, quantity, allocations);

                if (!success) {
                    json errorBody;
                    errorBody["error"] = "Insufficient stock";
                    errorBody["availableStock"] = inventoryService.getAvailableStock(productId);
                    return crow::response(400, errorBody.dump());
                }

                json allocArray = json::array();
                for (auto& a : allocations) {
                    json aObj;
                    aObj["batchId"] = a.batchId;
                    aObj["quantity"] = a.quantity;
                    allocArray.push_back(aObj);
                }

                json responseBody;
                responseBody["message"] = "Stock reserved successfully";
                responseBody["allocations"] = allocArray;
                return crow::response(200, responseBody.dump());
        }
    );

    // GET /api/inventory/expiry-alerts
    CROW_ROUTE(app, "/api/inventory/expiry-alerts").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            auto batches = batchService.listExpiredBatches();
            json alerts = json::array();
            int priority = 1;
            for (auto& b : batches) {
                json alert;
                alert["priority"] = priority++;
                alert["batchId"] = b.id();
                alert["productId"] = b.productId;
                alert["message"] = "Expired: batch #" + std::to_string(b.id()) + " expired on " + b.expiryDate;
                alert["type"] = "expiry";
                alerts.push_back(alert);
            }
            json body;
            body["alerts"] = alerts;
            return crow::response(200, body.dump());
        }
    );

    // GET /api/inventory/low-stock
    CROW_ROUTE(app, "/api/inventory/low-stock").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            int threshold = 10;
            if (req.url_params.get("threshold")) {
                threshold = std::stoi(req.url_params.get("threshold"));
            }
            auto batches = batchService.listLowStockBatches(threshold);
            json alerts = json::array();
            int priority = 1;
            for (auto& b : batches) {
                json alert;
                alert["priority"] = priority++;
                alert["batchId"] = b.id();
                alert["productId"] = b.productId;
                alert["message"] = "Low stock: batch #" + std::to_string(b.id()) + " has " +
                    std::to_string(b.availableStock()) + " units available";
                alert["type"] = "low_stock";
                alerts.push_back(alert);
            }
            json body;
            body["alerts"] = alerts;
            return crow::response(200, body.dump());
        }
    );
}
