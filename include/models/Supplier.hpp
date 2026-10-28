#pragma once

#include <string>

namespace medifind::models {

struct Supplier {
    int id{0};
    std::string name;
    std::string contactPerson;
    std::string email;
    std::string phone;
    std::string address;
    bool isActive{true};
    std::string createdAt;
};

} // namespace medifind::models
