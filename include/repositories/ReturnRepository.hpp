#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/ReturnRecord.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class ReturnRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit ReturnRepository(database::DatabaseConnection& db) : db_(db) {}

    int createReturn(const models::ReturnRecord& ret);
    std::optional<models::ReturnRecord> findById(int id);
    std::vector<models::ReturnRecord> listReturns(int limit = 100);
    std::string generateNextReturnRef();
};

} // namespace medifind::repositories
