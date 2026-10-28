#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include "database/DatabaseConnection.hpp"
#include "repositories/UserRepository.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "repositories/SupplierRepository.hpp"
#include "repositories/PurchaseOrderRepository.hpp"
#include "repositories/SaleRepository.hpp"
#include "repositories/ReturnRepository.hpp"
#include "repositories/StockMovementRepository.hpp"
#include "repositories/RestockRequestRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "services/AuthService.hpp"
#include "services/InventoryService.hpp"
#include "services/PurchasingService.hpp"
#include "services/SalesService.hpp"
#include "services/RestockService.hpp"
#include "services/ReportService.hpp"
#include "services/AuditService.hpp"

namespace medifind::server {

class WebServer {
public:
    WebServer(int port, const std::string& webRoot,
              database::DatabaseConnection& db,
              repositories::UserRepository& userRepo,
              repositories::MedicineRepository& medRepo,
              repositories::BatchRepository& batchRepo,
              repositories::SupplierRepository& supplierRepo,
              repositories::PurchaseOrderRepository& poRepo,
              repositories::SaleRepository& saleRepo,
              repositories::ReturnRepository& returnRepo,
              repositories::StockMovementRepository& movementRepo,
              repositories::RestockRequestRepository& restockRepo,
              repositories::AuditLogRepository& auditRepo,
              services::AuthService& authService,
              services::InventoryService& invService,
              services::PurchasingService& purchService,
              services::SalesService& salesService,
              services::RestockService& restockService,
              services::ReportService& reportService,
              services::AuditService& auditService);
    ~WebServer();

    void start();
    void stop();

private:
    int port_;
    std::string webRoot_;
    std::atomic<bool> running_{false};
    uintptr_t serverSocket_{0};

    database::DatabaseConnection& db_;
    repositories::UserRepository& userRepo_;
    repositories::MedicineRepository& medRepo_;
    repositories::BatchRepository& batchRepo_;
    repositories::SupplierRepository& supplierRepo_;
    repositories::PurchaseOrderRepository& poRepo_;
    repositories::SaleRepository& saleRepo_;
    repositories::ReturnRepository& returnRepo_;
    repositories::StockMovementRepository& movementRepo_;
    repositories::RestockRequestRepository& restockRepo_;
    repositories::AuditLogRepository& auditRepo_;

    services::AuthService& authService_;
    services::InventoryService& invService_;
    services::PurchasingService& purchService_;
    services::SalesService& salesService_;
    services::RestockService& restockService_;
    services::ReportService& reportService_;
    services::AuditService& auditService_;

    void handleClient(uintptr_t clientSocket);
    std::string handleApiRequest(const std::string& method, const std::string& path, const std::string& body);
    std::string getMimeType(const std::string& path);
    void openBrowser(const std::string& url);
};

} // namespace medifind::server
