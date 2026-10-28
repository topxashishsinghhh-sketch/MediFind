#include "services/AuthService.hpp"
#include "utils/CryptoUtils.hpp"

namespace medifind::services {

AuthResult AuthService::login(const std::string& username, const std::string& password, const std::string& context) {
    if (username.empty() || password.empty()) {
        return {false, "Username and password cannot be empty.", std::nullopt};
    }

    auto userOpt = userRepo_.findByUsername(username);
    if (!userOpt) {
        models::AuditLog log;
        log.username = username;
        log.action = "LOGIN_FAILED";
        log.entityType = "USER";
        log.entityId = username;
        log.changeSummary = "User not found";
        log.clientContext = context;
        auditRepo_.logEvent(log);
        return {false, "Invalid username or password.", std::nullopt};
    }

    const auto& user = *userOpt;
    if (!user.isActive) {
        models::AuditLog log;
        log.userId = user.id;
        log.username = user.username;
        log.action = "LOGIN_FAILED";
        log.entityType = "USER";
        log.entityId = std::to_string(user.id);
        log.changeSummary = "Account is inactive";
        log.clientContext = context;
        auditRepo_.logEvent(log);
        return {false, "Account has been deactivated. Please contact administrator.", std::nullopt};
    }

    bool matched = utils::CryptoUtils::verifyPassword(password, user.salt, user.passwordHash);
    if (!matched) {
        models::AuditLog log;
        log.userId = user.id;
        log.username = user.username;
        log.action = "LOGIN_FAILED";
        log.entityType = "USER";
        log.entityId = std::to_string(user.id);
        log.changeSummary = "Incorrect password";
        log.clientContext = context;
        auditRepo_.logEvent(log);
        return {false, "Invalid username or password.", std::nullopt};
    }

    currentUser_ = user;

    models::AuditLog log;
    log.userId = user.id;
    log.username = user.username;
    log.action = "LOGIN_SUCCESS";
    log.entityType = "USER";
    log.entityId = std::to_string(user.id);
    log.changeSummary = "Logged in successfully as " + models::userRoleToString(user.role);
    log.clientContext = context;
    auditRepo_.logEvent(log);

    return {true, "Login successful.", user};
}

void AuthService::logout() {
    if (currentUser_) {
        models::AuditLog log;
        log.userId = currentUser_->id;
        log.username = currentUser_->username;
        log.action = "LOGOUT";
        log.entityType = "USER";
        log.entityId = std::to_string(currentUser_->id);
        log.changeSummary = "User logged out";
        log.clientContext = "SESSION";
        auditRepo_.logEvent(log);
        currentUser_.reset();
    }
}

bool AuthService::bootstrapAdminIfEmpty(const std::string& username, const std::string& password, const std::string& fullName) {
    if (userRepo_.count() > 0) {
        return false; // Already has users
    }

    std::string salt = utils::CryptoUtils::generateSalt(16);
    std::string hash = utils::CryptoUtils::hashPassword(password, salt);

    models::User admin;
    admin.username = username;
    admin.passwordHash = hash;
    admin.salt = salt;
    admin.fullName = fullName;
    admin.role = models::UserRole::ADMINISTRATOR;
    admin.isActive = true;

    int newId = userRepo_.createUser(admin);

    models::AuditLog log;
    log.userId = newId;
    log.username = username;
    log.action = "INITIAL_ADMIN_CREATED";
    log.entityType = "USER";
    log.entityId = std::to_string(newId);
    log.changeSummary = "Bootstrap initial administrator account";
    log.clientContext = "SYSTEM";
    auditRepo_.logEvent(log);

    return newId > 0;
}

AuthResult AuthService::registerUser(const std::string& username, const std::string& password, const std::string& fullName, models::UserRole role) {
    if (userRepo_.count() > 0 && !isAdmin()) {
        return {false, "Permission denied: Only administrators can register new users.", std::nullopt};
    }

    if (username.length() < 3) {
        return {false, "Username must be at least 3 characters long.", std::nullopt};
    }
    if (password.length() < 6) {
        return {false, "Password must be at least 6 characters long.", std::nullopt};
    }
    if (userRepo_.findByUsername(username)) {
        return {false, "Username '" + username + "' is already taken.", std::nullopt};
    }

    std::string salt = utils::CryptoUtils::generateSalt(16);
    std::string hash = utils::CryptoUtils::hashPassword(password, salt);

    models::User newUser;
    newUser.username = username;
    newUser.passwordHash = hash;
    newUser.salt = salt;
    newUser.fullName = fullName;
    newUser.role = role;
    newUser.isActive = true;

    int newId = userRepo_.createUser(newUser);
    newUser.id = newId;

    models::AuditLog log;
    log.userId = currentUser_ ? currentUser_->id : newId;
    log.username = currentUser_ ? currentUser_->username : username;
    log.action = "USER_REGISTERED";
    log.entityType = "USER";
    log.entityId = std::to_string(newId);
    log.changeSummary = "Created user " + username + " with role " + models::userRoleToString(role);
    log.clientContext = "APP";
    auditRepo_.logEvent(log);

    return {true, "User created successfully.", newUser};
}

bool AuthService::changePassword(const std::string& oldPassword, const std::string& newPassword, std::string& outError) {
    if (!currentUser_) {
        outError = "Not authenticated.";
        return false;
    }
    if (newPassword.length() < 6) {
        outError = "New password must be at least 6 characters long.";
        return false;
    }

    bool matched = utils::CryptoUtils::verifyPassword(oldPassword, currentUser_->salt, currentUser_->passwordHash);
    if (!matched) {
        outError = "Current password is incorrect.";
        return false;
    }

    std::string newSalt = utils::CryptoUtils::generateSalt(16);
    std::string newHash = utils::CryptoUtils::hashPassword(newPassword, newSalt);

    bool updated = userRepo_.updatePassword(currentUser_->id, newHash, newSalt);
    if (updated) {
        currentUser_->passwordHash = newHash;
        currentUser_->salt = newSalt;

        models::AuditLog log;
        log.userId = currentUser_->id;
        log.username = currentUser_->username;
        log.action = "PASSWORD_CHANGED";
        log.entityType = "USER";
        log.entityId = std::to_string(currentUser_->id);
        log.changeSummary = "User changed own password";
        log.clientContext = "APP";
        auditRepo_.logEvent(log);
        return true;
    }

    outError = "Database error updating password.";
    return false;
}

} // namespace medifind::services
