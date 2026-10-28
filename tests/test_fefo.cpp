#include "TestFramework.hpp"
#include "database/DatabaseConnection.hpp"
#include "database/SchemaMigrations.hpp"
#include "repositories/MedicineRepository.hpp"
#include "repositories/BatchRepository.hpp"
#include "services/FefoAllocator.hpp"
#include <cstdio>

using namespace medifind;

TEST(FefoSuite, EarliestExpiryPriorityAndSkippingExpired) {
    const std::string dbFile = "test_fefo.db";
    std::remove(dbFile.c_str());

    {
        database::DatabaseConnection db(dbFile);
        database::SchemaMigrations::applyMigrations(db);

        repositories::MedicineRepository medRepo(db);
        repositories::BatchRepository batchRepo(db);
        services::FefoAllocator allocator(batchRepo);

        // 1. Create Medicine
        models::Medicine med;
        med.name = "Amoxicillin 500mg";
        med.strength = "500 mg";
        int medId = medRepo.createMedicine(med);

        // Current reference date: 2026-10-27
        const std::string today = "2026-10-27";

        // Insert Batch 1: Expired (2026-09-01), 50 units
        models::Batch bExp;
        bExp.medicineId = medId;
        bExp.batchNumber = "B-EXPIRED";
        bExp.mfgDate = "2024-09-01";
        bExp.expiryDate = "2026-09-01";
        bExp.quantity = 50;
        bExp.sellingPrice = 10.0;
        bExp.status = models::BatchStatus::EXPIRED;
        batchRepo.createBatch(bExp);

        // Insert Batch 2: Near Expiry (2026-11-15), 20 units -> Should be consumed first!
        models::Batch bNear;
        bNear.medicineId = medId;
        bNear.batchNumber = "B-EARLY";
        bNear.mfgDate = "2024-11-15";
        bNear.expiryDate = "2026-11-15";
        bNear.quantity = 20;
        bNear.sellingPrice = 10.0;
        bNear.status = models::BatchStatus::AVAILABLE;
        int bNearId = batchRepo.createBatch(bNear);

        // Insert Batch 3: Later Expiry (2027-06-30), 100 units -> Should be consumed second!
        models::Batch bLater;
        bLater.medicineId = medId;
        bLater.batchNumber = "B-LATER";
        bLater.mfgDate = "2025-06-30";
        bLater.expiryDate = "2027-06-30";
        bLater.quantity = 100;
        bLater.sellingPrice = 10.0;
        bLater.status = models::BatchStatus::AVAILABLE;
        int bLaterId = batchRepo.createBatch(bLater);

        // Case 1: Request 15 units -> Must come solely from B-EARLY
        auto plan1 = allocator.planAllocation(medId, 15, today);
        ASSERT_TRUE(plan1.isSuccessful);
        ASSERT_EQ(plan1.items.size(), 1);
        ASSERT_EQ(plan1.items[0].batchId, bNearId);
        ASSERT_EQ(plan1.items[0].allocatedQuantity, 15);

        // Case 2: Request 35 units -> Must take all 20 from B-EARLY, then 15 from B-LATER
        auto plan2 = allocator.planAllocation(medId, 35, today);
        ASSERT_TRUE(plan2.isSuccessful);
        ASSERT_EQ(plan2.items.size(), 2);
        ASSERT_EQ(plan2.items[0].batchId, bNearId);
        ASSERT_EQ(plan2.items[0].allocatedQuantity, 20);
        ASSERT_EQ(plan2.items[1].batchId, bLaterId);
        ASSERT_EQ(plan2.items[1].allocatedQuantity, 15);
        ASSERT_EQ(plan2.totalAllocatedQuantity, 35);

        // Case 3: Request 150 units -> Available is 20 + 100 = 120 (Expired batch must NOT be counted).
        auto plan3 = allocator.planAllocation(medId, 150, today);
        ASSERT_FALSE(plan3.isSuccessful); // Must fail cleanly due to insufficient unexpired stock!
    }

    std::remove(dbFile.c_str());
}
