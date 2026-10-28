# MediFind Web Operations Portal

Welcome to the **MediFind Web Operations Portal**, a modern, responsive Single Page Application (SPA) designed for hospital pharmacies and clinical dispensaries.

---

## 🌟 Key Features

1. **Executive Operations Dashboard**:
   - Real-time KPIs: Active Medicines, Queued Batches, Shelf Units, Asset Cost Basis, Projected Gross Retail Turnover, and Urgent Telemetry Alerts.
   - Live **FEFO Priority Dispatch Queue** showing next-to-expire batches with days remaining.
   - Real-time stock movement ledger.

2. **Medicine Master Catalog**:
   - Fast full-text & barcode search.
   - Category filtering (Analgesics, Antibiotics, Antidiabetics, etc.).
   - Dynamic threshold indicators (flags medicines below minimum reorder quantity).

3. **Batch & Expiry (FEFO) Management**:
   - Multi-batch tracking per medicine with distinct manufacturing and expiration dates.
   - Automatic quarantine for expired lots.
   - Status tracking: `AVAILABLE`, `NEAR_EXPIRY` (&le; 90 days), `EXPIRED`, `DEPLETED`.
   - **Direct Batch Stock-In workflow**.
   - **Manual Stock Adjustment with audit trail & mandatory reason justification** (Administrator restricted).

4. **Point of Sale (POS) & Clinical Dispenser**:
   - **Interactive FEFO Engine preview**: Before adding to cart, visually previews which specific batches and quantities will be allocated.
   - Automatic multi-batch spillover (e.g., if Batch A only has 10 units and 25 units are requested, it allocates 10 from Batch A and 15 from Batch B).
   - Instant printable receipt modal formatted with invoice references.

5. **Suppliers & Purchase Orders**:
   - Supplier registry with contact metadata.
   - Purchase order tracking with status updates (`APPROVED`, `RECEIVED`).

6. **Restock Request Pipeline (FIFO Queue)**:
   - Staff role can raise restock requests when shelf stock is low.
   - Administrator persona can review, approve, or reject submissions.

7. **Interactive DSA Diagnostics**:
   - **Hash Table (Separate Chaining)**: Interactive visualizer showing medicine bucket distribution (16 buckets) for O(1) cache lookups.
   - **Binary Search Tree (BST)**: Interactive prefix search demonstrating alphabetical in-order traversal autocomplete.
   - **FEFO Priority Queue**: Visual multi-batch prioritization based on expiration dates.

8. **Audit & Regulatory Compliance**:
   - Full immutable log tracking user actions, entity IDs, timestamps, and change summaries.

9. **Analytics & CSV Exports**:
   - Comprehensive **Inventory Valuation Report** distinguishing purchase cost book value from projected retail margin.
   - **Safety & Expiry Report**.
   - One-click CSV exports for Medicines, Batches, Sales Movements, and Audit Logs.

---

## 🚀 How to Run the Web Portal

### Option 1: Native C++ Web Server Binary (`medifind_server.exe`)
Built with C++17 and SQLite3, this native server starts an embedded HTTP server, connects to `medifind.db`, and automatically opens your web browser:
```powershell
cd C:\Users\AV\.gemini\antigravity\scratch\Medi-Inventory-Management\build
.\medifind_server.exe
```
This serves the application on `http://localhost:8080` with zero external dependencies (pure Winsock2 on Windows).

### Option 2: Direct Browser Launch (Zero Installation)
Simply double-click or open `web/index.html` in any modern web browser (Chrome, Firefox, Edge, Safari):
```
file:///C:/Users/AV/.gemini/antigravity/scratch/Medi-Inventory-Management/web/index.html
```

### Option 3: Using Python HTTP Server
```powershell
cd C:\Users\AV\.gemini\antigravity\scratch\Medi-Inventory-Management\web
python -m http.server 8080
```
Then navigate to: `http://localhost:8080`

### Option 3: Using Node / npx serve
```powershell
cd C:\Users\AV\.gemini\antigravity\scratch\Medi-Inventory-Management\web
npx serve -l 8080
```

---

## 👥 Demo User Personas

Click **"Switch Role / Logout"** in the lower left sidebar to toggle between:
- **Administrator (`admin`)**: Can stock-in batches, adjust stock, register new medicines, and approve restocks.
- **Employee (`employee1`)**: Can dispense medicines via POS, browse catalog, inspect FEFO batches, and submit restock requests.
