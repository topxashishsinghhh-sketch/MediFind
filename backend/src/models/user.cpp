#include "user.h"
#include "repositories/csv_repository.h"
#include <sstream>

std::string User::csvHeader() {
    return "id,email,passwordHash,role,fullName,phone,createdAt";
}

std::string User::toCsvRow() const {
    std::ostringstream oss;
    oss << _id << ","
        << csv::escapeField(email) << ","
        << csv::escapeField(passwordHash) << ","
        << roleToString(role) << ","
        << csv::escapeField(fullName) << ","
        << csv::escapeField(phone) << ","
        << csv::escapeField(createdAt);
    return oss.str();
}

User User::fromCsvRow(const std::vector<std::string>& fields) {
    User u;
    u._id = std::stoi(fields[0]);
    u.email = fields[1];
    u.passwordHash = fields[2];
    u.role = stringToRole(fields[3]);
    u.fullName = fields[4];
    u.phone = fields[5];
    u.createdAt = fields.size() > 6 ? fields[6] : "";
    return u;
}

json User::toJson(bool includeSensitive) const {
    json j;
    j["id"] = _id;
    j["email"] = email;
    j["role"] = roleToString(role);
    j["fullName"] = fullName;
    j["phone"] = phone;
    j["createdAt"] = createdAt;
    if (includeSensitive) {
        j["passwordHash"] = passwordHash;
    }
    return j;
}
