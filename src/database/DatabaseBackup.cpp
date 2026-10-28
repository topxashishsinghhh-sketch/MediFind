#include "database/DatabaseBackup.hpp"
#include "database/SchemaMigrations.hpp"
#include <fstream>
#include <filesystem>

namespace medifind::database {

bool DatabaseBackup::createBackup(DatabaseConnection& sourceDb, const std::string& destinationPath, std::string& outErrorMessage) {
    sqlite3* pDest = nullptr;
    int rc = sqlite3_open(destinationPath.c_str(), &pDest);
    if (rc != SQLITE_OK) {
        outErrorMessage = "Failed to open destination backup file: " + std::string(sqlite3_errmsg(pDest));
        if (pDest) sqlite3_close(pDest);
        return false;
    }

    sqlite3_backup* pBackup = sqlite3_backup_init(pDest, "main", sourceDb.getRawHandle(), "main");
    if (!pBackup) {
        outErrorMessage = "Failed to initialize SQLite backup: " + std::string(sqlite3_errmsg(pDest));
        sqlite3_close(pDest);
        return false;
    }

    rc = sqlite3_backup_step(pBackup, -1); // Copy all pages
    if (rc != SQLITE_DONE) {
        outErrorMessage = "SQLite backup step failed: " + std::string(sqlite3_errmsg(pDest));
        sqlite3_backup_finish(pBackup);
        sqlite3_close(pDest);
        return false;
    }

    sqlite3_backup_finish(pBackup);
    sqlite3_close(pDest);

    return verifyBackup(destinationPath, outErrorMessage);
}

bool DatabaseBackup::verifyBackup(const std::string& backupPath, std::string& outErrorMessage) {
    try {
        DatabaseConnection backupDb(backupPath);
        auto stmt = backupDb.prepare("PRAGMA quick_check;");
        if (stmt.step()) {
            std::string status = stmt.getText(0);
            if (status != "ok") {
                outErrorMessage = "Backup integrity check failed: " + status;
                return false;
            }
        }
        int ver = SchemaMigrations::getCurrentVersion(backupDb);
        if (ver < 1) {
            outErrorMessage = "Backup file is missing valid schema tables.";
            return false;
        }
        return true;
    } catch (const std::exception& ex) {
        outErrorMessage = std::string("Backup verification exception: ") + ex.what();
        return false;
    }
}

bool DatabaseBackup::restoreBackup(DatabaseConnection& targetDb, const std::string& backupPath, std::string& outErrorMessage) {
    // 1. Verify backup file first
    if (!verifyBackup(backupPath, outErrorMessage)) {
        return false;
    }

    // 2. Open backup file as source
    sqlite3* pSrc = nullptr;
    int rc = sqlite3_open(backupPath.c_str(), &pSrc);
    if (rc != SQLITE_OK) {
        outErrorMessage = "Failed to open backup source file: " + std::string(sqlite3_errmsg(pSrc));
        if (pSrc) sqlite3_close(pSrc);
        return false;
    }

    // 3. Backup from source file to targetDb
    sqlite3_backup* pBackup = sqlite3_backup_init(targetDb.getRawHandle(), "main", pSrc, "main");
    if (!pBackup) {
        outErrorMessage = "Failed to initialize restore backup: " + std::string(sqlite3_errmsg(targetDb.getRawHandle()));
        sqlite3_close(pSrc);
        return false;
    }

    rc = sqlite3_backup_step(pBackup, -1);
    if (rc != SQLITE_DONE) {
        outErrorMessage = "Restore step failed: " + std::string(sqlite3_errmsg(targetDb.getRawHandle()));
        sqlite3_backup_finish(pBackup);
        sqlite3_close(pSrc);
        return false;
    }

    sqlite3_backup_finish(pBackup);
    sqlite3_close(pSrc);

    return true;
}

} // namespace medifind::database
