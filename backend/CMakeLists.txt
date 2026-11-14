cmake_minimum_required(VERSION 3.20)
project(MediFindBackend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(include include/vendor include/vendor/crow/include include/vendor/asio/include include/vendor src)

# Source files
set(SERVER_SOURCES
    src/main.cpp
    src/controllers/auth_controller.cpp
    src/controllers/product_controller.cpp
    src/controllers/order_controller.cpp
    src/services/auth_service.cpp
    src/services/product_service.cpp
    src/services/order_service.cpp
    src/services/user_service.cpp
    src/services/batch_service.cpp
    src/services/inventory_service.cpp
    src/services/expiry_service.cpp
    src/repositories/csv_repository.cpp
    src/repositories/product_repository.cpp
    src/models/user.cpp
    src/models/product.cpp
    src/dsa/hash_table.cpp
    src/dsa/bst.cpp
    src/dsa/min_heap.cpp
)

add_executable(medifind-server ${SERVER_SOURCES})

# Test executable
set(TEST_SOURCES
    tests/test_hash_table.cpp
    tests/test_bst.cpp
    tests/test_min_heap.cpp
    tests/test_repository.cpp
    tests/test_services.cpp
)

add_executable(medifind-tests ${TEST_SOURCES})

enable_testing()
add_test(NAME medifind-tests COMMAND medifind-tests)
