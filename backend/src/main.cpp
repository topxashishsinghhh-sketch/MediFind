#include "crow.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "auth_service.h"
#include "user_service.h"
#include "repositories/user_repository.h"
#include "product_service.h"
#include "medicine_service.h"
#include "batch_service.h"
#include "inventory_service.h"
#include "repositories/product_repository.h"
#include "repositories/order_repository.h"
#include "order_service.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

inline crow::response ok_response(const json& body) {
    return crow::response(200, body.dump());
}

inline crow::response created_response(const json& body) {
    return crow::response(201, body.dump());
}

inline crow::response bad_request(const std::string& message) {
    json body;
    body["error"] = message;
    return crow::response(400, body.dump());
}

inline crow::response not_found(const std::string& message) {
    json body;
    body["error"] = message;
    return crow::response(404, body.dump());
}

inline crow::response server_error(const std::string& message) {
    json body;
    body["error"] = message;
    return crow::response(500, body.dump());
}

void setupAuthRoutes(crow::SimpleApp& app, AuthService& authService, UserService& userService);
void setupProductRoutes(crow::SimpleApp& app, ProductService& productService,
                        MedicineService& medicineService, BatchService& batchService,
                        InventoryService& inventoryService);
void setupOrderRoutes(crow::SimpleApp& app, OrderService& orderService);

