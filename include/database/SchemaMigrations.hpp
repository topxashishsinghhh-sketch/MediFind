#pragma once

#include "database/DatabaseConnection.hpp"

namespace medifind::database {

class SchemaMigrations {
public:
    /**
     * @brief Executes all pending migrations against the database.
     * Ensures schema version tracking and idempotent execution.
     */
    static void applyMigrations(DatabaseConnection& db);

    /**
     * @brief Gets current schema version stored in the database.
     */
    static int getCurrentVersion(DatabaseConnection& db);

private:
    static void migrateToV1(DatabaseConnection& db);
};

} // namespace medifind::database
