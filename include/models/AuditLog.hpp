#pragma once

#include <string>

namespace medifind::models {

struct AuditLog {
    int id{0};
    int userId{0};
    std::string username;
    std::string action;        // "LOGIN_SUCCESS", "STOCK_IN", "SALE_COMPLETE", "ADJUSTMENT", "BACKUP_CREATED"
    std::string entityType;    // "MEDICINE", "BATCH", "SALE", "PURCHASE_ORDER", "USER", "SYSTEM"
    std::string entityId;      // e.g. "MED-1", "BAT-4", "INV-2026-0001"
    std::string changeSummary; // JSON or descriptive summary
    std::string timestamp;     // YYYY-MM-DD HH:MM:SS
    std::string clientContext; // "CONSOLE", "GUI", "CLI"
};

} // namespace medifind::models
