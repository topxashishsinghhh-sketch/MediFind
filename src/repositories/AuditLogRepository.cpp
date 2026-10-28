#include "repositories/AuditLogRepository.hpp"

namespace medifind::repositories {

int AuditLogRepository::logEvent(const models::AuditLog& log) {
    auto stmt = db_.prepare(R"(
        INSERT INTO audit_logs (user_id, username, action, entity_type, entity_id, change_summary, timestamp, client_context)
        VALUES (?, ?, ?, ?, ?, ?, datetime('now'), ?);
    )");
    if (log.userId <= 0) stmt.bindNull(1);
    else stmt.bindInt(1, log.userId);

    stmt.bindText(2, log.username);
    stmt.bindText(3, log.action);
    stmt.bindText(4, log.entityType);
    stmt.bindText(5, log.entityId);
    stmt.bindText(6, log.changeSummary);
    stmt.bindText(7, log.clientContext.empty() ? "CONSOLE" : log.clientContext);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::vector<models::AuditLog> AuditLogRepository::listLogs(int limit, int offset, const std::string& actionFilter) {
    std::string sql = "SELECT id, user_id, username, action, entity_type, entity_id, change_summary, timestamp, client_context FROM audit_logs";
    if (!actionFilter.empty()) {
        sql += " WHERE action = ?";
    }
    sql += " ORDER BY id DESC LIMIT ? OFFSET ?;";

    auto stmt = db_.prepare(sql);
    int pIdx = 1;
    if (!actionFilter.empty()) {
        stmt.bindText(pIdx++, actionFilter);
    }
    stmt.bindInt(pIdx++, limit);
    stmt.bindInt(pIdx++, offset);

    std::vector<models::AuditLog> logs;
    while (stmt.step()) {
        models::AuditLog l;
        l.id = stmt.getInt(0);
        l.userId = stmt.isNull(1) ? 0 : stmt.getInt(1);
        l.username = stmt.getText(2);
        l.action = stmt.getText(3);
        l.entityType = stmt.getText(4);
        l.entityId = stmt.getText(5);
        l.changeSummary = stmt.getText(6);
        l.timestamp = stmt.getText(7);
        l.clientContext = stmt.getText(8);
        logs.push_back(l);
    }
    return logs;
}

size_t AuditLogRepository::count() {
    auto stmt = db_.prepare("SELECT COUNT(*) FROM audit_logs;");
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
