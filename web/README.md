# MediFind — Modern SaaS Pharmacy & FEFO Management Suite

Welcome to the **MediFind Web Operations Suite**, an enterprise-grade Single Page Application (SPA) designed for hospital pharmacies, clinical dispensaries, and pharmaceutical warehouses.

---

## 🌟 SaaS Design & Architecture

The web interface is engineered around modern SaaS principles:
* **Navy & Teal Aesthetic:** Deep navy sidebar (`#0f172a` / `#1e293b`), crisp teal brand accents (`#0d9488` / `#14b8a6`), clean white cards, subtle borders, and `Plus Jakarta Sans` typography.
* **Grouped 5-Section Navigation:** Structured into **Overview**, **Inventory**, **Operations**, **Insights**, and **Administration**.
* **Role-Based Views & Application-Level Enforcement:** 
  - **Administrator:** Full catalog CRUD, stock adjustment approvals, purchase order creation, regulatory audit ledger, system user provisioning, and pharmacy GST settings.
  - **Staff / Employee:** Controlled operational dispensing (POS), real-time inventory lookup, inbound delivery receiving against approved POs, stock adjustment request submission (with mandatory justification), restock requests, and personal shift transaction ledger.
* **Clinical Pharmacy Focus:** All educational demonstrator panels and Big-O algorithm explanations have been removed from the user-facing pharmacy interface while preserving custom DSA structures (`HashTable`, `BST`, `Queue`, `Stack`, `Sorting`) in the C++ core and academic documentation.

---

## 🚀 Key Functional Modules

1. **Role-Based Operations Dashboard**:
   - **Administrator View:** Executive KPIs (Active Medicines, Queued Batches, Shelf Units, Cost Book Valuation, Projected Retail Revenue, and Safety Alerts), Category distribution chart, 90-day expiry radar, FEFO priority dispatch queue, and recent stock movements ledger.
   - **Staff View:** Shift operations dashboard, quick dispense shortcuts, inbound stock receiving, and adjustment request shortcuts.

2. **Medicine Master Inventory**:
   - Fast full-text search and barcode scanner simulation.
   - Category filtering (Analgesics, Antibiotics, Antidiabetics, Cardiovascular, etc.).
   - Dynamic threshold indicators (flags medicines below minimum reorder quantity).
   - Direct batch drilldown and admin medicine registration.

3. **Batch & FEFO Expiry Management**:
   - Multi-batch tracking per medicine with manufacturing and expiration dates.
   - Status tracking: `AVAILABLE`, `NEAR_EXPIRY` (&le; 90 days), `EXPIRED` (quarantined), `DEPLETED`.
   - Direct batch stock-in workflow for authorized administrators.

4. **Point of Sale (POS) & GST Invoicing**:
   - **Live FEFO Engine Allocation:** Automatically selects and displays earliest-expiring non-expired batches for cart items.
   - Automatic multi-batch allocation when dispensing exceeds single batch quantity.
   - **Expired Stock Safety Block:** Batches past expiry date are strictly prohibited from POS allocation.
   - **Indian GST Tax Engine:** Computes subtotal, CGST, SGST, and Grand Total with selectable tax rates (12%, 18%, 5%, exempt).
   - **Print-Ready Tax Invoice:** Modal receipt formatted with Pharmacy Name, GSTIN, Drug License Number, Doctor Reference, Customer info, line items with HSN/Batch/Expiry, and authorized pharmacist signature.

5. **Inbound Stock Receiving**:
   - Delivery verification station matching physical deliveries against approved Purchase Orders.
   - Staff enters delivery challan, inspected batch numbers, manufacturing dates, and verified expiry dates.
   - Automatically records stock receipts into inventory and logs audit movements.

6. **Stock Adjustment Requests & Approval Pipeline**:
   - Operational staff can submit adjustment requests with mandatory justification and categorization (Damaged, Discrepancy, Expired Quarantine, Return).
   - Administrator approval pipeline reviews, approves, or rejects requests before physical stock balances are modified.

7. **Suppliers & Purchase Order Lifecycle**:
   - Healthcare supplier directory with contact metadata.
   - Purchase order lifecycle tracker: `DRAFT` &rarr; `APPROVED` &rarr; `RECEIVED`.

8. **Floor Restock Pipeline (FIFO Queue)**:
   - Staff raises floor replenishment requests when shelf stock is depleted.
   - Administrator reviews, approves, or rejects requests.

9. **Customer Records & Sales Ledger**:
   - Full transaction history searchable by customer name, phone, or invoice reference.
   - One-click reprint of historical GST tax invoices.

10. **Reports & Valuation**:
    - Comprehensive **Inventory Valuation Report** distinguishing purchase-cost book value from projected retail turnover and margin spread.
    - **Safety & Expiry Report** with 90-day horizon lookahead.
    - One-click CSV exports for Medicines, Batches, Sales, Movements, and Audit Logs.

11. **System Administration & Settings**:
    - **Notification Center:** Slide-out drawer and center tracking unread alerts.
    - **User Management:** User provisioning, role assignment, and active/suspended toggling.
    - **Pharmacy Settings:** Configurable pharmacy name, GSTIN, DL number, address, currency, and default GST rates.
    - **User Profile:** Session details and active role privileges.

---

## 💻 How to Run the Web Application

### Option 1: Native C++ HTTP Web Server (`medifind_server.exe`)
Built with C++17 and embedded SQLite3, this native server starts an embedded HTTP server, connects to `medifind.db`, and serves the web application on port 8080:
```powershell
cd C:\Users\AV\.gemini\antigravity\scratch\Medi-Inventory-Management\build
.\medifind_server.exe 8080
```
Open **[http://localhost:8080](http://localhost:8080)** in your browser.

### Option 2: Direct Browser Launch
Open `web/index.html` in any modern web browser (Chrome, Edge, Firefox, Safari):
```
file:///C:/Users/AV/.gemini/antigravity/scratch/Medi-Inventory-Management/web/index.html
```

---

## 👥 Pre-Configured Demo Accounts

Click **"Switch Role / Logout"** in the sidebar footer to switch personas:
* **Administrator (`admin` / `Admin@123`)**: Full management, approvals, reports, user management, audit logs, and settings.
* **Staff / Employee (`employee1` / `Staff@123`)**: Daily operations, POS dispensing, stock checking, inbound receiving, and request submissions.
