#include "services/AuditService.hpp"

namespace medifind::services {

void AuditService::log(int userId,
                       const std::string& username,
                       const std::string& action,
                       const std::string& entityType,
                       const std::string& entityId,
                       const std::string& changeSummary,
                       const std::string& context) {
    models::AuditLog l;
    l.userId = userId;
    l.username = username;
    l.action = action;
    l.entityType = entityType;
    l.entityId = entityId;
    l.changeSummary = changeSummary;
    l.clientContext = context;
    auditRepo_.logEvent(l);
}

std::vector<models::AuditLog> AuditService::getRecentLogs(int limit, int offset, const std::string& actionFilter) {
    return auditRepo_.listLogs(limit, offset, actionFilter);
}

size_t AuditService::getTotalLogCount() {
    return auditRepo_.count();
}

} // namespace medifind::services
