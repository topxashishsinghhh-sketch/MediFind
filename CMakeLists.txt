cmake_minimum_required(VERSION 3.16)
project(MediFind VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable strict warnings
if(MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3
)

# 1. SQLite3 Static Library
add_library(sqlite3_lib STATIC
    external/sqlite3/sqlite3.c
)
target_include_directories(sqlite3_lib PUBLIC external/sqlite3)
# Enable foreign keys and recommended SQLite compiler optimizations
target_compile_definitions(sqlite3_lib PRIVATE
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_RTREE
    SQLITE_ENABLE_DBSTAT_VTAB
    SQLITE_THREADSAFE=1
)

# 2. MediFind Core Library
set(CORE_SOURCES
    src/utils/CryptoUtils.cpp
    src/utils/DateUtils.cpp
    src/utils/CsvExporter.cpp
    src/database/DatabaseConnection.cpp
    src/database/SchemaMigrations.cpp
    src/database/DatabaseBackup.cpp
    src/repositories/UserRepository.cpp
    src/repositories/MedicineRepository.cpp
    src/repositories/BatchRepository.cpp
    src/repositories/SupplierRepository.cpp
    src/repositories/PurchaseOrderRepository.cpp
    src/repositories/SaleRepository.cpp
    src/repositories/ReturnRepository.cpp
    src/repositories/StockMovementRepository.cpp
    src/repositories/RestockRequestRepository.cpp
    src/repositories/AuditLogRepository.cpp
    src/services/AuthService.cpp
    src/services/FefoAllocator.cpp
    src/services/InventoryService.cpp
    src/services/PurchasingService.cpp
    src/services/SalesService.cpp
    src/services/RestockService.cpp
    src/services/AuditService.cpp
    src/services/ReportService.cpp
    src/seed/SeedData.cpp
)

add_library(medifind_core STATIC ${CORE_SOURCES})
target_link_libraries(medifind_core PUBLIC sqlite3_lib)
target_include_directories(medifind_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 3. Console Application Executable
add_executable(medifind_console
    console/ConsoleApp.cpp
    console/main_console.cpp
)
target_link_libraries(medifind_console PRIVATE medifind_core)

# 4. Automated Tests Executable
add_executable(medifind_tests
    tests/test_dsa.cpp
    tests/test_auth.cpp
    tests/test_fefo.cpp
    tests/test_inventory.cpp
    tests/test_sales.cpp
    tests/test_backup.cpp
    tests/main_tests.cpp
)
target_link_libraries(medifind_tests PRIVATE medifind_core)

# 5. C++ Web Server Application (Serves modern Web UI on http://localhost:8080)
add_executable(medifind_server
    server/WebServer.cpp
    server/main_server.cpp
)
target_link_libraries(medifind_server PRIVATE medifind_core)
if(WIN32)
    target_link_libraries(medifind_server PRIVATE ws2_32)
endif()

# Enable CTest
enable_testing()
add_test(NAME MediFindTestSuite COMMAND medifind_tests)
