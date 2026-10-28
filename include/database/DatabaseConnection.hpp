#pragma once

#include "sqlite3.h"
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <functional>

namespace medifind::database {

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& msg) : std::runtime_error(msg) {}
};

class PreparedStatement {
private:
    sqlite3_stmt* stmt_{nullptr};

public:
    explicit PreparedStatement(sqlite3_stmt* stmt) : stmt_(stmt) {}
    ~PreparedStatement() {
        if (stmt_) {
            sqlite3_finalize(stmt_);
            stmt_ = nullptr;
        }
    }

    // Disable copy, enable move
    PreparedStatement(const PreparedStatement&) = delete;
    PreparedStatement& operator=(const PreparedStatement&) = delete;

    PreparedStatement(PreparedStatement&& other) noexcept : stmt_(other.stmt_) {
        other.stmt_ = nullptr;
    }

    PreparedStatement& operator=(PreparedStatement&& other) noexcept {
        if (this != &other) {
            if (stmt_) sqlite3_finalize(stmt_);
            stmt_ = other.stmt_;
            other.stmt_ = nullptr;
        }
        return *this;
    }

    sqlite3_stmt* get() const { return stmt_; }

    void bindInt(int paramIndex, int value);
    void bindInt64(int paramIndex, int64_t value);
    void bindDouble(int paramIndex, double value);
    void bindText(int paramIndex, const std::string& value);
    void bindNull(int paramIndex);

    bool step(); // Returns true if SQLITE_ROW, false if SQLITE_DONE
    void reset();

    int getInt(int colIndex) const;
    int64_t getInt64(int colIndex) const;
    double getDouble(int colIndex) const;
    std::string getText(int colIndex) const;
    bool isNull(int colIndex) const;
};

class DatabaseConnection {
private:
    sqlite3* db_{nullptr};
    std::string dbPath_;

public:
    explicit DatabaseConnection(const std::string& dbPath);
    ~DatabaseConnection();

    // Disable copy
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    sqlite3* getRawHandle() const { return db_; }
    const std::string& getPath() const { return dbPath_; }

    void execute(const std::string& sql);
    PreparedStatement prepare(const std::string& sql);

    int64_t lastInsertRowId() const;
    int changes() const;

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
};

class TransactionGuard {
private:
    DatabaseConnection& db_;
    bool committed_{false};

public:
    explicit TransactionGuard(DatabaseConnection& db) : db_(db) {
        db_.beginTransaction();
    }

    ~TransactionGuard() {
        if (!committed_) {
            try {
                db_.rollbackTransaction();
            } catch (...) {
                // Suppress in destructor
            }
        }
    }

    void commit() {
        if (!committed_) {
            db_.commitTransaction();
            committed_ = true;
        }
    }
};

} // namespace medifind::database
