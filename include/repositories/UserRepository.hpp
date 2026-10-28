#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/User.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class UserRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit UserRepository(database::DatabaseConnection& db) : db_(db) {}

    int createUser(const models::User& user);
    std::optional<models::User> findById(int id);
    std::optional<models::User> findByUsername(const std::string& username);
    std::vector<models::User> listAll(bool includeInactive = false);
    bool updateUser(const models::User& user);
    bool updatePassword(int userId, const std::string& newHash, const std::string& newSalt);
    bool setActiveStatus(int userId, bool isActive);
    size_t count();
};

} // namespace medifind::repositories
