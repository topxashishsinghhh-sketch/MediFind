#include "services/RestockService.hpp"

namespace medifind::services {

RestockService::RestockService(database::DatabaseConnection& db,
                               repositories::RestockRequestRepository& restockRepo,
                               repositories::MedicineRepository& medRepo,
                               repositories::AuditLogRepository& auditRepo)
    : db_(db), restockRepo_(restockRepo), medRepo_(medRepo), auditRepo_(auditRepo) {
    refreshQueue();
}

void RestockService::refreshQueue() {
    requestQueue_.clear();
    auto pending = restockRepo_.listPending();
    for (const auto& req : pending) {
        requestQueue_.enqueue(req);
    }
}

int RestockService::submitRequest(int medicineId, int requestedQuantity, const std::string& reason, int requestedByUserId, std::string& outError) {
    if (requestedQuantity <= 0) {
        outError = "Requested restock quantity must be positive.";
        return -1;
    }

    auto med = medRepo_.findById(medicineId);
    if (!med) {
        outError = "Medicine not found.";
        return -1;
    }

    models::RestockRequest req;
    req.requestRef = restockRepo_.generateNextRequestRef();
    req.medicineId = medicineId;
    req.requestedQuantity = requestedQuantity;
    req.reason = reason.empty() ? "Low stock depletion" : reason;
    req.status = models::RestockStatus::PENDING;
    req.requestedByUserId = requestedByUserId;

    int reqId = restockRepo_.createRequest(req);
    if (reqId > 0) {
        req.id = reqId;
        requestQueue_.enqueue(req);

        models::AuditLog log;
        log.userId = requestedByUserId;
        log.username = "Actor #" + std::to_string(requestedByUserId);
        log.action = "RESTOCK_REQUESTED";
        log.entityType = "RESTOCK_REQUEST";
        log.entityId = req.requestRef;
        log.changeSummary = "Submitted restock request for " + std::to_string(requestedQuantity) + " units of " + med->name;
        auditRepo_.logEvent(log);
        return reqId;
    }

    outError = "Database error creating restock request.";
    return -1;
}

bool RestockService::reviewRequest(int requestId, models::RestockStatus newStatus, int reviewerUserId, const std::string& reviewNotes, std::string& outError) {
    auto reqOpt = restockRepo_.findById(requestId);
    if (!reqOpt) {
        outError = "Restock request not found.";
        return false;
    }

    if (reqOpt->status != models::RestockStatus::PENDING) {
        outError = "Only pending restock requests can be reviewed.";
        return false;
    }

    bool ok = restockRepo_.updateStatus(requestId, newStatus, reviewerUserId, reviewNotes);
    if (ok) {
        refreshQueue(); // Re-sync in-memory FIFO queue

        models::AuditLog log;
        log.userId = reviewerUserId;
        log.username = "Actor #" + std::to_string(reviewerUserId);
        log.action = "RESTOCK_REVIEWED";
        log.entityType = "RESTOCK_REQUEST";
        log.entityId = reqOpt->requestRef;
        log.changeSummary = "Status changed to " + models::restockStatusToString(newStatus) + ". Notes: " + reviewNotes;
        auditRepo_.logEvent(log);
        return true;
    }

    outError = "Database error updating restock request.";
    return false;
}

std::optional<models::RestockRequest> RestockService::popNextPendingFromQueue() {
    if (requestQueue_.empty()) {
        return std::nullopt;
    }
    return requestQueue_.dequeue();
}

std::vector<models::RestockRequest> RestockService::listAllRequests() {
    return restockRepo_.listAll();
}

std::vector<models::RestockRequest> RestockService::listPendingRequests() {
    return restockRepo_.listPending();
}

size_t RestockService::getPendingCount() {
    return restockRepo_.countPending();
}

} // namespace medifind::services
