#include "services/ReportService.hpp"
#include "utils/CsvExporter.hpp"
#include "utils/DateUtils.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace medifind::services {

ReportService::ReportService(InventoryService& invService,
                             SalesService& salesService,
                             PurchasingService& purchService,
                             repositories::AuditLogRepository& auditRepo,
                             repositories::MedicineRepository& medRepo,
                             repositories::BatchRepository& batchRepo)
    : invService_(invService),
      salesService_(salesService),
      purchService_(purchService),
      auditRepo_(auditRepo),
      medRepo_(medRepo),
      batchRepo_(batchRepo) {}

bool ReportService::exportInventoryCsv(const std::string& filepath) {
    std::vector<std::string> headers = {
        "ID", "Name", "Brand", "Strength", "Dosage Form", "Category", "Barcode", "Current Stock", "Min Threshold", "Status"
    };

    std::vector<std::vector<std::string>> rows;
    auto meds = medRepo_.listAll(true);
    for (const auto& m : meds) {
        int stock = medRepo_.getTotalStockQuantity(m.id);
        std::vector<std::string> row = {
            std::to_string(m.id),
            m.name,
            m.brand,
            m.strength,
            m.dosageForm,
            m.category,
            m.barcode,
            std::to_string(stock),
            std::to_string(m.minStockThreshold),
            m.isActive ? "ACTIVE" : "INACTIVE"
        };
        rows.push_back(row);
    }

    return utils::CsvExporter::exportToCsv(filepath, headers, rows);
}

bool ReportService::exportBatchesCsv(const std::string& filepath) {
    std::vector<std::string> headers = {
        "Batch ID", "Medicine ID", "Medicine Name", "Batch Number", "Mfg Date", "Expiry Date", "Quantity", "Purchase Cost", "Selling Price", "Location", "Status"
    };

    std::vector<std::vector<std::string>> rows;
    auto batches = batchRepo_.listAll();
    for (const auto& b : batches) {
        auto med = medRepo_.findById(b.medicineId);
        std::string medName = med ? med->name : "Unknown";

        std::ostringstream pCost, sPrice;
        pCost << std::fixed << std::setprecision(2) << b.purchaseCost;
        sPrice << std::fixed << std::setprecision(2) << b.sellingPrice;

        std::vector<std::string> row = {
            std::to_string(b.id),
            std::to_string(b.medicineId),
            medName,
            b.batchNumber,
            b.mfgDate,
            b.expiryDate,
            std::to_string(b.quantity),
            pCost.str(),
            sPrice.str(),
            b.storageLocation,
            models::batchStatusToString(b.status)
        };
        rows.push_back(row);
    }

    return utils::CsvExporter::exportToCsv(filepath, headers, rows);
}

bool ReportService::exportSalesCsv(const std::string& filepath, const std::string& startDate, const std::string& endDate) {
    std::vector<std::string> headers = {
        "Invoice", "Sale Date", "Sold By User", "Customer", "Payment Method", "Total Amount", "Items Count"
    };

    std::vector<std::vector<std::string>> rows;
    auto sales = salesService_.listSales(startDate, endDate, 10000, 0);
    for (const auto& s : sales) {
        std::ostringstream totalOss;
        totalOss << std::fixed << std::setprecision(2) << s.totalAmount;

        std::vector<std::string> row = {
            s.invoiceNumber,
            s.saleDate,
            std::to_string(s.soldByUserId),
            s.customerName.empty() ? "Walk-in" : s.customerName,
            s.paymentMethod,
            totalOss.str(),
            std::to_string(s.items.size())
        };
        rows.push_back(row);
    }

    return utils::CsvExporter::exportToCsv(filepath, headers, rows);
}

bool ReportService::exportAuditLogsCsv(const std::string& filepath) {
    std::vector<std::string> headers = {
        "Log ID", "Timestamp", "User ID", "Username", "Action", "Entity Type", "Entity ID", "Details", "Context"
    };

    std::vector<std::vector<std::string>> rows;
    auto logs = auditRepo_.listLogs(10000, 0);
    for (const auto& l : logs) {
        std::vector<std::string> row = {
            std::to_string(l.id),
            l.timestamp,
            std::to_string(l.userId),
            l.username,
            l.action,
            l.entityType,
            l.entityId,
            l.changeSummary,
            l.clientContext
        };
        rows.push_back(row);
    }

    return utils::CsvExporter::exportToCsv(filepath, headers, rows);
}

