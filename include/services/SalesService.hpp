#pragma once

#include "database/DatabaseConnection.hpp"
#include "repositories/SaleRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/ReturnRepository.hpp"
#include "repositories/StockMovementRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "services/FefoAllocator.hpp"
#include "models/Sale.hpp"
#include "models/ReturnRecord.hpp"
#include <string>
#include <vector>
#include <optional>

namespace medifind::services {

struct SaleCartItem {
    int medicineId{0};
    int requestedQuantity{0};
};

struct SaleTransactionResult {
    bool success{false};
    std::string message;
    std::string invoiceNumber;
    int saleId{0};
    double totalAmount{0.0};
    std::vector<BatchAllocationItem> allocatedItems;
};

struct ReturnResult {
    bool success{false};
    std::string message;
    std::string returnRef;
    double refundAmount{0.0};
};

class SalesService {
private:
    database::DatabaseConnection& db_;
    repositories::SaleRepository& saleRepo_;
    repositories::BatchRepository& batchRepo_;
    repositories::MedicineRepository& medRepo_;
    repositories::ReturnRepository& returnRepo_;
    repositories::StockMovementRepository& movementRepo_;
    repositories::AuditLogRepository& auditRepo_;
    FefoAllocator fefoAllocator_;

public:
    SalesService(database::DatabaseConnection& db,
                 repositories::SaleRepository& saleRepo,
                 repositories::BatchRepository& batchRepo,
                 repositories::MedicineRepository& medRepo,
                 repositories::ReturnRepository& returnRepo,
                 repositories::StockMovementRepository& movementRepo,
                 repositories::AuditLogRepository& auditRepo);

    /**
     * @brief Previews FEFO allocation plan for a multi-item cart without modifying database.
     */
    std::vector<FefoAllocationPlan> previewCartAllocation(const std::vector<SaleCartItem>& cart, const std::string& currentDate);

    /**
     * @brief Processes a complete atomic sales transaction using FEFO stock allocation.
     */
    SaleTransactionResult processSale(const std::vector<SaleCartItem>& cart,
                                      int soldByUserId,
                                      const std::string& paymentMethod,
                                      const std::string& customerName,
                                      const std::string& notes,
                                      const std::string& currentDate);

    /**
     * @brief Processes customer return of a previously sold item.
     * Prevents returning more than sold quantity.
     * Respects returnToSellableStock policy.
     */
    ReturnResult processReturn(int saleId,
                               int saleItemId,
                               int returnQuantity,
                               const std::string& reason,
                               bool returnToSellableStock,
                               int processedByUserId);

    std::optional<models::Sale> getSaleById(int saleId);
    std::optional<models::Sale> getSaleByInvoice(const std::string& invoiceNumber);
    std::vector<models::Sale> listSales(const std::string& startDate = "", const std::string& endDate = "", int limit = 50, int offset = 0);
    double getTotalSalesRevenue(const std::string& startDate = "", const std::string& endDate = "");
};

} // namespace medifind::services
