#pragma once

#include "database/DatabaseConnection.hpp"
#include "repositories/SupplierRepository.hpp"
#include "repositories/PurchaseOrderRepository.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "repositories/StockMovementRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "models/Supplier.hpp"
#include "models/PurchaseOrder.hpp"
#include <vector>
#include <string>
#include <optional>

namespace medifind::services {

struct ReorderRecommendation {
    int medicineId{0};
    std::string medicineName;
    std::string category;
    int currentStock{0};
    int minThreshold{0};
    int recommendedReorderQuantity{0};
    double estimatedCost{0.0};
    int suggestedSupplierId{0};
    std::string suggestedSupplierName;
};

struct ReceiveShipmentItem {
    int orderItemId{0};
    int medicineId{0};
    int quantityToReceive{0};
    std::string batchNumber;
    std::string mfgDate;
    std::string expiryDate;
    double sellingPrice{0.0};
    std::string storageLocation;
};

class PurchasingService {
private:
    database::DatabaseConnection& db_;
    repositories::SupplierRepository& supplierRepo_;
    repositories::PurchaseOrderRepository& poRepo_;
    repositories::MedicineRepository& medRepo_;
    repositories::BatchRepository& batchRepo_;
    repositories::StockMovementRepository& movementRepo_;
    repositories::AuditLogRepository& auditRepo_;

public:
    PurchasingService(database::DatabaseConnection& db,
                      repositories::SupplierRepository& supplierRepo,
                      repositories::PurchaseOrderRepository& poRepo,
                      repositories::MedicineRepository& medRepo,
                      repositories::BatchRepository& batchRepo,
                      repositories::StockMovementRepository& movementRepo,
                      repositories::AuditLogRepository& auditRepo);

    // Supplier Management
    int addSupplier(const models::Supplier& supplier, int actorUserId, std::string& outError);
    bool updateSupplier(const models::Supplier& supplier, int actorUserId, std::string& outError);
    std::vector<models::Supplier> listSuppliers();

    // Purchase Order Management
    int createPurchaseOrder(int supplierId,
                            const std::string& expectedDeliveryDate,
                            const std::string& notes,
                            const std::vector<std::pair<int, std::pair<int, double>>>& items, // medicineId -> (qty, cost)
                            int actorUserId,
                            std::string& outError);

    bool updateOrderStatus(int poId, models::OrderStatus newStatus, int actorUserId, std::string& outError);
    std::vector<models::PurchaseOrder> listOrders();
    std::optional<models::PurchaseOrder> getOrderById(int poId);

    // Receiving Stock against PO
    bool receiveShipment(int poId,
                         const std::vector<ReceiveShipmentItem>& receivedItems,
                         int actorUserId,
                         std::string& outError);

    // Rule-Based Reorder Recommendations
    std::vector<ReorderRecommendation> generateReorderRecommendations();
};

} // namespace medifind::services
