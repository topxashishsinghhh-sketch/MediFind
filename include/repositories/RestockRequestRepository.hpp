#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/RestockRequest.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class RestockRequestRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit RestockRequestRepository(database::DatabaseConnection& db) : db_(db) {}

    int createRequest(const models::RestockRequest& req);
    std::optional<models::RestockRequest> findById(int id);
    std::vector<models::RestockRequest> listAll();
    std::vector<models::RestockRequest> listPending();
    bool updateStatus(int requestId, models::RestockStatus newStatus, int reviewerUserId, const std::string& reviewNotes);
    std::string generateNextRequestRef();
    size_t countPending();
};

} // namespace medifind::repositories
