# MediFind — Advanced Medical Inventory & FEFO Management System

> An enterprise-grade clinical pharmacy inventory management system built in **C++17**, featuring a **modern Web UI/UX**, native C++ embedded HTTP web server, FEFO stock allocation, role-based access control (RBAC), SQLite persistence, a full console CLI, and custom Data Structures & Algorithms.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [UI/UX & Features](#uiux--features)
3. [Architecture](#architecture)
4. [Technology Stack](#technology-stack)
5. [Prerequisites & Build Instructions](#prerequisites--build-instructions)
6. [Running the Application](#running-the-application)
7. [User Roles and Demo Credentials](#user-roles-and-demo-credentials)
8. [FEFO & Expiry Protection Engine](#fefo--expiry-protection-engine)
9. [DSA Implementations & Diagnostics](#dsa-implementations--diagnostics)
10. [Database Schema](#database-schema)
11. [Running Automated Tests](#running-automated-tests)
12. [Reports & Financial Valuations](#reports--financial-valuations)
13. [Viva Q&A](#viva-qa)

---

## Project Overview

MediFind is a high-performance inventory management platform designed for pharmacies and clinical dispensaries. It demonstrates practical, production-level implementation of Data Structures and Algorithms (DSA) and Object-Oriented Programming (OOP) in C++17, paired with an executive Web UI/UX.

The system manages:
- **Medicine Master Records & Multi-Batch Tracking**
- **FEFO (First Expired, First Out) Automatic Stock Allocation**
- **Point of Sale (POS) & Clinical Dispensing with live multi-batch breakdown**
- **Automated Expiry Quarantining & Minimum Stock Threshold Alerts**
- **Supplier Procurement, Purchase Orders, and Stock Receipt Workflows**
- **Staff Restock Request Pipeline (FIFO Queue) with Administrator Approval**
- **Manual Stock Adjustments with Mandatory Audit Justification**
- **Role-Based Access Control (Administrator vs Employee)**
- **Immutable Audit Trail & Regulatory Movement Ledger**
- **Interactive DSA Visualizers** (HashTable, BST, FEFO Priority Queue)
- **Financial Inventory Valuation (Cost Book-Value vs Retail Turnover)**

---

## UI/UX & Features

MediFind provides two first-class interfaces that operate over the authoritative SQLite database:
1. **Modern SaaS Web Suite**: Designed with a clean navy & teal SaaS aesthetic (light/dark mode, Chart.js analytics, Indian GST tax invoicing, role-based 5-section navigation, and notification center).
2. **Interactive Terminal CLI (`medifind_console`)**: A structured command-line interface for terminal usage, automated workflows, and college viva demonstration.

| Feature | Web Suite UI | Console CLI |
|---|:---:|:---:|
| Role-based authentication & views (Admin / Staff) | ✅ | ✅ |
| Executive & Shift KPI Dashboards | ✅ | ✅ |
| Therapeutic Category Distribution Chart | ✅ | — |
| Expiry Risk Timeline Radar Chart | ✅ | — |
| Medicine Catalog & Barcode Search | ✅ | ✅ |
| Multi-Batch Management & Quarantine | ✅ | ✅ |
| Inbound Delivery Receiving Station (against PO) | ✅ | ✅ |
| POS Dispenser with Live FEFO Batch Allocation | ✅ | ✅ |
| Indian GST Tax Breakdown (CGST + SGST) | ✅ | — |
| Print-Ready GST Tax Invoices | ✅ | ✅ |
| Customer Records & Sales Ledger | ✅ | ✅ |
| Stock Adjustment Workflow (Mandatory Justification & Admin Sign-off) | ✅ | ✅ |
| Supplier Directory & Purchase Order Lifecycle | ✅ | ✅ |
| Floor Restock Request Pipeline (FIFO Queue) | ✅ | ✅ |
| User Provisioning & Pharmacy Settings | ✅ | ✅ |
| Notification Drawer & Safety Alert Badges | ✅ | — |
| Immutable System Audit Ledger | ✅ | ✅ |
| Cost-Basis vs Retail Margin Valuation | ✅ | ✅ |
| One-Click CSV Data Exports | ✅ | ✅ |

---

## Architecture

The project follows a strict layered software architecture:

```
┌─────────────────────────────────────────────────────────────┐
│                   Presentation Layer                        │
│   Clinical Web UI (HTML/CSS/JS)   │   Console Terminal CLI  │
├─────────────────────────────────────────────────────────────┤
│                 Embedded HTTP Web Server                    │
│   Native C++17 Multi-Threaded Winsock Server / REST API     │
├─────────────────────────────────────────────────────────────┤
│                 Business Services Layer                     │
│  AuthService  │  FefoAllocator  │  InventoryService         │
│  PurchasingService  │  SalesService  │  ReportService       │
├─────────────────────────────────────────────────────────────┤
│             Domain Models & Business Rules                  │
│  Medicine  │  Batch  │  Sale  │  PurchaseOrder  │  User ... │
├─────────────────────────────────────────────────────────────┤
│               Custom DSA Data Structures                    │
│  HashTable  │  BST  │  LinkedList  │  Queue  │  Stack       │
│  MergeSort  │  QuickSort  │  BinarySearch                   │
├─────────────────────────────────────────────────────────────┤
│                     Repository Layer                        │
│  MedicineRepo  │  BatchRepo  │  SaleRepo  │  AuditRepo ...  │
├─────────────────────────────────────────────────────────────┤
│              Authoritative Persistence Layer                │
│  SQLite3 (Embedded Amalgamation v3.46.1)                    │
└─────────────────────────────────────────────────────────────┘
```

**Key Architectural Rules:**
- **Encapsulation & RAII**: Database handles (`DatabaseConnection`), prepared statements (`PreparedStatement`), and transactions (`TransactionGuard`) are RAII-managed.
- **SOLID Design**: Repositories isolate SQL queries; Services enforce business permissions and data integrity; Interfaces remain independent.
- **Parametric Statements**: All SQL queries use parameterized bindings to prevent SQL injection.
- **Foreign Key Enforcement**: `PRAGMA foreign_keys = ON` is enforced with atomic database transactions.

---

## Technology Stack

| Component | Technology | Description |
|---|---|---|
| Language | C++17 | Modern, type-safe C++ using smart pointers, RAII, and `std::optional` |
| Build System | CMake 3.16+ / Ninja / Make | Cross-platform build generator |
| Database | SQLite 3.46.1 | Embedded amalgamation (zero external database setup required) |
| Web Server | Native C++17 HTTP Server | Multi-threaded Winsock/POSIX socket server |
| Web UI/UX | Vanilla HTML5 / Modern CSS / JS | Zero-dependency, lightweight, responsive clinical design system |
| Charts | Chart.js | Interactive donut & bar charts |
| Password Security | Custom Salted SHA-256 | 5,000-round salted iterative hashing (no plaintext passwords) |
| Test Framework | Custom Unit Test Runner | Lightweight test harness with assertion macros |

---

## Prerequisites & Build Instructions

### Prerequisites
Any standard modern C++17 compiler:
- **Windows**: MinGW-w64 (GCC 10+) or MSVC (Visual Studio 2019+)
- **Linux / macOS**: GCC 10+ or Clang 11+
- **CMake**: Version 3.16 or newer

*(Note: No heavy GUI frameworks like Qt are required! MediFind compiles in seconds using any standard C++ compiler.)*

### Build Commands

```powershell
# 1. Clone repository
git clone <repo_url>
cd Medi-Inventory-Management

# 2. Configure build with CMake
cmake -S . -B build -G "Ninja"

# 3. Compile all targets
cmake --build build
```

### Generated Executable Targets

| Binary | Path | Description |
|---|---|---|
| `medifind_server` | `build/medifind_server.exe` | **Native C++ Web Server** (serves web portal on `http://localhost:8080` & auto-opens browser) |
| `medifind_console` | `build/medifind_console.exe` | **Interactive Console CLI** for terminal operation and viva |
| `medifind_tests` | `build/medifind_tests.exe` | **Automated Unit Test Suite** (11 test suites) |
| Web Portal | `web/index.html` | Pure client-side SPA dashboard (can also run standalone) |

---

## Running the Application

### 1. Launching the Web Portal via C++ Server (Recommended)

Run the native C++ server:
```powershell
cd build
.\medifind_server.exe
```
Output:
```
=======================================================
       MEDIFIND CLINICAL WEB SERVER (C++17)            
=======================================================
 [+] Server active on: http://localhost:8080
 [+] Serving Web Application from: web
 [+] Launching Web UI in your default browser...
=======================================================
```
*Your browser will open automatically to `http://localhost:8080`.*

### 2. Standalone Browser Launch (Zero Installation)
You can also directly open `web/index.html` in any browser:
```
file:///C:/path/to/Medi-Inventory-Management/web/index.html
```

### 3. Launching the Console Application
```powershell
cd build
.\medifind_console.exe
```

---

## User Roles and Demo Credentials

The database is seeded on first launch with two demo accounts:

| Username | Password | Role | Permissions |
|---|---|---|---|
| `admin` | `Admin@123` | Administrator | Full access: stock-in, stock adjustments, purchase orders, approval of restocks, user management, audit review |
| `employee1` | `Staff@123` | Employee / Staff | Medicine search, batch viewing, POS dispensing, returns, and submitting restock requests |

---

## FEFO & Expiry Protection Engine

**FEFO (First Expired, First Out)** is the regulatory gold standard for pharmaceutical inventory. Unlike standard FIFO (First In, First Out), FEFO ensures batches with the earliest expiration date are dispensed first, preventing expired medicine from reaching patients.

### How the Engine Works:
1. **Batch Filtering**: Filters out batches where `quantity == 0`, `status == 'EXPIRED'`, or `expiry_date <= currentDate`.
2. **Deterministic Sorting**: Orders batches by `expiry_date ASC, id ASC`.
3. **Multi-Batch Allocation**: Allocates from the nearest-expiry batch. If the requested quantity exceeds that batch, it automatically spills over to subsequent valid batches.
4. **Atomic Decrement**: Updates inventory and creates immutable stock movement records atomically inside an SQLite transaction.

---

## DSA Implementations & Diagnostics

All data structures in MediFind are **custom-implemented** from scratch (no reliance on `std::map`, `std::unordered_map`, or STL containers for DSA demonstrations):

| Structure | File | Complexity | Purpose in MediFind |
|---|---|---|---|
| **Hash Table** (Separate Chaining) | `include/dsa/HashTable.hpp` | Search/Insert: $O(1)$ avg | High-speed cache for medicine lookups by integer ID |
| **Binary Search Tree** (BST) | `include/dsa/BST.hpp` | Insert: $O(\log n)$, Search: $O(n)$ prefix | Alphabetical in-order prefix autocomplete for medicine names |
| **Doubly Linked List** | `include/dsa/LinkedList.hpp` | Insert/Delete ends: $O(1)$ | Bidirectional ledger traversal for audit logs |
| **Queue** (FIFO) | `include/dsa/Queue.hpp` | Enqueue/Dequeue: $O(1)$ | Orderly processing queue for floor restock requests |
| **Stack** (LIFO) | `include/dsa/Stack.hpp` | Push/Pop: $O(1)$ | Operation history buffer for review and undo operations |
| **MergeSort** | `include/dsa/Sorting.hpp` | Time: $O(n \log n)$, Space: $O(n)$ | Stable sorting of batches by expiration dates (FEFO order) |
| **QuickSort** | `include/dsa/Sorting.hpp` | Time: $O(n \log n)$ avg, in-place | Fast in-place sorting of batch lists by price |
| **Binary Search** | `include/dsa/Sorting.hpp` | Search: $O(\log n)$ | Fast lookup on sorted ID and price arrays |

---

## Database Schema

Authoritative SQLite schema with 10 relational tables:
- `users`: ID, username, password_hash, salt, full_name, role, is_active
- `medicines`: ID, name, brand, strength, dosage_form, category, barcode, min_stock_threshold
- `batches`: ID, medicine_id, batch_number, mfg_date, expiry_date, quantity, purchase_cost, selling_price, status
- `suppliers`: ID, name, contact_person, email, phone, address
- `purchase_orders` & `purchase_order_items`: PO tracking, unit cost, quantities
- `sales` & `sale_items`: Invoices, line items, timestamps, payment methods
- `returns`: Return reference, refund amounts, restock flags
- `stock_movements`: Complete ledger trail of all stock additions, deductions, and adjustments
- `restock_requests`: Staff requests, approval notes, status
- `audit_logs`: User, action, entity type, entity ID, change summary

---

## Running Automated Tests

Run the full automated test suite verifying DSA, FEFO, Auth, Inventory, Sales, and Backups:

```powershell
cd build
.\medifind_tests.exe
```

Expected result:
```
=======================================================
        MEDIFIND AUTOMATED TEST SUITE RUNNER           
=======================================================
[ RUN      ] BackupSuite::CreateAndRestoreLiveBackup -> [ PASS ]
[ RUN      ] SalesSuite::CompleteSaleAndReturnWorkflow -> [ PASS ]
[ RUN      ] InventorySuite::MedicineAndStockOperations -> [ PASS ]
[ RUN      ] FefoSuite::EarliestExpiryPriorityAndSkippingExpired -> [ PASS ]
[ RUN      ] AuthSuite::UserCreationAndAuthentication -> [ PASS ]
[ RUN      ] DsaSuite::HashTableBasicAndRehash -> [ PASS ]
[ RUN      ] DsaSuite::LinkedListOperations -> [ PASS ]
[ RUN      ] DsaSuite::BinarySearchTreeOrderingAndPrefix -> [ PASS ]
[ RUN      ] DsaSuite::QueueFifoOrder -> [ PASS ]
[ RUN      ] DsaSuite::StackLifoOrder -> [ PASS ]
[ RUN      ] DsaSuite::SortingMergeAndQuickSort -> [ PASS ]
=======================================================
TEST RESULTS: 11 passed, 0 failed.
=======================================================
```

---

## Reports & Financial Valuations

- **Inventory Valuation**: Generates exact purchase-cost asset book value ($9,165.00) vs projected retail turnover ($15,730.00).
- **Expiry Risk Report**: Detects and highlights batches expired or near expiry (&le; 90 days).
- **CSV Data Exports**: One-click export for Medicines, Batches, Sales Movements, and Audit Ledger.

---

## Viva Q&A

**Q: Why FEFO instead of FIFO for pharmaceuticals?**
> FIFO dispatches items based on when they arrived in the warehouse. FEFO dispatches items based on expiration date. A recently arrived shipment may have an earlier expiration date than older stock. FEFO prevents stock obsolescence and ensures patient safety.

**Q: How does the web interface connect to C++?**
> The C++ application includes an embedded multi-threaded HTTP server (`server/WebServer.cpp`) that serves the static web assets (`web/`) and provides REST API endpoints (`/api/dashboard`, `/api/medicines`, etc.) backed directly by SQLite.

**Q: How are passwords secured?**
> A unique 32-byte salt is generated per user. The salt and plaintext password are iteratively hashed 5,000 times using SHA-256. Plaintext passwords are never stored.

**Q: What is the purpose of the custom DSA implementations?**
> In high-throughput clinical operations, the `HashTable` provides $O(1)$ in-memory lookups for medicine master records without querying disk repeatedly. The `BST` enables prefix autocomplete, and `MergeSort` provides guaranteed $O(n \log n)$ stable sorting for FEFO date ordering.
