#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

enum class UserRole {
    Customer,
    Employee,
    Admin
};

inline UserRole stringToRole(const std::string& s) {
    if (s == "admin") return UserRole::Admin;
    if (s == "employee") return UserRole::Employee;
    return UserRole::Customer;
}

inline std::string roleToString(UserRole r) {
    switch (r) {
        case UserRole::Admin: return "admin";
        case UserRole::Employee: return "employee";
        default: return "customer";
    }
}

struct User {
private:
    int _id;

public:
    std::string email;
    std::string passwordHash;
    UserRole role;
    std::string fullName;
    std::string phone;
    std::string createdAt;

    User() : _id(0), role(UserRole::Customer) {}

    void setId(int newId) { _id = newId; }
    int id() const { return _id; }

    static std::string csvHeader();
    std::string toCsvRow() const;
    static User fromCsvRow(const std::vector<std::string>& fields);

    json toJson(bool includeSensitive = false) const;
};

struct Customer {
    int userId;
    std::string dateOfBirth;
    std::string gender;
    std::string createdAt;
};
