#include "repositories/UserRepository.hpp"

namespace medifind::repositories {

int UserRepository::createUser(const models::User& user) {
    auto stmt = db_.prepare(R"(
        INSERT INTO users (username, password_hash, salt, full_name, role, is_active, created_at)
        VALUES (?, ?, ?, ?, ?, ?, datetime('now'));
    )");
    stmt.bindText(1, user.username);
    stmt.bindText(2, user.passwordHash);
    stmt.bindText(3, user.salt);
    stmt.bindText(4, user.fullName);
    stmt.bindText(5, models::userRoleToString(user.role));
    stmt.bindInt(6, user.isActive ? 1 : 0);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<models::User> UserRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, username, password_hash, salt, full_name, role, is_active, created_at FROM users WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::User u;
        u.id = stmt.getInt(0);
        u.username = stmt.getText(1);
        u.passwordHash = stmt.getText(2);
        u.salt = stmt.getText(3);
        u.fullName = stmt.getText(4);
        u.role = models::stringToUserRole(stmt.getText(5));
        u.isActive = (stmt.getInt(6) != 0);
        u.createdAt = stmt.getText(7);
        return u;
    }
    return std::nullopt;
}

std::optional<models::User> UserRepository::findByUsername(const std::string& username) {
    auto stmt = db_.prepare("SELECT id, username, password_hash, salt, full_name, role, is_active, created_at FROM users WHERE username = ?;");
    stmt.bindText(1, username);
    if (stmt.step()) {
        models::User u;
        u.id = stmt.getInt(0);
        u.username = stmt.getText(1);
        u.passwordHash = stmt.getText(2);
        u.salt = stmt.getText(3);
        u.fullName = stmt.getText(4);
        u.role = models::stringToUserRole(stmt.getText(5));
        u.isActive = (stmt.getInt(6) != 0);
        u.createdAt = stmt.getText(7);
        return u;
    }
    return std::nullopt;
}

std::vector<models::User> UserRepository::listAll(bool includeInactive) {
    std::string sql = "SELECT id, username, password_hash, salt, full_name, role, is_active, created_at FROM users";
    if (!includeInactive) {
        sql += " WHERE is_active = 1";
    }
    sql += " ORDER BY id ASC;";

    auto stmt = db_.prepare(sql);
    std::vector<models::User> users;
    while (stmt.step()) {
        models::User u;
        u.id = stmt.getInt(0);
        u.username = stmt.getText(1);
        u.passwordHash = stmt.getText(2);
        u.salt = stmt.getText(3);
        u.fullName = stmt.getText(4);
        u.role = models::stringToUserRole(stmt.getText(5));
        u.isActive = (stmt.getInt(6) != 0);
        u.createdAt = stmt.getText(7);
        users.push_back(u);
    }
    return users;
}

bool UserRepository::updateUser(const models::User& user) {
    auto stmt = db_.prepare(R"(
        UPDATE users SET full_name = ?, role = ?, is_active = ? WHERE id = ?;
    )");
    stmt.bindText(1, user.fullName);
    stmt.bindText(2, models::userRoleToString(user.role));
    stmt.bindInt(3, user.isActive ? 1 : 0);
    stmt.bindInt(4, user.id);
    stmt.step();
    return db_.changes() > 0;
}

bool UserRepository::updatePassword(int userId, const std::string& newHash, const std::string& newSalt) {
    auto stmt = db_.prepare("UPDATE users SET password_hash = ?, salt = ? WHERE id = ?;");
    stmt.bindText(1, newHash);
    stmt.bindText(2, newSalt);
    stmt.bindInt(3, userId);
    stmt.step();
    return db_.changes() > 0;
}

bool UserRepository::setActiveStatus(int userId, bool isActive) {
    auto stmt = db_.prepare("UPDATE users SET is_active = ? WHERE id = ?;");
    stmt.bindInt(1, isActive ? 1 : 0);
    stmt.bindInt(2, userId);
    stmt.step();
    return db_.changes() > 0;
}

size_t UserRepository::count() {
    auto stmt = db_.prepare("SELECT COUNT(*) FROM users;");
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
