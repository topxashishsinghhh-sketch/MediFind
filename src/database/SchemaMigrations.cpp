#include "database/SchemaMigrations.hpp"
#include <iostream>

namespace medifind::database {

void SchemaMigrations::applyMigrations(DatabaseConnection& db) {
    // 1. Create schema_version tracking table if not present
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS schema_version (
            version INTEGER PRIMARY KEY,
            applied_at TEXT NOT NULL
        );
    )");

    int currentVer = getCurrentVersion(db);

    if (currentVer < 1) {
        migrateToV1(db);
        db.execute("INSERT INTO schema_version (version, applied_at) VALUES (1, datetime('now'));");
    }
}

int SchemaMigrations::getCurrentVersion(DatabaseConnection& db) {
    try {
        auto stmt = db.prepare("SELECT COALESCE(MAX(version), 0) FROM schema_version;");
        if (stmt.step()) {
            return stmt.getInt(0);
        }
    } catch (...) {
        return 0;
    }
    return 0;
}

void SchemaMigrations::migrateToV1(DatabaseConnection& db) {
    TransactionGuard tx(db);

    // 1. Users table
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            salt TEXT NOT NULL,
            full_name TEXT NOT NULL,
            role TEXT NOT NULL CHECK(role IN ('ADMINISTRATOR', 'EMPLOYEE')),
            is_active INTEGER NOT NULL DEFAULT 1,
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        );
        CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
    )");

    // 2. Suppliers table
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS suppliers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            contact_person TEXT,
            email TEXT,
            phone TEXT,
            address TEXT,
            is_active INTEGER NOT NULL DEFAULT 1,
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        );
        CREATE INDEX IF NOT EXISTS idx_suppliers_name ON suppliers(name);
    )");

    // 3. Medicines table
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS medicines (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            brand TEXT,
            strength TEXT,
            dosage_form TEXT,
            category TEXT,
            barcode TEXT UNIQUE,
            description TEXT,
            min_stock_threshold INTEGER NOT NULL DEFAULT 10,
            is_active INTEGER NOT NULL DEFAULT 1,
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        );
        CREATE INDEX IF NOT EXISTS idx_medicines_name ON medicines(name);
        CREATE INDEX IF NOT EXISTS idx_medicines_category ON medicines(category);
        CREATE INDEX IF NOT EXISTS idx_medicines_barcode ON medicines(barcode);
    )");

    // 4. Batches table (Core of FEFO inventory)
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS batches (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            medicine_id INTEGER NOT NULL REFERENCES medicines(id) ON DELETE RESTRICT,
            batch_number TEXT NOT NULL,
            mfg_date TEXT NOT NULL,
            expiry_date TEXT NOT NULL,
            quantity INTEGER NOT NULL CHECK(quantity >= 0),
            purchase_cost REAL NOT NULL CHECK(purchase_cost >= 0.0),
            selling_price REAL NOT NULL CHECK(selling_price >= 0.0),
            supplier_id INTEGER REFERENCES suppliers(id) ON DELETE SET NULL,
            storage_location TEXT,
            status TEXT NOT NULL CHECK(status IN ('AVAILABLE', 'NEAR_EXPIRY', 'EXPIRED', 'DEPLETED', 'BLOCKED')),
            created_at TEXT NOT NULL DEFAULT (datetime('now')),
            UNIQUE(medicine_id, batch_number)
        );
        CREATE INDEX IF NOT EXISTS idx_batches_med_exp ON batches(medicine_id, expiry_date, quantity);
        CREATE INDEX IF NOT EXISTS idx_batches_expiry ON batches(expiry_date);
        CREATE INDEX IF NOT EXISTS idx_batches_status ON batches(status);
    )");

    // 5. Purchase Orders & Items
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS purchase_orders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            order_ref TEXT NOT NULL UNIQUE,
            supplier_id INTEGER NOT NULL REFERENCES suppliers(id) ON DELETE RESTRICT,
            order_date TEXT NOT NULL,
            expected_delivery_date TEXT,
            status TEXT NOT NULL CHECK(status IN ('DRAFT', 'SUBMITTED', 'APPROVED', 'REJECTED', 'PARTIALLY_RECEIVED', 'RECEIVED', 'CANCELLED')),
            total_cost REAL NOT NULL DEFAULT 0.0,
            notes TEXT,
            created_by_user_id INTEGER REFERENCES users(id),
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        );

        CREATE TABLE IF NOT EXISTS purchase_order_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            purchase_order_id INTEGER NOT NULL REFERENCES purchase_orders(id) ON DELETE CASCADE,
            medicine_id INTEGER NOT NULL REFERENCES medicines(id) ON DELETE RESTRICT,
            requested_quantity INTEGER NOT NULL CHECK(requested_quantity > 0),
            received_quantity INTEGER NOT NULL DEFAULT 0 CHECK(received_quantity >= 0),
            unit_cost REAL NOT NULL CHECK(unit_cost >= 0.0)
        );
        CREATE INDEX IF NOT EXISTS idx_po_supplier ON purchase_orders(supplier_id);
    )");

    // 6. Sales & Sale Items
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS sales (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            invoice_number TEXT NOT NULL UNIQUE,
            sale_date TEXT NOT NULL DEFAULT (datetime('now')),
            sold_by_user_id INTEGER NOT NULL REFERENCES users(id),
            total_amount REAL NOT NULL CHECK(total_amount >= 0.0),
            payment_method TEXT NOT NULL,
            customer_name TEXT,
            notes TEXT
        );

        CREATE TABLE IF NOT EXISTS sale_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sale_id INTEGER NOT NULL REFERENCES sales(id) ON DELETE CASCADE,
            medicine_id INTEGER NOT NULL REFERENCES medicines(id),
            batch_id INTEGER NOT NULL REFERENCES batches(id),
            quantity INTEGER NOT NULL CHECK(quantity > 0),
            unit_price REAL NOT NULL CHECK(unit_price >= 0.0),
            subtotal REAL NOT NULL CHECK(subtotal >= 0.0)
        );
        CREATE INDEX IF NOT EXISTS idx_sales_date ON sales(sale_date);
        CREATE INDEX IF NOT EXISTS idx_sale_items_batch ON sale_items(batch_id);
    )");

    // 7. Returns table
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS returns (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            return_ref TEXT NOT NULL UNIQUE,
            sale_id INTEGER NOT NULL REFERENCES sales(id),
            sale_item_id INTEGER NOT NULL REFERENCES sale_items(id),
            medicine_id INTEGER NOT NULL REFERENCES medicines(id),
            batch_id INTEGER NOT NULL REFERENCES batches(id),
            quantity INTEGER NOT NULL CHECK(quantity > 0),
            reason TEXT NOT NULL,
            return_to_sellable_stock INTEGER NOT NULL DEFAULT 1,
            refund_amount REAL NOT NULL DEFAULT 0.0,
            processed_by_user_id INTEGER NOT NULL REFERENCES users(id),
            return_date TEXT NOT NULL DEFAULT (datetime('now'))
        );
    )");

    // 8. Stock Movements (Audit Trail of all inventory changes)
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS stock_movements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            medicine_id INTEGER NOT NULL REFERENCES medicines(id),
            batch_id INTEGER NOT NULL REFERENCES batches(id),
            type TEXT NOT NULL,
            quantity INTEGER NOT NULL,
            balance_after INTEGER NOT NULL,
            reference_type TEXT,
            reference_id TEXT,
            reason TEXT,
            user_id INTEGER REFERENCES users(id),
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        );
        CREATE INDEX IF NOT EXISTS idx_stock_movements_med ON stock_movements(medicine_id, created_at);
        CREATE INDEX IF NOT EXISTS idx_stock_movements_batch ON stock_movements(batch_id);
    )");

    // 9. Restock Requests
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS restock_requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            request_ref TEXT NOT NULL UNIQUE,
            medicine_id INTEGER NOT NULL REFERENCES medicines(id),
            requested_quantity INTEGER NOT NULL CHECK(requested_quantity > 0),
            reason TEXT NOT NULL,
            status TEXT NOT NULL CHECK(status IN ('PENDING', 'APPROVED', 'REJECTED', 'ORDERED', 'RECEIVED', 'CANCELLED')),
            requested_by_user_id INTEGER NOT NULL REFERENCES users(id),
            approved_by_user_id INTEGER REFERENCES users(id),
            review_notes TEXT,
            created_at TEXT NOT NULL DEFAULT (datetime('now')),
            updated_at TEXT NOT NULL DEFAULT (datetime('now'))
        );
        CREATE INDEX IF NOT EXISTS idx_restock_status ON restock_requests(status);
    )");

    // 10. Audit Logs
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS audit_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER REFERENCES users(id),
            username TEXT NOT NULL,
            action TEXT NOT NULL,
            entity_type TEXT NOT NULL,
            entity_id TEXT NOT NULL,
            change_summary TEXT,
            timestamp TEXT NOT NULL DEFAULT (datetime('now')),
            client_context TEXT NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_audit_logs_timestamp ON audit_logs(timestamp);
        CREATE INDEX IF NOT EXISTS idx_audit_logs_action ON audit_logs(action);
    )");

    tx.commit();
}

} // namespace medifind::database
