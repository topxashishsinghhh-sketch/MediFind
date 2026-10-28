#include "database/DatabaseConnection.hpp"
#include <sstream>

namespace medifind::database {

// ========================================================
// PreparedStatement
// ========================================================

void PreparedStatement::bindInt(int paramIndex, int value) {
    if (sqlite3_bind_int(stmt_, paramIndex, value) != SQLITE_OK) {
        throw DatabaseException("Failed to bind integer at index " + std::to_string(paramIndex));
    }
}

void PreparedStatement::bindInt64(int paramIndex, int64_t value) {
    if (sqlite3_bind_int64(stmt_, paramIndex, value) != SQLITE_OK) {
        throw DatabaseException("Failed to bind int64 at index " + std::to_string(paramIndex));
    }
}

void PreparedStatement::bindDouble(int paramIndex, double value) {
    if (sqlite3_bind_double(stmt_, paramIndex, value) != SQLITE_OK) {
        throw DatabaseException("Failed to bind double at index " + std::to_string(paramIndex));
    }
}

void PreparedStatement::bindText(int paramIndex, const std::string& value) {
    if (sqlite3_bind_text(stmt_, paramIndex, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        throw DatabaseException("Failed to bind text at index " + std::to_string(paramIndex));
    }
}

void PreparedStatement::bindNull(int paramIndex) {
    if (sqlite3_bind_null(stmt_, paramIndex) != SQLITE_OK) {
        throw DatabaseException("Failed to bind null at index " + std::to_string(paramIndex));
    }
}

bool PreparedStatement::step() {
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW) {
        return true;
    } else if (rc == SQLITE_DONE) {
        return false;
    } else {
        std::string err = sqlite3_errmsg(sqlite3_db_handle(stmt_));
        throw DatabaseException("Error during statement step: " + err);
    }
}

void PreparedStatement::reset() {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
}

int PreparedStatement::getInt(int colIndex) const {
    return sqlite3_column_int(stmt_, colIndex);
}

int64_t PreparedStatement::getInt64(int colIndex) const {
    return sqlite3_column_int64(stmt_, colIndex);
}

double PreparedStatement::getDouble(int colIndex) const {
    return sqlite3_column_double(stmt_, colIndex);
}

std::string PreparedStatement::getText(int colIndex) const {
    const unsigned char* text = sqlite3_column_text(stmt_, colIndex);
    if (!text) return "";
    return std::string(reinterpret_cast<const char*>(text));
}

bool PreparedStatement::isNull(int colIndex) const {
    return sqlite3_column_type(stmt_, colIndex) == SQLITE_NULL;
}

// ========================================================
// DatabaseConnection
// ========================================================

DatabaseConnection::DatabaseConnection(const std::string& dbPath) : dbPath_(dbPath) {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    int rc = sqlite3_open_v2(dbPath.c_str(), &db_, flags, nullptr);
    if (rc != SQLITE_OK) {
        std::string errMsg = db_ ? sqlite3_errmsg(db_) : "Unable to allocate sqlite3 handle";
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        throw DatabaseException("Failed to open database at " + dbPath + ": " + errMsg);
    }

    // Enforce foreign key constraints and WAL journal mode
    execute("PRAGMA foreign_keys = ON;");
    execute("PRAGMA journal_mode = WAL;");
    execute("PRAGMA synchronous = NORMAL;");
}

DatabaseConnection::~DatabaseConnection() {
    if (db_) {
        sqlite3_close_v2(db_);
        db_ = nullptr;
    }
}

void DatabaseConnection::execute(const std::string& sql) {
    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::string err = zErrMsg ? zErrMsg : "Unknown error";
        sqlite3_free(zErrMsg);
        throw DatabaseException("SQL execution error: " + err + " (Query: " + sql + ")");
    }
}

PreparedStatement DatabaseConnection::prepare(const std::string& sql) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        throw DatabaseException("SQL prepare error: " + err + " (Query: " + sql + ")");
    }
    return PreparedStatement(stmt);
}

int64_t DatabaseConnection::lastInsertRowId() const {
    return sqlite3_last_insert_rowid(db_);
}

int DatabaseConnection::changes() const {
    return sqlite3_changes(db_);
}

void DatabaseConnection::beginTransaction() {
    execute("BEGIN TRANSACTION;");
}

void DatabaseConnection::commitTransaction() {
    execute("COMMIT;");
}

void DatabaseConnection::rollbackTransaction() {
    execute("ROLLBACK;");
}

} // namespace medifind::database