static std::string currentTimeIso() {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&timeT), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// Seed sample products and batches
void seedSampleData(ProductService& productService, BatchService& batchService) {
    // Only seed if no products exist yet
    // We check by trying to create - but simpler: check if data files are empty
    // For now, always seed (createProduct handles duplicates via existsByNameAndBrand)

    // Medicines
    int p1 = productService.createProduct(
        "Paracetamol 500mg", "MediCare", ProductCategory::Medicine,
        "MediCare Pharmaceuticals", "500mg", "Tablet", false,
        "Pain relief and fever reduction medication.", "https://via.placeholder.com/150?text=Paracetamol");
    int p2 = productService.createProduct(
        "Amoxicillin 250mg", "PharmaCo", ProductCategory::Medicine,
        "PharmaCo Labs", "250mg", "Capsule", true,
        "Antibiotic for bacterial infections. Requires prescription.", "https://via.placeholder.com/150?text=Amoxicillin");
    int p3 = productService.createProduct(
        "Omeprazole 20mg", "GutHealth", ProductCategory::Medicine,
        "GutHealth Pharma", "20mg", "Tablet", false,
        "Proton pump inhibitor for acid reflux.", "https://via.placeholder.com/150?text=Omeprazole");
    int p4 = productService.createProduct(
        "Ibuprofen 400mg", "PainAway", ProductCategory::Medicine,
        "PainAway Inc", "400mg", "Tablet", false,
        "Non-steroidal anti-inflammatory drug for pain relief.", "https://via.placeholder.com/150?text=Ibuprofen");
    int p5 = productService.createProduct(
        "Cetirizine 10mg", "AllerFree", ProductCategory::Medicine,
        "AllerFree Labs", "10mg", "Tablet", false,
        "Antihistamine for allergy relief.", "https://via.placeholder.com/150?text=Cetirizine");

    // Healthcare products
    int p6 = productService.createProduct(
        "Vitamin C 1000mg", "HealthPlus", ProductCategory::Healthcare,
        "HealthPlus Nutrition", "", "", false,
        "Immune support supplement.", "https://via.placeholder.com/150?text=Vitamin+C");
    int p7 = productService.createProduct(
        "Digital Blood Pressure Monitor", "MediTech", ProductCategory::Healthcare,
        "MediTech Devices", "", "", false,
        "Automatic digital blood pressure monitor.", "https://via.placeholder.com/150?text=BP+Monitor");
    int p8 = productService.createProduct(
        "Multivitamin Tablets", "DailyWell", ProductCategory::Healthcare,
        "DailyWell Health", "", "", false,
        "Daily multivitamin supplement.", "https://via.placeholder.com/150?text=Multivitamin");
    int p9 = productService.createProduct(
        "First Aid Kit", "SafeCare", ProductCategory::Healthcare,
        "SafeCare Supplies", "", "", false,
        "Comprehensive first aid kit for home and travel.", "https://via.placeholder.com/150?text=First+Aid");
    int p10 = productService.createProduct(
        "Digital Thermometer", "MediTech", ProductCategory::Healthcare,
        "MediTech Devices", "", "", false,
        "Digital infrared thermometer.", "https://via.placeholder.com/150?text=Thermometer");

    // Batches for Paracetamol (p1) - FEFO test data
    batchService.createBatch(p1, "2025-01-01", "2027-06-01", 2.00, 4.99, 150, 101);
    batchService.createBatch(p1, "2025-03-01", "2027-12-01", 2.10, 4.99, 200, 102);

    // Batches for Amoxicillin (p2) - prescription required
    batchService.createBatch(p2, "2025-02-01", "2027-03-15", 10.00, 18.99, 80, 103);
    batchService.createBatch(p2, "2025-04-01", "2028-01-01", 10.50, 18.99, 120, 104);

    // Batches for Omeprazole (p3)
    batchService.createBatch(p3, "2025-01-15", "2027-09-01", 5.00, 14.50, 100, 105);
    batchService.createBatch(p3, "2025-05-01", "2028-06-01", 5.20, 14.50, 90, 106);

    // Batches for Ibuprofen (p4) - one low stock
    batchService.createBatch(p4, "2025-02-15", "2027-08-01", 3.00, 7.99, 5, 107);  // low stock
    batchService.createBatch(p4, "2025-06-01", "2028-03-01", 3.10, 7.99, 200, 108);

    // Batches for Cetirizine (p5) - one near expiry
    batchService.createBatch(p5, "2025-01-01", "2026-12-01", 2.50, 6.25, 60, 109);  // near expiry
    batchService.createBatch(p5, "2025-07-01", "2028-09-01", 2.60, 6.25, 100, 110);

    // Batches for healthcare products
    batchService.createBatch(p6, "2025-03-01", "2028-03-01", 6.00, 12.50, 200, 111);
    batchService.createBatch(p7, "2025-04-01", "2029-04-01", 30.00, 45.00, 50, 112);
    batchService.createBatch(p8, "2025-05-01", "2028-05-01", 8.00, 15.99, 150, 113);
    batchService.createBatch(p9, "2025-06-01", "2029-06-01", 15.00, 25.00, 80, 114);
    batchService.createBatch(p10, "2025-07-01", "2028-07-01", 5.00, 12.00, 120, 115);

    std::cout << "[seed] Sample products and batches created\n";
}

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    std::cout << "[debug] Starting MediFind backend...\n";

    fs::create_directories("data");
    std::cout << "[debug] Data directory ready\n";

    // Repositories
    UserRepository userRepo("data/users.csv");
    ProductRepository productRepo("data/products.csv");
    BatchRepository batchRepo("data/batches.csv");
    OrderRepository orderRepo("data/orders.csv");
    OrderItemRepository itemRepo("data/order_items.csv");
    std::cout << "[debug] Repositories initialized\n";

    // Services
    AuthService authService(userRepo);
    UserService userService(userRepo);
    ProductService productService(productRepo);
    MedicineService medicineService(productRepo);
    BatchService batchService(batchRepo);
    InventoryService inventoryService(productRepo, batchRepo);
    OrderService orderService(orderRepo, itemRepo, batchRepo, productRepo, inventoryService);
    std::cout << "[debug] Services initialized\n";

    // Seed default users
    if (!userRepo.emailExists("admin@medifind.demo")) {
        authService.registerUser("admin@medifind.demo", "Admin123!", "System Administrator",
                                 "+1 000 000 0000", UserRole::Admin);
    }
    if (!userRepo.emailExists("employee@medifind.demo")) {
        authService.registerUser("employee@medifind.demo", "Emp123!", "Pharmacy Employee",
                                 "+1 000 000 0001", UserRole::Employee);
    }
    if (!userRepo.emailExists("customer@medifind.demo")) {
        authService.registerUser("customer@medifind.demo", "Cust123!", "Demo Customer",
                                 "+1 000 000 0002", UserRole::Customer);
    }
    std::cout << "[debug] Default users seeded\n";

    // Seed sample products/batches (only if products file is empty)
    auto existing = productRepo.findAll();
    if (existing.empty()) {
        seedSampleData(productService, batchService);
    }
    std::cout << "[debug] Sample data seeded\n";

    crow::SimpleApp app;

    // Health check
    CROW_ROUTE(app, "/api/health").methods(crow::HTTPMethod::Get)([]() {
        json body;
        body["status"] = "ok";
        body["service"] = "medifind-backend";
        body["version"] = "1.0.0";
        return ok_response(body);
    });

    // Root
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::Get)([]() {
        json body;
        body["message"] = "MediFind Backend API";
        body["version"] = "1.0.0";
        return ok_response(body);
    });

    // Auth routes
    setupAuthRoutes(app, authService, userService);

    // Product, batch, inventory routes
    setupProductRoutes(app, productService, medicineService, batchService, inventoryService);

    // Order routes
    setupOrderRoutes(app, orderService);

    std::cout << "MediFind backend starting on http://0.0.0.0:8080\n";
    std::cout << "Demo accounts:\n";
    std::cout << "  admin:    admin@medifind.demo / Admin123!\n";
    std::cout << "  employee: employee@medifind.demo / Emp123!\n";
    std::cout << "  customer: customer@medifind.demo / Cust123!\n";

    try {
        app.port(8080).multithreaded().run();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Crow run() exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[FATAL] Unknown exception in Crow run()\n";
        return 1;
    }

    return 0;
}
