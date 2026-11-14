#include "crow.h"
#include "auth_service.h"
#include "user_service.h"
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Crow's request body sometimes loses JSON quotes (e.g. {email:test@test.com,password:123456}).
// This helper extracts a string field value from either standard JSON or Crow's modified format.
static std::string extractBodyField(const std::string& body, const std::string& key) {
    // Try standard JSON: "key":"value"
    std::string quotedKey = "\"" + key + "\":\"";
    size_t pos = body.find(quotedKey);
    if (pos != std::string::npos) {
        size_t start = pos + quotedKey.size();
        size_t end = body.find("\"", start);
        if (end != std::string::npos) return body.substr(start, end - start);
    }
    // Fallback: Crow's modified format {key:value,key2:value2}
    std::string plainKey = key + ":";
    pos = body.find(plainKey);
    if (pos != std::string::npos) {
        size_t start = pos + plainKey.size();
        size_t end = body.find(",", start);
        if (end == std::string::npos) end = body.find("}", start);
        if (end != std::string::npos) {
            std::string val = body.substr(start, end - start);
            // Trim surrounding whitespace
            size_t first = val.find_first_not_of(" \t");
            size_t last = val.find_last_not_of(" \t");
            if (first != std::string::npos && last != std::string::npos)
                return val.substr(first, last - first + 1);
        }
    }
    // Try unquoted key with no colon separator issues
    return "";
}

void setupAuthRoutes(crow::SimpleApp& app, AuthService& authService, UserService& /*userService*/) {

    // POST /api/auth/register
    CROW_ROUTE(app, "/api/auth/register").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            std::string email = extractBodyField(req.body, "email");
            std::string password = extractBodyField(req.body, "password");
            std::string fullName = extractBodyField(req.body, "fullName");
            std::string phone = extractBodyField(req.body, "phone");
            std::string roleStr = extractBodyField(req.body, "role");

            if (email.empty() || password.empty() || fullName.empty()) {
                json errorBody;
                errorBody["error"] = "Email, password, and full name are required.";
                return crow::response(400, errorBody.dump());
            }
            if (password.size() < 6) {
                json errorBody;
                errorBody["error"] = "Password must be at least 6 characters.";
                return crow::response(400, errorBody.dump());
            }

            UserRole role = UserRole::Customer;
            if (roleStr == "admin") role = UserRole::Admin;
            else if (roleStr == "employee") role = UserRole::Employee;

            auto result = authService.registerUser(email, password, fullName, phone, role);
            if (!result.success) {
                json errorBody;
                errorBody["error"] = result.error;
                return crow::response(400, errorBody.dump());
            }
            json body;
            body["token"] = result.token;
            body["user"] = result.user.toJson();
            body["message"] = "Registration successful";
            return crow::response(201, body.dump());
        }
    );

    // POST /api/auth/login
    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            std::string email = extractBodyField(req.body, "email");
            std::string password = extractBodyField(req.body, "password");

            auto result = authService.login(email, password);
            if (!result.success) {
                json errorBody;
                errorBody["error"] = result.error;
                return crow::response(401, errorBody.dump());
            }
            json body;
            body["token"] = result.token;
            body["user"] = result.user.toJson();
            body["message"] = "Login successful";
            return crow::response(200, body.dump());
        }
    );

    // GET /api/auth/me
    CROW_ROUTE(app, "/api/auth/me").methods(crow::HTTPMethod::Get)(
        [&](const crow::request& req) {
            auto authHeader = req.get_header_value("Authorization");
            if (authHeader.empty()) {
                json errorBody;
                errorBody["error"] = "Authentication required";
                return crow::response(401, errorBody.dump());
            }
            std::string prefix = "Bearer ";
            if (authHeader.rfind(prefix, 0) != 0) {
                json errorBody;
                errorBody["error"] = "Invalid authorization header";
                return crow::response(401, errorBody.dump());
            }
            std::string token = authHeader.substr(prefix.size());
            auto optUser = authService.validateToken(token);
            if (!optUser.has_value()) {
                json errorBody;
                errorBody["error"] = "Invalid or expired token";
                return crow::response(401, errorBody.dump());
            }
            json body;
            body["user"] = optUser.value().toJson();
            return crow::response(200, body.dump());
        }
    );

    // POST /api/auth/logout
    CROW_ROUTE(app, "/api/auth/logout").methods(crow::HTTPMethod::Post)(
        [&](const crow::request& req) {
            auto authHeader = req.get_header_value("Authorization");
            if (!authHeader.empty()) {
                std::string prefix = "Bearer ";
                if (authHeader.rfind(prefix, 0) == 0) {
                    std::string token = authHeader.substr(prefix.size());
                    authService.logout(token);
                }
            }
            json body;
            body["message"] = "Logged out";
            return crow::response(200, body.dump());
        }
    );
}
