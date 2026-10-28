#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/PurchaseOrder.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class PurchaseOrderRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit PurchaseOrderRepository(database::DatabaseConnection& db) : db_(db) {}

    int createPurchaseOrder(const models::PurchaseOrder& po);
    std::optional<models::PurchaseOrder> findById(int id);
    std::optional<models::PurchaseOrder> findByRef(const std::string& orderRef);
    std::vector<models::PurchaseOrder> listAll();
    bool updateStatus(int poId, models::OrderStatus newStatus);
    bool updateItemReceivedQuantity(int itemId, int additionalReceived);
    std::vector<models::PurchaseOrderItem> getOrderItems(int poId);
    std::string generateNextOrderRef();
};

} // namespace medifind::repositories
