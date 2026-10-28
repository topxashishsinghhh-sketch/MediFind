#pragma once

#include "database/DatabaseConnection.hpp"
#include <string>

namespace medifind::database {

class DatabaseBackup {
public:
    /**
     * @brief Performs an atomic online backup of sourceDb to destinationPath.
     * Uses the official SQLite Backup API.
     * @return true on success, false on failure.
     */
    static bool createBackup(DatabaseConnection& sourceDb, const std::string& destinationPath, std::string& outErrorMessage);

    /**
     * @brief Restores database from backupPath into targetDb.
     * Safely preserves a temporary safety copy of targetDb before applying changes.
     * @return true on success, false on failure.
     */
    static bool restoreBackup(DatabaseConnection& targetDb, const std::string& backupPath, std::string& outErrorMessage);

    /**
     * @brief Verifies integrity and schema version of a backup file.
     */
    static bool verifyBackup(const std::string& backupPath, std::string& outErrorMessage);
};

} // namespace medifind::database
