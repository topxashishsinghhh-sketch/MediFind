#pragma once

#include "services/InventoryService.hpp"
#include "services/SalesService.hpp"
#include "services/PurchasingService.hpp"
#include "repositories/AuditLogRepository.hpp"
#include <string>

namespace medifind::services {

class ReportService {
private:
    InventoryService& invService_;
    SalesService& salesService_;
    PurchasingService& purchService_;
    repositories::AuditLogRepository& auditRepo_;
    repositories::MedicineRepository& medRepo_;
    repositories::BatchRepository& batchRepo_;

public:
    ReportService(InventoryService& invService,
                  SalesService& salesService,
                  PurchasingService& purchService,
                  repositories::AuditLogRepository& auditRepo,
                  repositories::MedicineRepository& medRepo,
                  repositories::BatchRepository& batchRepo);

    // CSV Exports
    bool exportInventoryCsv(const std::string& filepath);
    bool exportBatchesCsv(const std::string& filepath);
    bool exportSalesCsv(const std::string& filepath, const std::string& startDate = "", const std::string& endDate = "");
    bool exportAuditLogsCsv(const std::string& filepath);

    // Printable Formatted Text Reports
    std::string generateInventoryValuationReport();
    std::string generateExpiryAlertReport(const std::string& currentDate, int warningDays = 90);
    std::string generateLowStockReport();
    std::string generateSalesSummaryReport(const std::string& startDate = "", const std::string& endDate = "");

    // Export printable report to file
    bool saveTextReport(const std::string& filepath, const std::string& reportContent);
};

} // namespace medifind::services
