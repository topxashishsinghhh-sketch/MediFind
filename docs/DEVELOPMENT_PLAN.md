# MediFind Development Plan

## Phase 1 — Project Structure, Architecture, Design System (COMPLETED)

### Completed
- [x] Repository directory structure created (`backend/`, `frontend/`, `data/`, `docs/`)
- [x] Backend CMake project configured with Crow + nlohmann-json + Asio (vendored headers)
- [x] Backend compiles with MinGW-w64 GCC 13.1 (`-lws2_32 -lmswsock` required on Windows)
- [x] Health check endpoint `/api/health` and root `/` responding with JSON
- [x] Frontend scaffolded with Vite + React 19 + TypeScript + Tailwind CSS v4 + React Router v7
- [x] Frontend dev server running at `http://localhost:5173`
- [x] Design system colors defined: Navy `#102A43`, Teal `#13A89E`, Soft Grey `#F3F7FA`, White `#FFFFFF`
- [x] README.md created

### Blockers Resolved
- **Node.js missing**: Installed v26.12.1 via winget
- **C++ compiler**: g++ 6.3 (MinGW.org) lacks C++17; using MinGW-w64 GCC 13.1 from Qt installation
- **Crow dependency**: vcpkg builds failing; cloned crow + asio repos directly into `backend/include/vendor/`
- **Winsock linking**: Added `-lws2_32 -lmswsock` for Windows socket support

## Phase 2 — Customer Frontend with Sample Data

### Planned
- [ ] Homepage with MediFind branding, search bar, categories, promotional banners
- [ ] Login and registration pages
- [ ] Medicines catalogue and healthcare catalogue
- [ ] Product details page
- [ ] Search results page
- [ ] Shopping cart with add/remove/update
- [ ] Checkout flow (simulated payment)
- [ ] Order confirmation
- [ ] My orders and order tracking
- [ ] Prescription management (upload)
- [ ] Customer profile and address management
- [ ] Support and FAQs

### Sample Data
Fictional products, batches, customers, and orders will be seeded in CSV files.

## Phase 3 — Backend Foundation

### Planned
- [ ] AuthService: registration, login, password hashing, JWT/session tokens
- [ ] UserService: CRUD for users, role-based access control
- [ ] CSV Repository layer with quoted field support
- [ ] Middleware for authentication and authorization on all restricted routes
- [ ] Consistent JSON response format and HTTP status codes

## Phase 4 — Products, Batches, Inventory, DSA

### Planned
- [ ] ProductService: medicine and healthcare product management
- [ ] MedicineService: prescription requirement, strength, dosage form
- [ ] BatchService: batch CRUD, expiry dates, pricing
- [ ] InventoryService: available stock calculation (physical − reserved − blocked)
- [ ] Custom Hash Table: medicine ID/barcode lookup (O(1) average)
- [ ] Custom BST: batch expiry-date indexing with in-order FEFO traversal
- [ ] Custom Min Heap: urgent expiry and stock alerts (O(log n) insert/remove)
- [ ] FEFO batch allocation algorithm
- [ ] Merge sort for reports, binary search on sorted datasets

## Phase 5 — Cart, Checkout, Orders, Prescriptions, Payments

### Planned
- [ ] CartService: cart CRUD, cart items
- [ ] OrderService: order creation, status tracking, cancellation, returns
- [ ] Batch-level stock reservation with release on failure/cancellation
- [ ] PaymentService: simulated success/failure (clearly labeled as demo)
- [ ] PrescriptionService: file upload, review queue, statuses (pending/approved/rejected/clarification)
- [ ] Order status flow: pending → paid → processing → packed → dispatched → delivered

## Phase 6 — Admin & Employee Dashboards

### Admin
- [ ] Dashboard with analytics cards
- [ ] Products, batches, inventory management
- [ ] Orders, prescriptions, customers, suppliers management
- [ ] Purchases, approvals, employees management
- [ ] Notifications, reports, audit logs, backup/restore

### Employee
- [ ] Dashboard, product search, inventory view
- [ ] Assigned orders, order processing
- [ ] Prescription tasks, expiry alerts, restock requests

## Phase 7 — Testing, Security, Documentation

### Planned
- [ ] Google Test unit tests for DSA (hash table collisions, BST traversal, min heap ordering)
- [ ] Invalid product/batch data validation tests
- [ ] Authentication and permission tests
- [ ] Expired-batch sale prevention tests
- [ ] FEFO allocation tests
- [ ] Insufficient inventory, stock reservation/release tests
- [ ] Simulated payment success/failure tests
- [ ] CSV parsing and file failure tests
- [ ] Application restart and data recovery tests
- [ ] Security review: password hashing, RBAC, data isolation, audit logs
- [ ] Full integration journey test: admin creates product → customer orders → employee processes → admin reports

## Immediate Next Step

Begin Phase 2: Implement customer frontend pages with fictional sample data, connecting to the backend API endpoints as they are implemented in subsequent phases.