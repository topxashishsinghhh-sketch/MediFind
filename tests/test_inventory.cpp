#include "TestFramework.hpp"
#include "database/DatabaseConnection.hpp"
#include "database/SchemaMigrations.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "repositories/StockMovementRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "services/InventoryService.hpp"
#include <cstdio>

using namespace medifind;

TEST(InventorySuite, MedicineAndStockOperations) {
    const std::string dbFile = "test_inv.db";
    std::remove(dbFile.c_str());

    {
        database::DatabaseConnection db(dbFile);
        database::SchemaMigrations::applyMigrations(db);

        // Seed a test user (id=1) required by stock_movements and audit_logs FK constraints.
        db.execute(R"(
            INSERT INTO users (id, username, password_hash, salt, full_name, role, is_active)
            VALUES (1, 'testadmin', 'hash', 'salt', 'Test Admin', 'ADMINISTRATOR', 1);
        )");

        repositories::MedicineRepository medRepo(db);
        repositories::BatchRepository batchRepo(db);
        repositories::StockMovementRepository movRepo(db);
        repositories::AuditLogRepository auditRepo(db);

        services::InventoryService invService(db, medRepo, batchRepo, movRepo, auditRepo);

        // 1. Add Medicine with barcode
        models::Medicine m;
        m.name = "Paracetamol 650mg";
        m.strength = "650 mg";
        m.barcode = "BARCODE-12345";
        m.minStockThreshold = 10;
        std::string err;
        int medId = invService.addMedicine(m, 1, err);
        ASSERT_TRUE(medId > 0);

        // 2. Duplicate barcode must fail
        models::Medicine mDup;
        mDup.name = "Other Medicine";
        mDup.barcode = "BARCODE-12345";
        int dupId = invService.addMedicine(mDup, 1, err);
        ASSERT_EQ(dupId, -1);

        // 3. Stock in batch
        models::Batch b;
        b.medicineId = medId;
        b.batchNumber = "BAT-PARA-01";
        b.mfgDate = "2025-01-01";
        b.expiryDate = "2027-01-01";
        b.quantity = 50;
        b.purchaseCost = 1.50;
        b.sellingPrice = 3.00;
        int batchId = invService.stockInBatch(b, 1, "RECEIPT", "REC-01", err);
        ASSERT_TRUE(batchId > 0);

        // Verify stock quantity and valuation
        ASSERT_EQ(invService.getTotalUnitsInStock(), 50);
        ASSERT_NEAR(invService.getTotalInventoryCostValue(), 75.0, 0.01);
        ASSERT_NEAR(invService.getTotalInventoryRetailValue(), 150.0, 0.01);

        // 4. Adjust stock
        auto adjRes = invService.adjustStock(batchId, 45, "Cycle count adjustment", 1, true);
        ASSERT_TRUE(adjRes.success);
        ASSERT_EQ(adjRes.newQuantity, 45);
        ASSERT_EQ(adjRes.difference, -5);
        ASSERT_EQ(invService.getTotalUnitsInStock(), 45);

        // 5. Unauthorized employee adjustment must fail
        auto unauthRes = invService.adjustStock(batchId, 40, "Stealing stock", 2, false);
        ASSERT_FALSE(unauthRes.success);
    }

    std::remove(dbFile.c_str());
}
