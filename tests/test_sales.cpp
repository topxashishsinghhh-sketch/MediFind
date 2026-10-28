#include "TestFramework.hpp"
#include "database/DatabaseConnection.hpp"
#include "database/SchemaMigrations.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "repositories/SaleRepository.hpp"
#include "repositories/ReturnRepository.hpp"
#include "repositories/StockMovementRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "services/SalesService.hpp"
#include <cstdio>

using namespace medifind;

TEST(SalesSuite, CompleteSaleAndReturnWorkflow) {
    const std::string dbFile = "test_sales.db";
    std::remove(dbFile.c_str());

    {
        database::DatabaseConnection db(dbFile);
        database::SchemaMigrations::applyMigrations(db);

        // Seed a test user (id=1) — required by FK on sales.sold_by_user_id
        // and returns.processed_by_user_id.
        db.execute(R"(
            INSERT INTO users (id, username, password_hash, salt, full_name, role, is_active)
            VALUES (1, 'teststaff', 'hash', 'salt', 'Test Staff', 'EMPLOYEE', 1);
        )");

        repositories::MedicineRepository medRepo(db);
        repositories::BatchRepository batchRepo(db);
        repositories::SaleRepository saleRepo(db);
        repositories::ReturnRepository retRepo(db);
        repositories::StockMovementRepository movRepo(db);
        repositories::AuditLogRepository auditRepo(db);

        services::SalesService salesService(db, saleRepo, batchRepo, medRepo, retRepo, movRepo, auditRepo);

        // Setup Medicine & Batch
        models::Medicine med;
        med.name = "Ibuprofen 400mg";
        int medId = medRepo.createMedicine(med);

        models::Batch b;
        b.medicineId = medId;
        b.batchNumber = "IBU-001";
        b.mfgDate = "2025-01-01";
        b.expiryDate = "2027-01-01";
        b.quantity = 30;
        b.sellingPrice = 5.00;
        b.purchaseCost = 2.00;
        b.status = models::BatchStatus::AVAILABLE;
        int batchId = batchRepo.createBatch(b);

        // 1. Process Sale of 10 units
        std::vector<services::SaleCartItem> cart = {{medId, 10}};
        auto saleRes = salesService.processSale(cart, 1, "CASH", "Patient A", "Normal sale", "2026-10-27");
        ASSERT_TRUE(saleRes.success);
        ASSERT_NEAR(saleRes.totalAmount, 50.00, 0.01);

        // Verify remaining batch quantity
        auto updatedBatch = batchRepo.findById(batchId);
        ASSERT_TRUE(updatedBatch.has_value());
        ASSERT_EQ(updatedBatch->quantity, 20);

        // 2. Process Customer Return of 2 units (Return to sellable stock)
        auto items = saleRepo.getSaleItems(saleRes.saleId);
        ASSERT_EQ(items.size(), 1);
        int saleItemId = items[0].id;

        auto retRes = salesService.processReturn(saleRes.saleId, saleItemId, 2, "Customer bought excess", true, 1);
        ASSERT_TRUE(retRes.success);
        ASSERT_NEAR(retRes.refundAmount, 10.00, 0.01);

        // Batch should now be 22
        auto restockedBatch = batchRepo.findById(batchId);
        ASSERT_EQ(restockedBatch->quantity, 22);

        // 3. Prevent returning more than originally bought
        auto overReturn = salesService.processReturn(saleRes.saleId, saleItemId, 50, "Impossible return", true, 1);
        ASSERT_FALSE(overReturn.success);
    }

    std::remove(dbFile.c_str());
}
