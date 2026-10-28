#include "seed/SeedData.hpp"
#include "utils/DateUtils.hpp"

namespace medifind::seed {

void SeedData::seedDemoData(database::DatabaseConnection& /*db*/,
                             services::AuthService& authService,
                             services::InventoryService& invService,
                             services::PurchasingService& purchService,
                             services::SalesService& salesService) {
    // 1. Seed Users if not present
    if (!authService.bootstrapAdminIfEmpty("admin", "Admin@123", "Chief Administrator")) {
        return; // Already seeded
    }

    // Login as admin for seed actor
    authService.login("admin", "Admin@123", "BOOTSTRAP");
    int adminId = authService.getCurrentUser()->id;

    // Create demo employee
    auto empRes = authService.registerUser("employee1", "Staff@123", "Sarah Jenkins", models::UserRole::EMPLOYEE);
    int empId = empRes.user ? empRes.user->id : adminId;

    // 2. Seed Suppliers
    std::string err;
    models::Supplier s1{0, "Pfizer Healthcare Logistics", "Dr. Robert Vance", "orders@pfizer-logistics.test", "+1-800-555-0101", "770 Innovation Way, Cambridge, MA", true, ""};
    models::Supplier s2{0, "Novartis Global Distribution", "Elena Rostova", "supply@novartis-dist.test", "+1-800-555-0102", "120 Pharma Park, Basel / NJ", true, ""};
    models::Supplier s3{0, "Cipla Care Supplies", "Amitabh Sen", "sales@ciplacare.test", "+1-800-555-0103", "45 Industrial Boulevard, Mumbai", true, ""};

    int sup1 = purchService.addSupplier(s1, adminId, err);
    int sup2 = purchService.addSupplier(s2, adminId, err);
    int sup3 = purchService.addSupplier(s3, adminId, err);

    // 3. Seed Medicines
    models::Medicine m1{0, "Paracetamol 500mg", "Tylenol/GSK", "500 mg", "Tablet", "Analgesics", "8901001001", "Pain reliever and fever reducer", 20, true, ""};
    models::Medicine m2{0, "Amoxicillin 250mg", "Amoxil", "250 mg", "Capsule", "Antibiotics", "8901001002", "Broad-spectrum penicillin antibiotic", 15, true, ""};
    models::Medicine m3{0, "Cetirizine 10mg", "Zyrtec", "10 mg", "Tablet", "Antihistamines", "8901001003", "Second-generation allergy relief", 10, true, ""};
    models::Medicine m4{0, "Metformin 500mg", "Glucophage", "500 mg", "Tablet", "Antidiabetic", "8901001004", "First-line Type 2 diabetes medication", 25, true, ""};
    models::Medicine m5{0, "Atorvastatin 20mg", "Lipitor", "20 mg", "Tablet", "Cardiovascular", "8901001005", "Statin medication for cholesterol reduction", 15, true, ""};
    models::Medicine m6{0, "Omeprazole 20mg", "Prilosec", "20 mg", "Capsule", "Gastrointestinal", "8901001006", "Proton-pump inhibitor for acid reflux", 12, true, ""};
    models::Medicine m7{0, "Azithromycin 500mg", "Zithromax", "500 mg", "Tablet", "Antibiotics", "8901001007", "Macrolide antibiotic 3-day treatment", 10, true, ""};
    models::Medicine m8{0, "Ibuprofen 400mg", "Advil", "400 mg", "Tablet", "Analgesics", "8901001008", "Nonsteroidal anti-inflammatory drug (NSAID)", 20, true, ""};
    models::Medicine m9{0, "Salbutamol Inhaler", "Ventolin", "100 mcg", "Inhaler", "Respiratory", "8901001009", "Fast-acting bronchodilator for asthma", 8, true, ""};
    models::Medicine m10{0, "Insulin Glargine", "Lantus", "100 IU/ml", "Injection", "Antidiabetic", "8901001010", "Long-acting insulin (keep refrigerated 2-8C)", 5, true, ""};

    int med1 = invService.addMedicine(m1, adminId, err);
    int med2 = invService.addMedicine(m2, adminId, err);
    int med3 = invService.addMedicine(m3, adminId, err);
    int med4 = invService.addMedicine(m4, adminId, err);
    int med5 = invService.addMedicine(m5, adminId, err);
    int med6 = invService.addMedicine(m6, adminId, err);
    int med7 = invService.addMedicine(m7, adminId, err);
    int med8 = invService.addMedicine(m8, adminId, err);
    int med9 = invService.addMedicine(m9, adminId, err);
    int med10 = invService.addMedicine(m10, adminId, err);

    // 4. Seed Batches demonstrating FEFO
    // Paracetamol:
    // Batch A: Expiring soon (2026-11-15), 30 units -> FEFO #1
    // Batch B: Fresh (2027-08-20), 100 units -> FEFO #2
    // Batch C: Expired (2026-05-10), 15 units -> Should NOT be sold!
    models::Batch b1A{0, med1, "PAR-24-001", "2024-11-15", "2026-11-15", 30, 1.20, 2.50, sup1, "Shelf A-1", models::BatchStatus::AVAILABLE, ""};
    models::Batch b1B{0, med1, "PAR-25-002", "2025-08-20", "2027-08-20", 100, 1.10, 2.50, sup1, "Shelf A-1", models::BatchStatus::AVAILABLE, ""};
    models::Batch b1C{0, med1, "PAR-23-OLD", "2023-05-10", "2026-05-10", 15, 1.00, 2.50, sup1, "Quarantine Zone", models::BatchStatus::EXPIRED, ""};

    // Amoxicillin:
    // Batch A: (2026-12-01), 25 units
    // Batch B: (2027-04-10), 50 units
    models::Batch b2A{0, med2, "AMX-24-01", "2024-12-01", "2026-12-01", 25, 4.50, 8.00, sup2, "Shelf B-2", models::BatchStatus::AVAILABLE, ""};
    models::Batch b2B{0, med2, "AMX-25-02", "2025-04-10", "2027-04-10", 50, 4.20, 8.00, sup2, "Shelf B-2", models::BatchStatus::AVAILABLE, ""};

    // Cetirizine: Low stock demo! Total stock = 5 (Threshold is 10)
    models::Batch b3{0, med3, "CET-25-01", "2025-01-10", "2027-06-30", 5, 0.80, 2.00, sup3, "Shelf C-1", models::BatchStatus::AVAILABLE, ""};

    // Metformin:
    models::Batch b4{0, med4, "MET-25-01", "2025-02-15", "2027-10-15", 80, 2.10, 4.50, sup3, "Shelf D-3", models::BatchStatus::AVAILABLE, ""};

    // Atorvastatin:
    models::Batch b5{0, med5, "ATV-25-01", "2025-03-01", "2027-09-01", 45, 6.00, 12.50, sup1, "Shelf E-2", models::BatchStatus::AVAILABLE, ""};

    // Omeprazole:
    models::Batch b6{0, med6, "OMP-25-01", "2025-01-20", "2027-07-20", 60, 3.20, 7.00, sup2, "Shelf F-1", models::BatchStatus::AVAILABLE, ""};

    // Azithromycin:
    models::Batch b7{0, med7, "AZI-25-01", "2025-02-10", "2027-05-10", 35, 7.50, 15.00, sup2, "Shelf B-3", models::BatchStatus::AVAILABLE, ""};

    // Ibuprofen:
    models::Batch b8{0, med8, "IBU-25-01", "2025-03-12", "2027-11-12", 70, 1.80, 4.00, sup1, "Shelf A-2", models::BatchStatus::AVAILABLE, ""};

    // Salbutamol:
    models::Batch b9{0, med9, "SAL-25-01", "2025-01-05", "2027-03-05", 18, 5.50, 11.00, sup3, "Cabinet R", models::BatchStatus::AVAILABLE, ""};

    // Insulin:
    models::Batch b10{0, med10, "INS-25-01", "2025-02-01", "2026-11-30", 12, 18.00, 32.00, sup2, "Cold Storage Fridge #1", models::BatchStatus::AVAILABLE, ""};

    invService.stockInBatch(b1A, adminId, "INITIAL_STOCK", "INIT-001", err);
    invService.stockInBatch(b1B, adminId, "INITIAL_STOCK", "INIT-002", err);
    invService.stockInBatch(b1C, adminId, "INITIAL_STOCK", "INIT-003", err);
    invService.stockInBatch(b2A, adminId, "INITIAL_STOCK", "INIT-004", err);
    invService.stockInBatch(b2B, adminId, "INITIAL_STOCK", "INIT-005", err);
    invService.stockInBatch(b3, adminId, "INITIAL_STOCK", "INIT-006", err);
    invService.stockInBatch(b4, adminId, "INITIAL_STOCK", "INIT-007", err);
    invService.stockInBatch(b5, adminId, "INITIAL_STOCK", "INIT-008", err);
    invService.stockInBatch(b6, adminId, "INITIAL_STOCK", "INIT-009", err);
    invService.stockInBatch(b7, adminId, "INITIAL_STOCK", "INIT-010", err);
    invService.stockInBatch(b8, adminId, "INITIAL_STOCK", "INIT-011", err);
    invService.stockInBatch(b9, adminId, "INITIAL_STOCK", "INIT-012", err);
    invService.stockInBatch(b10, adminId, "INITIAL_STOCK", "INIT-013", err);

    // 5. Seed a demo completed sale to populate financial transactions
    std::string today = utils::DateUtils::getCurrentDate();
    std::vector<services::SaleCartItem> demoCart = {
        {med1, 5}, // 5 units of Paracetamol (should consume from Batch PAR-24-001 via FEFO)
        {med8, 2}  // 2 units of Ibuprofen
    };
    salesService.processSale(demoCart, empId, "CARD", "John Doe (Patient #4401)", "Fever prescription fulfillment", today);

    // Seed a purchase order
    purchService.createPurchaseOrder(sup3, "2026-11-20", "Emergency stock replenishment for low Cetirizine",
                                    {{med3, {40, 0.75}}}, adminId, err);
}

} // namespace medifind::seed
