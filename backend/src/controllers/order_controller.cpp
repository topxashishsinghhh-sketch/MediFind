#include "crow.h"
#include "order_service.h"
#include "repositories/order_repository.h"
#include "repositories/product_repository.h"
#include "inventory_service.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Crow's request body sometimes loses JSON quotes (e.g. {customerId:1,items:[{productId:1,quantity:2}]}).
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

static std::string extractBodyString(const std::string& body, const std::string& key) {
    return extractBodyField(body, key);
}

// Parse items array from Crow body: items:[{productId:1,quantity:2},{productId:3,quantity:1}]
static std::vector<std::pair<int, int>> extractBodyItems(const std::string& body) {
    std::vector<std::pair<int, int>> items;
    std::string key = "items:[";
    size_t pos = body.find(key);
    if (pos == std::string::npos) return items;
    size_t start = pos + key.size();
    size_t end = body.find("]", start);
    if (end == std::string::npos) return items;
    std::string itemsStr = body.substr(start, end - start);

    // Find each {productId:X,quantity:Y} block
    size_t i = 0;
    while (i < itemsStr.size()) {
        size_t ob = itemsStr.find("{", i);
        if (ob == std::string::npos) break;
        size_t cb = itemsStr.find("}", ob);
        if (cb == std::string::npos) break;
        std::string itemStr = itemsStr.substr(ob + 1, cb - ob - 1);

        int productId = 0, quantity = 0;
        size_t pidPos = itemStr.find("productId:");
        if (pidPos != std::string::npos) {
            size_t valStart = pidPos + 10;
            size_t valEnd = itemStr.find(",", valStart);
            if (valEnd == std::string::npos) valEnd = itemStr.size();
            std::string pidStr = itemStr.substr(valStart, valEnd - valStart);
            size_t first = pidStr.find_first_not_of(" \t");
            size_t last = pidStr.find_last_not_of(" \t");
            if (first != std::string::npos && last != std::string::npos) {
                try { productId = std::stoi(pidStr.substr(first, last - first + 1)); } catch (...) {}
            }
        }
        size_t qtyPos = itemStr.find("quantity:");
        if (qtyPos != std::string::npos) {
            size_t valStart = qtyPos + 9;
            size_t valEnd = itemStr.find(",", valStart);
            if (valEnd == std::string::npos) valEnd = itemStr.size();
            std::string qtyStr = itemStr.substr(valStart, valEnd - valStart);
            size_t first = qtyStr.find_first_not_of(" \t");
            size_t last = qtyStr.find_last_not_of(" \t");
            if (first != std::string::npos && last != std::string::npos) {
                try { quantity = std::stoi(qtyStr.substr(first, last - first + 1)); } catch (...) {}
            }
        }

        if (productId > 0 && quantity > 0) {
            items.push_back({productId, quantity});
        }
        i = cb + 1;
    }
    return items;
}

