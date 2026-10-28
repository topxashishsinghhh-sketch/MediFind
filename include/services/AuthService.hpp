#pragma once

#include "repositories/UserRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "models/User.hpp"
#include <optional>
#include <memory>

namespace medifind::services {

struct AuthResult {
    bool success{false};
    std::string message;
    std::optional<models::User> user;
};

class AuthService {
private:
    repositories::UserRepository& userRepo_;
    repositories::AuditLogRepository& auditRepo_;
    std::optional<models::User> currentUser_;

public:
    AuthService(repositories::UserRepository& userRepo, repositories::AuditLogRepository& auditRepo)
        : userRepo_(userRepo), auditRepo_(auditRepo) {}

    /**
     * @brief Logs in user with username and password.
     */
    AuthResult login(const std::string& username, const std::string& password, const std::string& context = "CONSOLE");

    /**
     * @brief Clears current active session.
     */
    void logout();

    bool isAuthenticated() const { return currentUser_.has_value(); }
    const std::optional<models::User>& getCurrentUser() const { return currentUser_; }
    bool isAdmin() const { return currentUser_.has_value() && currentUser_->role == models::UserRole::ADMINISTRATOR; }

    /**
     * @brief Ensures at least one administrator exists in the system.
     * If no users exist, creates the initial admin account.
     */
    bool bootstrapAdminIfEmpty(const std::string& username, const std::string& password, const std::string& fullName);

    /**
     * @brief Creates a new user (Requires Admin role or bootstrap).
     */
    AuthResult registerUser(const std::string& username, const std::string& password, const std::string& fullName, models::UserRole role);

    /**
     * @brief Changes current logged-in user password.
     */
    bool changePassword(const std::string& oldPassword, const std::string& newPassword, std::string& outError);
};

} // namespace medifind::services
