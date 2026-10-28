#include "server/WebServer.hpp"
#include "database/DatabaseConnection.hpp"
#include "database/SchemaMigrations.hpp"
#include "seed/SeedData.hpp"
#include "utils/DateUtils.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    std::string dbFile = "medifind.db";
    std::cout << "[+] Connecting to SQLite authoritative database: " << dbFile << "\n";

    medifind::database::DatabaseConnection db(dbFile);
    medifind::database::SchemaMigrations::applyMigrations(db);

    medifind::repositories::UserRepository userRepo(db);
    medifind::repositories::MedicineRepository medRepo(db);
    medifind::repositories::BatchRepository batchRepo(db);
    medifind::repositories::SupplierRepository supplierRepo(db);
    medifind::repositories::PurchaseOrderRepository poRepo(db);
    medifind::repositories::SaleRepository saleRepo(db);
    medifind::repositories::ReturnRepository returnRepo(db);
    medifind::repositories::StockMovementRepository movementRepo(db);
    medifind::repositories::RestockRequestRepository restockRepo(db);
    medifind::repositories::AuditLogRepository auditRepo(db);

    medifind::services::AuthService authService(userRepo, auditRepo);
    medifind::services::InventoryService invService(db, medRepo, batchRepo, movementRepo, auditRepo);
    medifind::services::PurchasingService purchService(db, supplierRepo, poRepo, medRepo, batchRepo, movementRepo, auditRepo);
    medifind::services::SalesService salesService(db, saleRepo, batchRepo, medRepo, returnRepo, movementRepo, auditRepo);
    medifind::services::RestockService restockService(db, restockRepo, medRepo, auditRepo);
    medifind::services::ReportService reportService(invService, salesService, purchService, auditRepo, medRepo, batchRepo);
    medifind::services::AuditService auditService(auditRepo);

    // Seed demo data if database is brand new
    medifind::seed::SeedData::seedDemoData(db, authService, invService, purchService, salesService);
    invService.refreshExpiryStatuses(medifind::utils::DateUtils::getCurrentDate());

    // Determine web folder location
    std::string webDir = "web";
    if (!std::filesystem::exists(webDir)) {
        if (std::filesystem::exists("../web")) {
            webDir = "../web";
        }
    }

    medifind::server::WebServer server(port, webDir,
                                       db, userRepo, medRepo, batchRepo, supplierRepo,
                                       poRepo, saleRepo, returnRepo, movementRepo,
                                       restockRepo, auditRepo, authService, invService,
                                       purchService, salesService, restockService,
                                       reportService, auditService);

    server.start();
    return 0;
}
