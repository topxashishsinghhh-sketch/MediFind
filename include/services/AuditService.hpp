#pragma once

#include "repositories/AuditLogRepository.hpp"
#include "models/AuditLog.hpp"
#include <vector>
#include <string>

namespace medifind::services {

class AuditService {
private:
    repositories::AuditLogRepository& auditRepo_;

public:
    explicit AuditService(repositories::AuditLogRepository& auditRepo) : auditRepo_(auditRepo) {}

    void log(int userId,
             const std::string& username,
             const std::string& action,
             const std::string& entityType,
             const std::string& entityId,
             const std::string& changeSummary,
             const std::string& context = "APP");

    std::vector<models::AuditLog> getRecentLogs(int limit = 100, int offset = 0, const std::string& actionFilter = "");
    size_t getTotalLogCount();
};

} // namespace medifind::services
