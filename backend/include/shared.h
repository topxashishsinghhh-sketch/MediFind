#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct CartItem {
    int productId;
    int quantity;
};

struct Cart {
    int customerId;
    std::vector<CartItem> items;
};

struct Address {
    int id;
    int customerId;
    std::string label;           // home, office, etc.
    std::string street;
    std::string city;
    std::string state;
    std::string postalCode;
    std::string phone;
    bool isDefault;
};

struct Prescription {
    int id;
    int customerId;
    int orderId;                 // optional, links to order
    std::string filePath;        // protected reference, never exposed directly
    std::string status;          // pending, approved, rejected, clarification_required
    std::string reviewerId;      // employee who reviewed
    std::string decisionReason;
    std::string createdAt;
    std::string reviewedAt;
};

struct Supplier {
    int id;
    std::string name;
    std::string contactPerson;
    std::string phone;
    std::string email;
    std::string address;
};

struct Notification {
    int id;
    int userId;
    std::string title;
    std::string message;
    std::string type;            // info, warning, alert, success
    bool isRead;
    std::string createdAt;
};

struct AuditLog {
    int id;
    int userId;
    std::string action;
    std::string entityType;
    int entityId;
    std::string details;
    std::string createdAt;
};
