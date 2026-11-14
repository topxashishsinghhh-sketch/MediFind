#pragma once
#include "user.h"
#include "repositories/user_repository.h"
#include "password_hasher.h"
#include <string>
#include <optional>
#include <map>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

// Result of authentication
struct AuthResult {
    bool success;
    std::string token;
    User user;
    std::string error;
};

class AuthService {
private:
    UserRepository& userRepo;
    std::map<std::string, User> tokens; // token -> user

    std::string generateToken() {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
        std::ostringstream oss;
        oss << std::hex;
        for (int i = 0; i < 32; ++i) oss << dis(gen);
        return oss.str();
    }

    static std::string currentTimeIso() {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&timeT), "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

public:
    explicit AuthService(UserRepository& repo) : userRepo(repo) {}

    // Register a new user
    AuthResult registerUser(const std::string& email, const std::string& password,
                            const std::string& fullName, const std::string& phone,
                            UserRole role = UserRole::Customer) {
        AuthResult result;
        result.success = false;

        if (email.empty() || password.empty() || fullName.empty()) {
            result.error = "Email, password, and full name are required.";
            return result;
        }
        if (password.size() < 6) {
            result.error = "Password must be at least 6 characters.";
            return result;
        }

        if (userRepo.emailExists(email)) {
            result.error = "Email already registered.";
            return result;
        }

        std::string hash = PasswordHasher::hash(password);
        User user;
        user.setId(0);
        user.email = email;
        user.passwordHash = hash;
        user.role = role;
        user.fullName = fullName;
        user.phone = phone;
        user.createdAt = currentTimeIso();

        int newId = userRepo.create(user);
        user.setId(newId);

        result.success = true;
        result.user = user;
        result.token = generateToken();
        tokens[result.token] = user;
        return result;
    }

    // Login: verify email + password, return token
    AuthResult login(const std::string& email, const std::string& password) {
        AuthResult result;
        result.success = false;

        auto optUser = userRepo.findByEmail(email);
        if (!optUser.has_value()) {
            result.error = "Invalid email or password.";
            return result;
        }
        User user = optUser.value();

        if (!PasswordHasher::verify(password, user.passwordHash)) {
            result.error = "Invalid email or password.";
            return result;
        }

        result.success = true;
        result.user = user;
        result.token = generateToken();
        tokens[result.token] = user;
        return result;
    }

    // Validate a token and return the associated user
    std::optional<User> validateToken(const std::string& token) {
        auto it = tokens.find(token);
        if (it == tokens.end()) return std::nullopt;
        return it->second;
    }

    void logout(const std::string& token) {
        tokens.erase(token);
    }

    bool isAdmin(const User& user) { return user.role == UserRole::Admin; }
    bool isEmployee(const User& user) { return user.role == UserRole::Employee; }
    bool isCustomer(const User& user) { return user.role == UserRole::Customer; }
};
