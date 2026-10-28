#pragma once

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
#include "dsa/Stack.hpp"
#include <string>

namespace medifind::console {

class ConsoleApp {
private:
    database::DatabaseConnection db_;
    repositories::UserRepository userRepo_;
    repositories::MedicineRepository medRepo_;
    repositories::BatchRepository batchRepo_;
    repositories::SupplierRepository supplierRepo_;
    repositories::PurchaseOrderRepository poRepo_;
    repositories::SaleRepository saleRepo_;
    repositories::ReturnRepository returnRepo_;
    repositories::StockMovementRepository movementRepo_;
    repositories::RestockRequestRepository restockRepo_;
    repositories::AuditLogRepository auditRepo_;

    services::AuthService authService_;
    services::InventoryService invService_;
    services::PurchasingService purchService_;
    services::SalesService salesService_;
    services::RestockService restockService_;
    services::ReportService reportService_;
    services::AuditService auditService_;

    // DSA Stack for navigation breadcrumbs / history
    dsa::Stack<std::string> navigationHistory_;

    bool isRunning_{true};

public:
    explicit ConsoleApp(const std::string& dbPath);

    void run();

private:
    void printHeader(const std::string& title);
    void printDivider();
    std::string promptString(const std::string& prompt);
    int promptInt(const std::string& prompt);
    double promptDouble(const std::string& prompt);
    void pause();

    // Authentication screens
    void handleLogin();
    void handleBootstrap();

    // Main Menus
    void runAdminMenu();
    void runEmployeeMenu();

    // Admin & Employee Features
    void viewDashboard();
    void manageMedicines();
    void addMedicineWorkflow();
    void searchMedicinesWorkflow();
    void bstPrefixSearchWorkflow();

    void manageBatches();
    void stockInBatchWorkflow();
    void adjustStockWorkflow();
    void expiryAlertsWorkflow();

    void pointOfSaleWorkflow();
    void processReturnWorkflow();

    void manageSuppliersAndOrders();
    void createPurchaseOrderWorkflow();
    void receiveShipmentWorkflow();

    void manageRestockRequests();
    void submitRestockWorkflow();
    void processRestockQueueFifoWorkflow();

    void stockReconciliationWorkflow();
    void reportsAndExportsWorkflow();
    void backupAndRestoreWorkflow();
    void manageUsersWorkflow();
    void dsaDemonstrationWorkflow();
    void changePasswordWorkflow();
};

} // namespace medifind::console
