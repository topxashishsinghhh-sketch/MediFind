# MediFind — Online Pharmacy & Healthcare Platform

A modern, full-stack online pharmacy and healthcare e-commerce platform inspired by PharmEasy.

## Architecture

```
React Frontend (TypeScript + Tailwind)
    ↓ REST API (JSON)
C++17 Backend (Crow framework)
    ↓ Services → Repositories
CSV/Text Files (with transaction journal, backups)
```

## Technology Stack

| Layer | Technology |
|---|---|
| Frontend | React 19, TypeScript, Tailwind CSS v4, React Router v7 |
| Backend | C++17, Crow (REST API), nlohmann/json, CMake |
| Storage | CSV/text files (SQLite-migration-ready repository interfaces) |
| Testing | Google Test (backend), frontend + API integration tests |
| Build | CMake (backend), Vite (frontend) |

## Directory Structure

```
mediFind/
├── backend/              # C++17 Crow backend
│   ├── bin/              # Compiled executables
│   ├── build/            # CMake build output
│   ├── data/             # CSV data files
│   ├── include/          # Header files
│   │   └── vendor/       # Third-party headers (crow, asio, nlohmann-json)
│   ├── src/              # Source files
│   │   ├── controllers/  # HTTP request handlers
│   │   ├── dsa/          # Custom DSA (hash table, BST, min heap)
│   │   ├── models/       # Data models
│   │   ├── repositories/ # CSV persistence layer
│   │   └── services/     # Business logic
│   └── tests/            # Google Test unit tests
│   └── CMakeLists.txt
├── frontend/             # React frontend
│   ├── src/
│   │   ├── components/   # Reusable UI components
│   │   ├── pages/        # Page components
│   │   ├── hooks/        # Custom React hooks
│   │   ├── services/     # API client services
│   │   ├── context/      # React context providers
│   │   ├── types/        # TypeScript type definitions
│   │   └── assets/       # Static assets
│   ├── postcss.config.js
│   ├── vite.config.ts
│   └── package.json
├── data/                 # Shared data directory
├── docs/                 # Documentation
└── README.md
```

## User Roles

### Customer
- Registration/login, browse medicines, search/filter, cart, checkout
- Prescription upload, address management, order history, tracking

### Employee
- Login, dashboard, product search, inventory, order processing
- Prescription review, expiry alerts, restock requests

### Admin
- Dashboard, analytics, employee/product/batch management
- Inventory control, purchase approvals, reports, audit logs, backup

## Key Features

- **Role-based access control** enforced in backend services
- **Custom DSA implementations**: Hash Table (O(1) lookup), BST (expiry indexing), Min Heap (alerts)
- **FEFO inventory** (First Expired, First Out) batch allocation
- **Prescription workflow** with review queue (pending/approved/rejected)
- **Simulated payment** (demo only, no real card data)
- **Stock reservation** with release on failure/cancellation
- **CSV persistence** with transaction journal and recovery
- **Audit logs** for sensitive operations

## Development Phases

| Phase | Focus |
|---|---|
| 1 | Project structure, CMake backend, Vite frontend, design system |
| 2 | Customer frontend with sample data |
| 3 | Backend foundation: models, repositories, authentication |
| 4 | Products, batches, inventory, DSA, FEFO, expiry |
| 5 | Cart, checkout, orders, prescriptions, payments, tracking |
| 6 | Admin/employee dashboards, suppliers, reports, notifications |
| 7 | Testing, security review, bug fixes, documentation |

## Backend Build (Windows + MSVC or MinGW)

### Using MinGW-w64 (GCC 13.1+)

```bash
# Set PATH to include MinGW bin
set PATH=C:\Qt\Tools\mingw1310_64\bin;%PATH%

# Compile
g++ -std=c++17 -O2 -Iinclude -Iinclude/vendor/crow/include ^
  -Iinclude/vendor/asio/include -Iinclude/vendor -Isrc ^
  src/main.cpp src/controllers/*.cpp src/services/*.cpp ^
  src/repositories/*.cpp src/models/*.cpp src/dsa/*.cpp ^
  -o bin/medifind-server.exe -lws2_32 -lmswsock

# Run
bin\medifind-server.exe
# Test: curl http://localhost:8080/api/health
```

### Dependencies (vendored in `backend/include/vendor/`)

- `crow/` — Crow HTTP framework (cloned from github.com/crowcpp/crow)
- `asio/` — Standalone Asio (cloned from github.com/chriskohlhoff/asio)
- `nlohmann/json.hpp` — JSON library

## Frontend Build

```bash
cd frontend
npm install
npm run dev      # Start dev server at http://localhost:5173
npm run build    # Production build
```

## Sample Data

Fictional demonstration data is used throughout. MediFind is not a licensed pharmacy and is not a substitute for professional medical advice. Simulated payments are clearly marked as demonstrations.

## License

Portfolio project — educational use only.