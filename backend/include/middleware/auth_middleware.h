#pragma once
#include "crow.h"
#include "auth_service.h"
#include <string>
#include <functional>

// Middleware wrapper: protects a route handler with authentication + optional role check.
// Usage: CROW_ROUTE(app, "/api/protected")([&](const crow::request& req){
//     auto* ctx = req.get_user_context();
//     ...
// });
//
// We attach the authenticated User to crow::request via a middleware middleware object.

struct AuthContext {
    bool authenticated = false;
    User user;
};

class AuthMiddleware {
public:
    AuthService& authService;

    explicit AuthMiddleware(AuthService& svc) : authService(svc) {}

    // Called before the route handler. Sets req.get_user_context() if we add a context.
    // Crow uses middleware base class; we implement before_handle.
    struct context {
        std::optional<AuthContext> auth;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // Skip auth for public routes (handled in route registration instead)
        ctx.auth = std::nullopt;

        auto authHeader = req.get_header_value("Authorization");
        if (authHeader.empty()) return;

        std::string prefix = "Bearer ";
        if (authHeader.rfind(prefix, 0) == 0) {
            std::string token = authHeader.substr(prefix.size());
            auto optUser = authService.validateToken(token);
            if (optUser.has_value()) {
                AuthContext ac;
                ac.authenticated = true;
                ac.user = optUser.value();
                ctx.auth = ac;
            }
        }
    }

    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) {}
};

// Helper: require authentication. Returns 401 if not authenticated.
inline crow::response requireAuth(const std::optional<AuthContext>& auth) {
    if (!auth || !auth->authenticated) {
        json body;
        body["error"] = "Authentication required";
        return crow::response(401, body.dump());
    }
    return crow::response(200, "");
}

// Helper: require a specific role. Returns 403 if not authorized.
inline crow::response requireRole(const std::optional<AuthContext>& auth, UserRole role) {
    if (!auth || !auth->authenticated) {
        json body;
        body["error"] = "Authentication required";
        return crow::response(401, body.dump());
    }
    if (auth->user.role != role) {
        json body;
        body["error"] = "Insufficient permissions";
        return crow::response(403, body.dump());
    }
    return crow::response(200, "");
}

#include <nlohmann/json.hpp>
using json = nlohmann::json;
