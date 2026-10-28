#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/AuditLog.hpp"
#include <vector>

namespace medifind::repositories {

class AuditLogRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit AuditLogRepository(database::DatabaseConnection& db) : db_(db) {}

    int logEvent(const models::AuditLog& log);
    std::vector<models::AuditLog> listLogs(int limit = 100, int offset = 0, const std::string& actionFilter = "");
    size_t count();
};

} // namespace medifind::repositories
