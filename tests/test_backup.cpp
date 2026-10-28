#include "TestFramework.hpp"
#include "database/DatabaseConnection.hpp"
#include "database/SchemaMigrations.hpp"
#include "database/DatabaseBackup.hpp"
#include "repositories/MedicineRepository.hpp"
#include <cstdio>

using namespace medifind;

TEST(BackupSuite, CreateAndRestoreLiveBackup) {
    const std::string originalDb = "test_orig.db";
    const std::string backupDb = "test_backup.db";
    const std::string restoredDb = "test_restored.db";

    std::remove(originalDb.c_str());
    std::remove(backupDb.c_str());
    std::remove(restoredDb.c_str());

    {
        database::DatabaseConnection db(originalDb);
        database::SchemaMigrations::applyMigrations(db);

        repositories::MedicineRepository medRepo(db);
        models::Medicine m;
        m.name = "Backup Test Med";
        m.strength = "100 mg";
        int medId = medRepo.createMedicine(m);
        ASSERT_TRUE(medId > 0);

        // 1. Create live online backup
        std::string err;
        bool bkpOk = database::DatabaseBackup::createBackup(db, backupDb, err);
        ASSERT_TRUE(bkpOk);
    }

    // 2. Verify backup file independently
    std::string err;
    ASSERT_TRUE(database::DatabaseBackup::verifyBackup(backupDb, err));

    // 3. Restore backup into a fresh target database
    {
        database::DatabaseConnection targetDb(restoredDb);
        database::SchemaMigrations::applyMigrations(targetDb);

        bool resOk = database::DatabaseBackup::restoreBackup(targetDb, backupDb, err);
        ASSERT_TRUE(resOk);

        // Verify restored contents
        repositories::MedicineRepository restoredMedRepo(targetDb);
        auto found = restoredMedRepo.findByName("Backup Test Med");
        ASSERT_FALSE(found.empty());
        ASSERT_EQ(found[0].name, "Backup Test Med");
    }

    std::remove(originalDb.c_str());
    std::remove(backupDb.c_str());
    std::remove(restoredDb.c_str());
}