std::string ReportService::generateInventoryValuationReport() {
    std::ostringstream oss;
    oss << "========================================================================================\n";
    oss << "               MEDIFIND - INVENTORY VALUATION & AUDIT REPORT                            \n";
    oss << "               Generated: " << utils::DateUtils::getCurrentDateTime() << "               \n";
    oss << "========================================================================================\n\n";

    double totalCost = invService_.getTotalInventoryCostValue();
    double totalRetail = invService_.getTotalInventoryRetailValue();
    int totalUnits = invService_.getTotalUnitsInStock();
    double projectedMargin = totalRetail - totalCost;
    double marginPercent = totalCost > 0.0 ? ((projectedMargin / totalCost) * 100.0) : 0.0;

    oss << "INVENTORY SUMMARY:\n";
    oss << "  - Total Available Stock Units : " << totalUnits << "\n";
    oss << "  - Total Asset Value (Cost)    : $" << std::fixed << std::setprecision(2) << totalCost << "\n";
    oss << "  - Projected Retail Value      : $" << std::fixed << std::setprecision(2) << totalRetail << "\n";
    oss << "  - Projected Gross Margin      : $" << std::fixed << std::setprecision(2) << projectedMargin
        << " (" << std::fixed << std::setprecision(1) << marginPercent << "%)\n\n";

    oss << "----------------------------------------------------------------------------------------\n";
    oss << std::left << std::setw(6)  << "ID"
        << std::left << std::setw(25) << "Medicine"
        << std::left << std::setw(14) << "Batch #"
        << std::right << std::setw(8) << "Stock"
        << std::right << std::setw(12) << "Cost ($)"
        << std::right << std::setw(12) << "Total Cost"
        << std::right << std::setw(12) << "Retail ($)"
        << "\n";
    oss << "----------------------------------------------------------------------------------------\n";

    auto batches = batchRepo_.listAll();
    for (const auto& b : batches) {
        if (b.quantity <= 0 || b.status == models::BatchStatus::EXPIRED) continue;
        auto med = medRepo_.findById(b.medicineId);
        std::string medName = med ? med->name : "Unknown";
        if (medName.length() > 23) medName = medName.substr(0, 20) + "...";

        double batchTotalCost = b.quantity * b.purchaseCost;

        oss << std::left  << std::setw(6)  << b.id
            << std::left  << std::setw(25) << medName
            << std::left  << std::setw(14) << b.batchNumber
            << std::right << std::setw(8)  << b.quantity
            << std::right << std::setw(12) << std::fixed << std::setprecision(2) << b.purchaseCost
            << std::right << std::setw(12) << std::fixed << std::setprecision(2) << batchTotalCost
            << std::right << std::setw(12) << std::fixed << std::setprecision(2) << b.sellingPrice
            << "\n";
    }

    oss << "========================================================================================\n";
    oss << "END OF VALUATION REPORT\n";
    return oss.str();
}

std::string ReportService::generateExpiryAlertReport(const std::string& currentDate, int warningDays) {
    std::ostringstream oss;
    oss << "========================================================================================\n";
    oss << "               MEDIFIND - EXPIRY & SAFETY COMPLIANCE REPORT                             \n";
    oss << "               Reference Date: " << currentDate << " (Warning Horizon: " << warningDays << " days)\n";
    oss << "========================================================================================\n\n";

    auto expired = invService_.getExpiredBatches(currentDate);
    auto nearExpiry = invService_.getExpiringBatches(currentDate, warningDays);

    oss << "[!] EXPIRED BATCHES (QUARANTINED FROM FEFO SALES): " << expired.size() << "\n";
    oss << "----------------------------------------------------------------------------------------\n";
    oss << std::left << std::setw(6)  << "ID"
        << std::left << std::setw(25) << "Medicine"
        << std::left << std::setw(14) << "Batch #"
        << std::left << std::setw(12) << "Expiry"
        << std::right << std::setw(8) << "Units"
        << std::right << std::setw(14) << "Loss Val ($)"
        << "\n";
    oss << "----------------------------------------------------------------------------------------\n";

    double expiredLoss = 0.0;
    for (const auto& b : expired) {
        auto med = medRepo_.findById(b.medicineId);
        std::string medName = med ? med->name : "Unknown";
        double loss = b.quantity * b.purchaseCost;
        expiredLoss += loss;

        oss << std::left  << std::setw(6)  << b.id
            << std::left  << std::setw(25) << medName
            << std::left  << std::setw(14) << b.batchNumber
            << std::left  << std::setw(12) << b.expiryDate
            << std::right << std::setw(8)  << b.quantity
            << std::right << std::setw(14) << std::fixed << std::setprecision(2) << loss
            << "\n";
    }
    oss << "Total Expired Value: $" << std::fixed << std::setprecision(2) << expiredLoss << "\n\n";

    oss << "[*] NEAR-EXPIRY BATCHES (PRIORITIZED FOR FEFO DISPATCH): " << nearExpiry.size() << "\n";
    oss << "----------------------------------------------------------------------------------------\n";
    oss << std::left << std::setw(6)  << "ID"
        << std::left << std::setw(25) << "Medicine"
        << std::left << std::setw(14) << "Batch #"
        << std::left << std::setw(12) << "Expiry"
        << std::right << std::setw(10) << "Days Left"
        << std::right << std::setw(8) << "Units"
        << "\n";
    oss << "----------------------------------------------------------------------------------------\n";

    for (const auto& b : nearExpiry) {
        auto med = medRepo_.findById(b.medicineId);
        std::string medName = med ? med->name : "Unknown";
        int days = utils::DateUtils::daysDifference(currentDate, b.expiryDate);

        oss << std::left  << std::setw(6)  << b.id
            << std::left  << std::setw(25) << medName
            << std::left  << std::setw(14) << b.batchNumber
            << std::left  << std::setw(12) << b.expiryDate
            << std::right << std::setw(10) << days
            << std::right << std::setw(8)  << b.quantity
            << "\n";
    }

    oss << "========================================================================================\n";
    return oss.str();
}

