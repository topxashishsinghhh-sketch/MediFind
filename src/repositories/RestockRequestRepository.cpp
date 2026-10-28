#include "repositories/RestockRequestRepository.hpp"
#include <iomanip>
#include <sstream>

namespace medifind::repositories {

int RestockRequestRepository::createRequest(const models::RestockRequest& req) {
    auto stmt = db_.prepare(R"(
        INSERT INTO restock_requests (request_ref, medicine_id, requested_quantity, reason, status, requested_by_user_id, created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?, datetime('now'), datetime('now'));
    )");
    stmt.bindText(1, req.requestRef);
    stmt.bindInt(2, req.medicineId);
    stmt.bindInt(3, req.requestedQuantity);
    stmt.bindText(4, req.reason);
    stmt.bindText(5, models::restockStatusToString(req.status));
    stmt.bindInt(6, req.requestedByUserId);
    stmt.step();
    return static_cast<int>(db_.lastInsertRowId());
}

std::optional<models::RestockRequest> RestockRequestRepository::findById(int id) {
    auto stmt = db_.prepare("SELECT id, request_ref, medicine_id, requested_quantity, reason, status, requested_by_user_id, approved_by_user_id, review_notes, created_at, updated_at FROM restock_requests WHERE id = ?;");
    stmt.bindInt(1, id);
    if (stmt.step()) {
        models::RestockRequest r;
        r.id = stmt.getInt(0);
        r.requestRef = stmt.getText(1);
        r.medicineId = stmt.getInt(2);
        r.requestedQuantity = stmt.getInt(3);
        r.reason = stmt.getText(4);
        r.status = models::stringToRestockStatus(stmt.getText(5));
        r.requestedByUserId = stmt.getInt(6);
        r.approvedByUserId = stmt.isNull(7) ? 0 : stmt.getInt(7);
        r.reviewNotes = stmt.isNull(8) ? "" : stmt.getText(8);
        r.createdAt = stmt.getText(9);
        r.updatedAt = stmt.getText(10);
        return r;
    }
    return std::nullopt;
}

std::vector<models::RestockRequest> RestockRequestRepository::listAll() {
    auto stmt = db_.prepare("SELECT id, request_ref, medicine_id, requested_quantity, reason, status, requested_by_user_id, approved_by_user_id, review_notes, created_at, updated_at FROM restock_requests ORDER BY id DESC;");
    std::vector<models::RestockRequest> list;
    while (stmt.step()) {
        models::RestockRequest r;
        r.id = stmt.getInt(0);
        r.requestRef = stmt.getText(1);
        r.medicineId = stmt.getInt(2);
        r.requestedQuantity = stmt.getInt(3);
        r.reason = stmt.getText(4);
        r.status = models::stringToRestockStatus(stmt.getText(5));
        r.requestedByUserId = stmt.getInt(6);
        r.approvedByUserId = stmt.isNull(7) ? 0 : stmt.getInt(7);
        r.reviewNotes = stmt.isNull(8) ? "" : stmt.getText(8);
        r.createdAt = stmt.getText(9);
        r.updatedAt = stmt.getText(10);
        list.push_back(r);
    }
    return list;
}

std::vector<models::RestockRequest> RestockRequestRepository::listPending() {
    auto stmt = db_.prepare("SELECT id, request_ref, medicine_id, requested_quantity, reason, status, requested_by_user_id, approved_by_user_id, review_notes, created_at, updated_at FROM restock_requests WHERE status = 'PENDING' ORDER BY id ASC;");
    std::vector<models::RestockRequest> list;
    while (stmt.step()) {
        models::RestockRequest r;
        r.id = stmt.getInt(0);
        r.requestRef = stmt.getText(1);
        r.medicineId = stmt.getInt(2);
        r.requestedQuantity = stmt.getInt(3);
        r.reason = stmt.getText(4);
        r.status = models::stringToRestockStatus(stmt.getText(5));
        r.requestedByUserId = stmt.getInt(6);
        r.approvedByUserId = stmt.isNull(7) ? 0 : stmt.getInt(7);
        r.reviewNotes = stmt.isNull(8) ? "" : stmt.getText(8);
        r.createdAt = stmt.getText(9);
        r.updatedAt = stmt.getText(10);
        list.push_back(r);
    }
    return list;
}

bool RestockRequestRepository::updateStatus(int requestId, models::RestockStatus newStatus, int reviewerUserId, const std::string& reviewNotes) {
    auto stmt = db_.prepare(R"(
        UPDATE restock_requests
        SET status = ?, approved_by_user_id = ?, review_notes = ?, updated_at = datetime('now')
        WHERE id = ?;
    )");
    stmt.bindText(1, models::restockStatusToString(newStatus));
    stmt.bindInt(2, reviewerUserId);
    stmt.bindText(3, reviewNotes);
    stmt.bindInt(4, requestId);
    stmt.step();
    return db_.changes() > 0;
}

std::string RestockRequestRepository::generateNextRequestRef() {
    auto stmt = db_.prepare("SELECT COALESCE(MAX(id), 0) + 1 FROM restock_requests;");
    int nextId = 1;
    if (stmt.step()) {
        nextId = stmt.getInt(0);
    }
    std::ostringstream oss;
    oss << "REQ-2026-" << std::setw(4) << std::setfill('0') << nextId;
    return oss.str();
}

size_t RestockRequestRepository::countPending() {
    auto stmt = db_.prepare("SELECT COUNT(*) FROM restock_requests WHERE status = 'PENDING';");
    if (stmt.step()) {
        return static_cast<size_t>(stmt.getInt(0));
    }
    return 0;
}

} // namespace medifind::repositories