void setupOrderRoutes(crow::SimpleApp& app, OrderService& orderService) {
    // GET /api/orders - list orders (optional ?customerId=N)
    CROW_ROUTE(app, "/api/orders").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            // Parse query params manually
            int customerId = -1;
            auto url = req.url;
            size_t qPos = url.find("?");
            if (qPos != std::string::npos) {
                std::string qs = url.substr(qPos + 1);
                size_t cidPos = qs.find("customerId=");
                if (cidPos != std::string::npos) {
                    size_t valStart = cidPos + 11;
                    size_t valEnd = qs.find("&", valStart);
                    if (valEnd == std::string::npos) valEnd = qs.size();
                    std::string cidStr = qs.substr(valStart, valEnd - valStart);
                    try { customerId = std::stoi(cidStr); } catch (...) {}
                }
            }

            auto orders = orderService.listOrders(customerId);
            json arr = json::array();
            for (auto& o : orders) {
                arr.push_back(o.toJson());
            }
            json body;
            body["orders"] = arr;
            return crow::response(200, body.dump());
        }
    );

    // GET /api/orders/<int>
    CROW_ROUTE(app, "/api/orders/<int>").methods(crow::HTTPMethod::Get)(
        [&](int orderId) {
            auto opt = orderService.getOrder(orderId);
            if (!opt.has_value()) {
                json body; body["error"] = "Order not found";
                return crow::response(404, body.dump());
            }
            return crow::response(200, opt.value().toJson().dump());
        }
    );

    // POST /api/orders - create order
    CROW_ROUTE(app, "/api/orders").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            int customerId = extractBodyInt(req.body, "customerId", 0);
            std::string deliveryAddress = extractBodyString(req.body, "deliveryAddress");

            if (customerId <= 0) {
                json body; body["error"] = "Invalid customerId";
                return crow::response(400, body.dump());
            }
            if (deliveryAddress.empty()) {
                json body; body["error"] = "Delivery address is required";
                return crow::response(400, body.dump());
            }

            auto items = extractBodyItems(req.body);
            if (items.empty()) {
                json body; body["error"] = "No items in order";
                return crow::response(400, body.dump());
            }

            std::vector<CartItemRequest> cartItems;
            for (auto& [pid, qty] : items) {
                cartItems.push_back({pid, qty});
            }

            int orderId = orderService.createOrder(customerId, cartItems, deliveryAddress);
            if (orderId == -1) {
                json body; body["error"] = "No items in order";
                return crow::response(400, body.dump());
            }
            if (orderId == -2) {
                json body; body["error"] = "Delivery address is required";
                return crow::response(400, body.dump());
            }
            if (orderId == -3) {
                json body; body["error"] = "Invalid item data";
                return crow::response(400, body.dump());
            }
            if (orderId == -4) {
                json body; body["error"] = "Product not found";
                return crow::response(404, body.dump());
            }
            if (orderId == -5) {
                json body; body["error"] = "Insufficient stock";
                return crow::response(400, body.dump());
            }

            json body;
            body["id"] = orderId;
            body["message"] = "Order created successfully";
            return crow::response(201, body.dump());
        }
    );

    // POST /api/orders/<int>/cancel
    CROW_ROUTE(app, "/api/orders/<int>/cancel").methods(crow::HTTPMethod::Post)(
        [&](int orderId) {
            if (orderService.cancelOrder(orderId)) {
                json body; body["message"] = "Order cancelled";
                return crow::response(200, body.dump());
            }
            json body; body["error"] = "Cannot cancel order";
            return crow::response(400, body.dump());
        }
    );

    // POST /api/orders/<int>/pay
    CROW_ROUTE(app, "/api/orders/<int>/pay").methods(crow::HTTPMethod::Post)(
        [&](int orderId) {
            if (orderService.markPaid(orderId)) {
                json body; body["message"] = "Payment successful";
                return crow::response(200, body.dump());
            }
            json body; body["error"] = "Cannot process payment";
            return crow::response(400, body.dump());
        }
    );

    // POST /api/orders/<int>/processing
    CROW_ROUTE(app, "/api/orders/<int>/processing").methods(crow::HTTPMethod::Post)(
        [&](int orderId) {
            if (orderService.markProcessing(orderId)) {
                json body; body["message"] = "Order processing";
                return crow::response(200, body.dump());
            }
            json body; body["error"] = "Cannot update order status";
            return crow::response(400, body.dump());
        }
    );

    // POST /api/orders/<int>/dispatched
    CROW_ROUTE(app, "/api/orders/<int>/dispatched").methods(crow::HTTPMethod::Post)(
        [&](int orderId) {
            if (orderService.markDispatched(orderId)) {
                json body; body["message"] = "Order dispatched";
                return crow::response(200, body.dump());
            }
            json body; body["error"] = "Cannot update order status";
            return crow::response(400, body.dump());
        }
    );

    // POST /api/orders/<int>/delivered
    CROW_ROUTE(app, "/api/orders/<int>/delivered").methods(crow::HTTPMethod::Post)(
        [&](int orderId) {
            if (orderService.markDelivered(orderId)) {
                json body; body["message"] = "Order delivered";
                return crow::response(200, body.dump());
            }
            json body; body["error"] = "Cannot update order status";
            return crow::response(400, body.dump());
        }
    );
}
