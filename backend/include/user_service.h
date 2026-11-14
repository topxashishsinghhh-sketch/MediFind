#pragma once
#include "user.h"
#include "repositories/user_repository.h"
#include <string>
#include <vector>
#include <optional>

class UserService {
private:
    UserRepository& userRepo;

public:
    explicit UserService(UserRepository& repo) : userRepo(repo) {}

    std::optional<User> getUserById(int id) {
        return userRepo.findById(id);
    }

    std::vector<User> listUsers() {
        return userRepo.findAll();
    }

    std::vector<User> listByRole(UserRole role) {
        std::vector<User> result;
        for (auto& u : userRepo.findAll()) {
            if (u.role == role) result.push_back(u);
        }
        return result;
    }

    bool updateUser(int id, const std::string& fullName, const std::string& phone) {
        auto opt = userRepo.findById(id);
        if (!opt.has_value()) return false;
        User u = opt.value();
        u.fullName = fullName;
        u.phone = phone;
        return userRepo.update(id, u);
    }

    bool changeRole(int id, UserRole newRole) {
        auto opt = userRepo.findById(id);
        if (!opt.has_value()) return false;
        User u = opt.value();
        u.role = newRole;
        return userRepo.update(id, u);
    }

    bool archiveUser(int id) {
        // Never permanently delete records referenced by historical transactions.
        // Instead we mark via role? Here we just return false - archiving handled by admin service.
        return false;
    }
};
