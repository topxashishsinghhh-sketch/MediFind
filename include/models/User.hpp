#pragma once

#include <string>

namespace medifind::models {

enum class UserRole {
    ADMINISTRATOR,
    EMPLOYEE
};

inline std::string userRoleToString(UserRole role) {
    switch (role) {
        case UserRole::ADMINISTRATOR: return "ADMINISTRATOR";
        case UserRole::EMPLOYEE:      return "EMPLOYEE";
    }
    return "EMPLOYEE";
}

inline UserRole stringToUserRole(const std::string& str) {
    if (str == "ADMINISTRATOR") return UserRole::ADMINISTRATOR;
    return UserRole::EMPLOYEE;
}

struct User {
    int id{0};
    std::string username;
    std::string passwordHash;
    std::string salt;
    std::string fullName;
    UserRole role{UserRole::EMPLOYEE};
    bool isActive{true};
    std::string createdAt;
};

} // namespace medifind::models
