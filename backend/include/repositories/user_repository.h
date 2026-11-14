#pragma once
#include "user.h"
#include "repositories/csv_repository.h"
#include <string>

class UserRepository : public CsvRepository<User> {
public:
    UserRepository(const std::string& filePath) : CsvRepository<User>(filePath) {}

    std::optional<User> findByEmail(const std::string& email) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& [id, user] : cache) {
            if (user.email == email) return user;
        }
        return std::nullopt;
    }

    bool emailExists(const std::string& email) {
        return findByEmail(email).has_value();
    }
};
