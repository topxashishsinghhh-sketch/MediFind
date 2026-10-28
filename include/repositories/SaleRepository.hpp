#pragma once

#include "database/DatabaseConnection.hpp"
#include "models/Sale.hpp"
#include <optional>
#include <vector>

namespace medifind::repositories {

class SaleRepository {
private:
    database::DatabaseConnection& db_;

public:
    explicit SaleRepository(database::DatabaseConnection& db) : db_(db) {}

    int createSaleWithItems(const models::Sale& sale);
    std::optional<models::Sale> findById(int id);
    std::optional<models::Sale> findByInvoice(const std::string& invoiceNumber);
    std::vector<models::Sale> listSales(const std::string& startDate = "", const std::string& endDate = "", int limit = 100, int offset = 0);
    std::vector<models::SaleItem> getSaleItems(int saleId);
    std::string generateNextInvoiceNumber();
    double getTotalSalesRevenue(const std::string& startDate = "", const std::string& endDate = "");
    size_t countSales();
};

} // namespace medifind::repositories
