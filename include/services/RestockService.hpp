#pragma once

#include "database/DatabaseConnection.hpp"
#include "repositories/RestockRequestRepository.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "models/RestockRequest.hpp"
#include "dsa/Queue.hpp"
#include <vector>
#include <string>
#include <optional>

namespace medifind::services {

class RestockService {
private:
    database::DatabaseConnection& db_;
    repositories::RestockRequestRepository& restockRepo_;
    repositories::MedicineRepository& medRepo_;
    repositories::AuditLogRepository& auditRepo_;

    // DSA Queue demonstrating FIFO restock dispatch
    dsa::Queue<models::RestockRequest> requestQueue_;

public:
    RestockService(database::DatabaseConnection& db,
                   repositories::RestockRequestRepository& restockRepo,
                   repositories::MedicineRepository& medRepo,
                   repositories::AuditLogRepository& auditRepo);

    void refreshQueue();

    int submitRequest(int medicineId, int requestedQuantity, const std::string& reason, int requestedByUserId, std::string& outError);
    bool reviewRequest(int requestId, models::RestockStatus newStatus, int reviewerUserId, const std::string& reviewNotes, std::string& outError);
    std::optional<models::RestockRequest> popNextPendingFromQueue();

    std::vector<models::RestockRequest> listAllRequests();
    std::vector<models::RestockRequest> listPendingRequests();
    size_t getPendingCount();
};

} // namespace medifind::services