std::string ReportService::generateLowStockReport() {
    std::ostringstream oss;
    oss << "========================================================================================\n";
    oss << "               MEDIFIND - LOW STOCK & REORDER ALERT REPORT                              \n";
    oss << "               Generated: " << utils::DateUtils::getCurrentDateTime() << "               \n";
    oss << "========================================================================================\n\n";

    auto lowStock = invService_.getLowStockMedicines();
    oss << "Total Medicines Requiring Restock: " << lowStock.size() << "\n\n";

    oss << std::left << std::setw(6)  << "ID"
        << std::left << std::setw(28) << "Medicine Name"
        << std::left << std::setw(16) << "Category"
        << std::right << std::setw(10) << "Current"
        << std::right << std::setw(10) << "Min Thresh"
        << std::right << std::setw(12) << "Deficit"
        << "\n";
    oss << "----------------------------------------------------------------------------------------\n";

    for (const auto& m : lowStock) {
        int current = medRepo_.getTotalStockQuantity(m.id);
        int deficit = m.minStockThreshold - current;

        oss << std::left  << std::setw(6)  << m.id
            << std::left  << std::setw(28) << m.name
            << std::left  << std::setw(16) << m.category
            << std::right << std::setw(10) << current
            << std::right << std::setw(10) << m.minStockThreshold
            << std::right << std::setw(12) << (deficit > 0 ? deficit : 0)
            << "\n";
    }

    oss << "========================================================================================\n";
    return oss.str();
}

std::string ReportService::generateSalesSummaryReport(const std::string& startDate, const std::string& endDate) {
    std::ostringstream oss;
    oss << "========================================================================================\n";
    oss << "               MEDIFIND - SALES PERFORMANCE & AUDIT REPORT                              \n";
    oss << "               Period: " << (startDate.empty() ? "Beginning" : startDate) << " to "
        << (endDate.empty() ? "Present" : endDate) << "\n";
    oss << "========================================================================================\n\n";

    auto sales = salesService_.listSales(startDate, endDate, 1000, 0);
    double totalRev = salesService_.getTotalSalesRevenue(startDate, endDate);

    oss << "Total Transactions Completed : " << sales.size() << "\n";
    oss << "Total Revenue Generated      : $" << std::fixed << std::setprecision(2) << totalRev << "\n\n";

    oss << std::left << std::setw(16) << "Invoice #"
        << std::left << std::setw(20) << "Date & Time"
        << std::left << std::setw(18) << "Customer"
        << std::left << std::setw(10) << "Payment"
        << std::right << std::setw(12) << "Amount ($)"
        << "\n";
    oss << "----------------------------------------------------------------------------------------\n";

    for (const auto& s : sales) {
        oss << std::left  << std::setw(16) << s.invoiceNumber
            << std::left  << std::setw(20) << s.saleDate
            << std::left  << std::setw(18) << (s.customerName.empty() ? "Walk-in" : s.customerName)
            << std::left  << std::setw(10) << s.paymentMethod
            << std::right << std::setw(12) << std::fixed << std::setprecision(2) << s.totalAmount
            << "\n";
    }

    oss << "========================================================================================\n";
    return oss.str();
}

bool ReportService::saveTextReport(const std::string& filepath, const std::string& reportContent) {
    std::ofstream out(filepath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) return false;
    out << reportContent;
    out.close();
    return true;
}

} // namespace medifind::services
