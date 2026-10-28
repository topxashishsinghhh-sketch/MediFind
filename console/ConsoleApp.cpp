#include "console/ConsoleApp.hpp"
#include "database/SchemaMigrations.hpp"
#include "database/DatabaseBackup.hpp"
#include "seed/SeedData.hpp"
#include "utils/DateUtils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

namespace medifind::console {

ConsoleApp::ConsoleApp(const std::string& dbPath)
    : db_(dbPath),
      userRepo_(db_),
      medRepo_(db_),
      batchRepo_(db_),
      supplierRepo_(db_),
      poRepo_(db_),
      saleRepo_(db_),
      returnRepo_(db_),
      movementRepo_(db_),
      restockRepo_(db_),
      auditRepo_(db_),
      authService_(userRepo_, auditRepo_),
      invService_(db_, medRepo_, batchRepo_, movementRepo_, auditRepo_),
      purchService_(db_, supplierRepo_, poRepo_, medRepo_, batchRepo_, movementRepo_, auditRepo_),
      salesService_(db_, saleRepo_, batchRepo_, medRepo_, returnRepo_, movementRepo_, auditRepo_),
      restockService_(db_, restockRepo_, medRepo_, auditRepo_),
      reportService_(invService_, salesService_, purchService_, auditRepo_, medRepo_, batchRepo_),
      auditService_(auditRepo_) {

    // Ensure database tables exist
    database::SchemaMigrations::applyMigrations(db_);

    // Seed demo data if database is brand new
    seed::SeedData::seedDemoData(db_, authService_, invService_, purchService_, salesService_);

    // Auto-refresh expiry statuses for today
    invService_.refreshExpiryStatuses(utils::DateUtils::getCurrentDate());
}

void ConsoleApp::printHeader(const std::string& title) {
    std::cout << "\n========================================================================================\n";
    std::cout << "  MEDIFIND :: " << title << "\n";
    std::cout << "========================================================================================\n";
}

void ConsoleApp::printDivider() {
    std::cout << "----------------------------------------------------------------------------------------\n";
}

std::string ConsoleApp::promptString(const std::string& prompt) {
    std::cout << prompt;
    std::string val;
    std::getline(std::cin, val);
    return val;
}

int ConsoleApp::promptInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        try {
            return std::stoi(line);
        } catch (...) {
            std::cout << "[!] Invalid integer input. Please try again.\n";
        }
    }
}

double ConsoleApp::promptDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        try {
            return std::stod(line);
        } catch (...) {
            std::cout << "[!] Invalid numeric input. Please try again.\n";
        }
    }
}

void ConsoleApp::pause() {
    std::cout << "\nPress ENTER to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void ConsoleApp::run() {
    while (isRunning_) {
        if (!authService_.isAuthenticated()) {
            handleLogin();
        } else {
            if (authService_.isAdmin()) {
                runAdminMenu();
            } else {
                runEmployeeMenu();
            }
        }
    }
    std::cout << "\nThank you for using MediFind. Goodbye!\n\n";
}

void ConsoleApp::handleLogin() {
    printHeader("AUTHENTICATION & LOGIN");
    std::cout << "1. Login with Username & Password\n";
    std::cout << "2. Exit Application\n";
    printDivider();

    int choice = promptInt("Enter choice (1-2): ");
    if (choice == 2) {
        isRunning_ = false;
        return;
    }

    std::string username = promptString("Enter Username: ");
    std::string password = promptString("Enter Password: ");

    auto res = authService_.login(username, password, "CONSOLE");
    if (res.success) {
        std::cout << "\n[+] " << res.message << " Welcome, " << res.user->fullName << "!\n";
        pause();
    } else {
        std::cout << "\n[!] Authentication Failed: " << res.message << "\n";
        pause();
    }
}

void ConsoleApp::runAdminMenu() {
    navigationHistory_.push("Admin Main Menu");
    printHeader("ADMINISTRATOR CONTROL PANEL");
    std::cout << "Logged in as: " << authService_.getCurrentUser()->fullName << " [ADMINISTRATOR]\n";
    printDivider();
    std::cout << " 1. Executive Dashboard & Stock Valuation\n";
    std::cout << " 2. Medicine Catalog Management\n";
    std::cout << " 3. Batch Inventory & Stock-In\n";
    std::cout << " 4. Point of Sale (POS) - FEFO Billing\n";
    std::cout << " 5. Customer Returns Processing\n";
    std::cout << " 6. Suppliers & Purchase Orders\n";
    std::cout << " 7. Restock Requests (FIFO Queue)\n";
    std::cout << " 8. Physical Stock Reconciliation\n";
    std::cout << " 9. Reports & CSV Export Center\n";
    std::cout << "10. Database Backup & Disaster Recovery\n";
    std::cout << "11. User Management & Permissions\n";
    std::cout << "12. DSA Visualizer (HashTable, BST, Sorting)\n";
    std::cout << "13. Change Password\n";
    std::cout << "14. Logout\n";
    printDivider();

    int choice = promptInt("Select option (1-14): ");
    switch (choice) {
        case 1:  viewDashboard(); break;
        case 2:  manageMedicines(); break;
        case 3:  manageBatches(); break;
        case 4:  pointOfSaleWorkflow(); break;
        case 5:  processReturnWorkflow(); break;
        case 6:  manageSuppliersAndOrders(); break;
        case 7:  manageRestockRequests(); break;
        case 8:  stockReconciliationWorkflow(); break;
        case 9:  reportsAndExportsWorkflow(); break;
        case 10: backupAndRestoreWorkflow(); break;
        case 11: manageUsersWorkflow(); break;
        case 12: dsaDemonstrationWorkflow(); break;
        case 13: changePasswordWorkflow(); break;
        case 14: authService_.logout(); break;
        default: std::cout << "[!] Invalid selection.\n"; pause(); break;
    }
}

void ConsoleApp::runEmployeeMenu() {
    navigationHistory_.push("Employee Main Menu");
    printHeader("EMPLOYEE WORKSTATION");
    std::cout << "Logged in as: " << authService_.getCurrentUser()->fullName << " [EMPLOYEE]\n";
    printDivider();
    std::cout << " 1. View Dashboard & Alerts\n";
    std::cout << " 2. Search Medicines & Check Stock\n";
    std::cout << " 3. Point of Sale (POS) - Dispense via FEFO\n";
    std::cout << " 4. Process Customer Return\n";
    std::cout << " 5. Submit Restock Request\n";
    std::cout << " 6. Expiry & Low-Stock Alerts\n";
    std::cout << " 7. Change Password\n";
    std::cout << " 8. Logout\n";
    printDivider();

    int choice = promptInt("Select option (1-8): ");
    switch (choice) {
        case 1:  viewDashboard(); break;
        case 2:  searchMedicinesWorkflow(); break;
        case 3:  pointOfSaleWorkflow(); break;
        case 4:  processReturnWorkflow(); break;
        case 5:  submitRestockWorkflow(); break;
        case 6:  expiryAlertsWorkflow(); break;
        case 7:  changePasswordWorkflow(); break;
        case 8:  authService_.logout(); break;
        default: std::cout << "[!] Invalid selection.\n"; pause(); break;
    }
}

void ConsoleApp::viewDashboard() {
    printHeader("EXECUTIVE INVENTORY DASHBOARD");
    std::string today = utils::DateUtils::getCurrentDate();
    invService_.refreshExpiryStatuses(today);

    int totalUnits = invService_.getTotalUnitsInStock();
    double costVal = invService_.getTotalInventoryCostValue();
    double retailVal = invService_.getTotalInventoryRetailValue();
    auto lowStock = invService_.getLowStockMedicines();
    auto expiring = invService_.getExpiringBatches(today, 90);
    auto expired = invService_.getExpiredBatches(today);

    std::cout << "CURRENT DATE: " << today << "\n";
    printDivider();
    std::cout << std::left << std::setw(30) << "Total Active Medicines:"   << medRepo_.count() << "\n";
    std::cout << std::left << std::setw(30) << "Active Batches in Stock:"   << batchRepo_.countActiveBatches() << "\n";
    std::cout << std::left << std::setw(30) << "Total Stocked Units:"       << totalUnits << "\n";
    std::cout << std::left << std::setw(30) << "Inventory Valuation (Cost):"<< "$" << std::fixed << std::setprecision(2) << costVal << "\n";
    std::cout << std::left << std::setw(30) << "Projected Retail Revenue:"  << "$" << std::fixed << std::setprecision(2) << retailVal << "\n";
    std::cout << std::left << std::setw(30) << "Projected Gross Margin:"    << "$" << std::fixed << std::setprecision(2) << (retailVal - costVal) << "\n";
    printDivider();
    std::cout << "[!] Low-Stock Medicines Alert: " << lowStock.size() << " item(s) below threshold\n";
    std::cout << "[*] Near-Expiry Batches (90d):  " << expiring.size() << " batch(es) nearing expiry\n";
    std::cout << "[X] Expired Batches:            " << expired.size() << " batch(es) quarantined\n";
    printDivider();

    pause();
}

void ConsoleApp::manageMedicines() {
    printHeader("MEDICINE CATALOG MANAGEMENT");
    std::cout << "1. List All Medicines\n";
    std::cout << "2. Search Medicines (Name / Category / Barcode)\n";
    std::cout << "3. BST Prefix Autocomplete Search (DSA Demonstration)\n";
    std::cout << "4. Add New Medicine Record\n";
    std::cout << "5. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-5): ");
    switch (choice) {
        case 1: {
            auto meds = medRepo_.listAll();
            printHeader("ALL ACTIVE MEDICINES (" + std::to_string(meds.size()) + ")");
            std::cout << std::left << std::setw(6)  << "ID"
                      << std::left << std::setw(25) << "Name"
                      << std::left << std::setw(12) << "Strength"
                      << std::left << std::setw(12) << "Form"
                      << std::left << std::setw(18) << "Category"
                      << std::left << std::setw(14) << "Barcode"
                      << std::right << std::setw(8) << "Stock"
                      << "\n";
            printDivider();
            for (const auto& m : meds) {
                int stock = medRepo_.getTotalStockQuantity(m.id);
                std::cout << std::left  << std::setw(6)  << m.id
                          << std::left  << std::setw(25) << m.name
                          << std::left  << std::setw(12) << m.strength
                          << std::left  << std::setw(12) << m.dosageForm
                          << std::left  << std::setw(18) << m.category
                          << std::left  << std::setw(14) << (m.barcode.empty() ? "-" : m.barcode)
                          << std::right << std::setw(8)  << stock
                          << "\n";
            }
            pause();
            break;
        }
        case 2: searchMedicinesWorkflow(); break;
        case 3: bstPrefixSearchWorkflow(); break;
        case 4: addMedicineWorkflow(); break;
        case 5: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::addMedicineWorkflow() {
    printHeader("ADD NEW MEDICINE RECORD");
    models::Medicine m;
    m.name = promptString("Medicine Name: ");
    m.brand = promptString("Brand / Manufacturer: ");
    m.strength = promptString("Strength (e.g. 500 mg): ");
    m.dosageForm = promptString("Dosage Form (e.g. Tablet, Syrup): ");
    m.category = promptString("Category (e.g. Antibiotics, Analgesics): ");
    m.barcode = promptString("Barcode (Optional, press Enter to skip): ");
    m.description = promptString("Description / Indications: ");
    m.minStockThreshold = promptInt("Min Stock Alert Threshold (e.g. 15): ");

    std::string err;
    int id = invService_.addMedicine(m, authService_.getCurrentUser()->id, err);
    if (id > 0) {
        std::cout << "\n[+] Medicine created successfully with ID #" << id << "!\n";
    } else {
        std::cout << "\n[!] Error adding medicine: " << err << "\n";
    }
    pause();
}

void ConsoleApp::searchMedicinesWorkflow() {
    printHeader("SEARCH MEDICINES");
    std::string q = promptString("Enter search keyword (name, brand, or barcode): ");
    auto results = invService_.searchMedicines(q);

    std::cout << "\nFound " << results.size() << " match(es):\n";
    printDivider();
    std::cout << std::left << std::setw(6)  << "ID"
              << std::left << std::setw(25) << "Name"
              << std::left << std::setw(12) << "Strength"
              << std::left << std::setw(18) << "Category"
              << std::right << std::setw(8) << "Stock"
              << "\n";
    printDivider();
    for (const auto& m : results) {
        int stock = medRepo_.getTotalStockQuantity(m.id);
        std::cout << std::left  << std::setw(6)  << m.id
                  << std::left  << std::setw(25) << m.name
                  << std::left  << std::setw(12) << m.strength
                  << std::left  << std::setw(18) << m.category
                  << std::right << std::setw(8)  << stock
                  << "\n";
    }
    pause();
}

void ConsoleApp::bstPrefixSearchWorkflow() {
    printHeader("BINARY SEARCH TREE (BST) PREFIX AUTOCOMPLETE DEMO");
    std::cout << "Demonstrates in-memory O(log N) prefix traversal using BinarySearchTree.\n";
    std::string prefix = promptString("Enter prefix to match (e.g. 'A', 'Par', 'Met'): ");

    auto matches = invService_.searchMedicinesByPrefixBst(prefix);
    std::cout << "\nFound " << matches.size() << " matching medicine(s) in BST:\n";
    printDivider();
    for (const auto& match : matches) {
        std::cout << "  - " << std::left << std::setw(30) << match.first << " [Medicine ID #" << match.second << "]\n";
    }
    pause();
}

void ConsoleApp::manageBatches() {
    printHeader("BATCH INVENTORY & STOCK-IN");
    std::cout << "1. List Batches for a Medicine\n";
    std::cout << "2. Stock-In New Batch (Add Stock)\n";
    std::cout << "3. Manual Stock Adjustment (Admin Authorization Required)\n";
    std::cout << "4. Expiry & Near-Expiry Alerts\n";
    std::cout << "5. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-5): ");
    switch (choice) {
        case 1: {
            int medId = promptInt("Enter Medicine ID: ");
            auto batches = invService_.getBatchesForMedicine(medId);
            printHeader("BATCHES FOR MEDICINE #" + std::to_string(medId));
            std::cout << std::left << std::setw(6)  << "ID"
                      << std::left << std::setw(14) << "Batch #"
                      << std::left << std::setw(12) << "Mfg Date"
                      << std::left << std::setw(12) << "Expiry"
                      << std::right << std::setw(8) << "Qty"
                      << std::right << std::setw(10) << "Cost ($)"
                      << std::right << std::setw(10) << "Price ($)"
                      << std::left << std::setw(14) << "  Status"
                      << "\n";
            printDivider();
            for (const auto& b : batches) {
                std::cout << std::left  << std::setw(6)  << b.id
                          << std::left  << std::setw(14) << b.batchNumber
                          << std::left  << std::setw(12) << b.mfgDate
                          << std::left  << std::setw(12) << b.expiryDate
                          << std::right << std::setw(8)  << b.quantity
                          << std::right << std::setw(10) << std::fixed << std::setprecision(2) << b.purchaseCost
                          << std::right << std::setw(10) << std::fixed << std::setprecision(2) << b.sellingPrice
                          << "  " << std::left << std::setw(14) << models::batchStatusToString(b.status)
                          << "\n";
            }
            pause();
            break;
        }
        case 2: stockInBatchWorkflow(); break;
        case 3: adjustStockWorkflow(); break;
        case 4: expiryAlertsWorkflow(); break;
        case 5: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::stockInBatchWorkflow() {
    printHeader("STOCK-IN NEW BATCH");
    models::Batch b;
    b.medicineId = promptInt("Medicine ID: ");
    b.batchNumber = promptString("Manufacturer Batch Number (e.g. B-2026-99): ");
    b.mfgDate = promptString("Manufacturing Date (YYYY-MM-DD): ");
    b.expiryDate = promptString("Expiry Date (YYYY-MM-DD): ");
    b.quantity = promptInt("Units Received: ");
    b.purchaseCost = promptDouble("Unit Purchase Cost ($): ");
    b.sellingPrice = promptDouble("Unit Selling Price ($): ");
    b.supplierId = promptInt("Supplier ID (or 0 if direct): ");
    b.storageLocation = promptString("Storage Location (e.g. Shelf A-2): ");

    std::string err;
    int batchId = invService_.stockInBatch(b, authService_.getCurrentUser()->id, "DIRECT_STOCK_IN", "", err);
    if (batchId > 0) {
        std::cout << "\n[+] Batch #" << batchId << " successfully stocked in!\n";
    } else {
        std::cout << "\n[!] Stock-in failed: " << err << "\n";
    }
    pause();
}

void ConsoleApp::adjustStockWorkflow() {
    printHeader("MANUAL STOCK ADJUSTMENT");
    int batchId = promptInt("Enter Batch ID to Adjust: ");
    auto batchOpt = invService_.getBatchById(batchId);
    if (!batchOpt) {
        std::cout << "[!] Batch not found.\n";
        pause();
        return;
    }

    std::cout << "Current Recorded Quantity: " << batchOpt->quantity << " units\n";
    int newQty = promptInt("Enter New Verified Physical Quantity: ");
    std::string reason = promptString("Reason for Adjustment (Audit Mandatory): ");

    auto res = invService_.adjustStock(batchId, newQty, reason, authService_.getCurrentUser()->id, authService_.isAdmin());
    if (res.success) {
        std::cout << "\n[+] Stock updated successfully! (Old: " << res.oldQuantity << ", New: " << res.newQuantity
                  << ", Diff: " << res.difference << ")\n";
    } else {
        std::cout << "\n[!] Adjustment rejected: " << res.message << "\n";
    }
    pause();
}

void ConsoleApp::expiryAlertsWorkflow() {
    std::string today = utils::DateUtils::getCurrentDate();
    std::string report = reportService_.generateExpiryAlertReport(today, 90);
    std::cout << report;
    pause();
}

void ConsoleApp::pointOfSaleWorkflow() {
    printHeader("POINT OF SALE (POS) - FEFO ALLOCATION ENGINE");
    std::cout << "All medicines will be automatically allocated from earliest-expiring eligible batches.\n";
    std::string today = utils::DateUtils::getCurrentDate();

    std::vector<services::SaleCartItem> cart;
    bool adding = true;

    while (adding) {
        int medId = promptInt("\nEnter Medicine ID to add to cart: ");
        auto med = invService_.getMedicineById(medId);
        if (!med) {
            std::cout << "[!] Medicine ID not found.\n";
        } else {
            int stock = medRepo_.getTotalStockQuantity(medId);
            std::cout << "Selected: " << med->name << " (" << med->strength << ") | Available Stock: " << stock << "\n";
            int qty = promptInt("Enter Quantity: ");
            cart.push_back({medId, qty});
        }

        std::string cont = promptString("Add another item? (y/n): ");
        if (cont != "y" && cont != "Y") {
            adding = false;
        }
    }

    if (cart.empty()) {
        std::cout << "[!] Cart is empty. Aborting.\n";
        pause();
        return;
    }

    // Preview FEFO allocation
    printHeader("FEFO ALLOCATION PREVIEW");
    auto previews = salesService_.previewCartAllocation(cart, today);
    bool allValid = true;
    double grandTotal = 0.0;

    for (const auto& p : previews) {
        auto med = invService_.getMedicineById(p.medicineId);
        std::string medName = med ? med->name : "Medicine";
        std::cout << "\nItem: " << medName << " (Requested: " << p.requestedQuantity << ")\n";
        if (!p.isSuccessful) {
            std::cout << "  [!] ERROR: " << p.message << "\n";
            allValid = false;
        } else {
            for (const auto& item : p.items) {
                std::cout << "  -> Batch #" << item.batchNumber << " (Exp: " << item.expiryDate << "): "
                          << item.allocatedQuantity << " units @ $" << std::fixed << std::setprecision(2) << item.unitPrice
                          << " = $" << item.subtotal << "\n";
            }
            grandTotal += p.totalAmount;
        }
    }

    if (!allValid) {
        std::cout << "\n[!] One or more items cannot be fulfilled due to stock / expiry constraints.\n";
        pause();
        return;
    }

    std::cout << "\nTotal Cart Amount: $" << std::fixed << std::setprecision(2) << grandTotal << "\n";
    std::string confirm = promptString("Confirm and finalize sale? (y/n): ");
    if (confirm != "y" && confirm != "Y") {
        std::cout << "Sale cancelled.\n";
        pause();
        return;
    }

    std::string customer = promptString("Customer Name (Walk-in if blank): ");
    std::string payMethod = promptString("Payment Method (CASH, CARD, UPI): ");
    std::string notes = promptString("Sale Notes: ");

    auto res = salesService_.processSale(cart, authService_.getCurrentUser()->id, payMethod, customer, notes, today);
    if (res.success) {
        printHeader("INVOICE GENERATED");
        std::cout << "Invoice Number : " << res.invoiceNumber << "\n";
        std::cout << "Total Paid     : $" << std::fixed << std::setprecision(2) << res.totalAmount << "\n";
        std::cout << "Payment Method : " << payMethod << "\n";
        std::cout << "Customer       : " << (customer.empty() ? "Walk-in" : customer) << "\n";
        std::cout << "Dispatched Via : FEFO Policy (Earliest Expired Batches Consumed)\n";
        printDivider();
        std::cout << "[+] Inventory balances and audit logs updated atomically!\n";
    } else {
        std::cout << "\n[!] Transaction failed: " << res.message << "\n";
    }
    pause();
}

void ConsoleApp::processReturnWorkflow() {
    printHeader("PROCESS CUSTOMER RETURN");
    int saleId = promptInt("Enter Sale ID: ");
    auto saleOpt = salesService_.getSaleById(saleId);
    if (!saleOpt) {
        std::cout << "[!] Sale invoice not found.\n";
        pause();
        return;
    }

    std::cout << "\nInvoice #" << saleOpt->invoiceNumber << " | Date: " << saleOpt->saleDate << "\n";
    std::cout << "Items in this Invoice:\n";
    printDivider();
    std::cout << std::left << std::setw(8) << "Item ID"
              << std::left << std::setw(25) << "Medicine"
              << std::right << std::setw(8) << "Sold Qty"
              << std::right << std::setw(12) << "Unit Price"
              << "\n";
    printDivider();
    for (const auto& it : saleOpt->items) {
        auto med = invService_.getMedicineById(it.medicineId);
        std::cout << std::left  << std::setw(8)  << it.id
                  << std::left  << std::setw(25) << (med ? med->name : "Medicine")
                  << std::right << std::setw(8)  << it.quantity
                  << std::right << std::setw(12) << std::fixed << std::setprecision(2) << it.unitPrice
                  << "\n";
    }

    int saleItemId = promptInt("\nEnter Sale Item ID to Return: ");
    int returnQty = promptInt("Enter Quantity to Return: ");
    std::string reason = promptString("Reason for Return: ");
    std::string sellableStr = promptString("Is item unopened and safe to return to sellable stock? (y/n): ");
    bool sellable = (sellableStr == "y" || sellableStr == "Y");

    auto retRes = salesService_.processReturn(saleId, saleItemId, returnQty, reason, sellable, authService_.getCurrentUser()->id);
    if (retRes.success) {
        std::cout << "\n[+] Return Processed Successfully!\n";
        std::cout << "Return Reference: " << retRes.returnRef << "\n";
        std::cout << "Refund Amount   : $" << std::fixed << std::setprecision(2) << retRes.refundAmount << "\n";
        std::cout << "Disposition     : " << (sellable ? "Returned to batch stock" : "Quarantined / Disposed") << "\n";
    } else {
        std::cout << "\n[!] Return rejected: " << retRes.message << "\n";
    }
    pause();
}

void ConsoleApp::manageSuppliersAndOrders() {
    printHeader("SUPPLIER & PURCHASE ORDER MANAGEMENT");
    std::cout << "1. List All Suppliers\n";
    std::cout << "2. Add New Supplier\n";
    std::cout << "3. Create Purchase Order\n";
    std::cout << "4. List Purchase Orders\n";
    std::cout << "5. Receive Shipment Against Purchase Order\n";
    std::cout << "6. View Automated Reorder Recommendations\n";
    std::cout << "7. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-7): ");
    switch (choice) {
        case 1: {
            auto sups = purchService_.listSuppliers();
            printHeader("REGISTERED SUPPLIERS (" + std::to_string(sups.size()) + ")");
            for (const auto& s : sups) {
                std::cout << "#" << s.id << " : " << s.name << " | Contact: " << s.contactPerson << " | Phone: " << s.phone << "\n";
            }
            pause();
            break;
        }
        case 2: {
            models::Supplier s;
            s.name = promptString("Supplier Company Name: ");
            s.contactPerson = promptString("Contact Person: ");
            s.email = promptString("Email Address: ");
            s.phone = promptString("Phone Number: ");
            s.address = promptString("Physical Address: ");
            std::string err;
            int id = purchService_.addSupplier(s, authService_.getCurrentUser()->id, err);
            if (id > 0) std::cout << "\n[+] Supplier created with ID #" << id << "!\n";
            else std::cout << "\n[!] Error: " << err << "\n";
            pause();
            break;
        }
        case 3: createPurchaseOrderWorkflow(); break;
        case 4: {
            auto orders = purchService_.listOrders();
            printHeader("PURCHASE ORDERS (" + std::to_string(orders.size()) + ")");
            std::cout << std::left << std::setw(6)  << "ID"
                      << std::left << std::setw(16) << "Order Ref"
                      << std::left << std::setw(12) << "Date"
                      << std::left << std::setw(18) << "Status"
                      << std::right << std::setw(12) << "Total Cost"
                      << "\n";
            printDivider();
            for (const auto& po : orders) {
                std::cout << std::left  << std::setw(6)  << po.id
                          << std::left  << std::setw(16) << po.orderRef
                          << std::left  << std::setw(12) << po.orderDate
                          << std::left  << std::setw(18) << models::orderStatusToString(po.status)
                          << std::right << std::setw(12) << std::fixed << std::setprecision(2) << po.totalCost
                          << "\n";
            }
            pause();
            break;
        }
        case 5: receiveShipmentWorkflow(); break;
        case 6: {
            auto recs = purchService_.generateReorderRecommendations();
            printHeader("AUTOMATED REORDER RECOMMENDATIONS (" + std::to_string(recs.size()) + ")");
            for (const auto& r : recs) {
                std::cout << "Medicine: " << std::left << std::setw(25) << r.medicineName
                          << " | Current Stock: " << r.currentStock
                          << " | Min: " << r.minThreshold
                          << " | Reorder Qty: " << r.recommendedReorderQuantity
                          << " | Est Cost: $" << std::fixed << std::setprecision(2) << r.estimatedCost
                          << " | Supplier: " << r.suggestedSupplierName << "\n";
            }
            pause();
            break;
        }
        case 7: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::createPurchaseOrderWorkflow() {
    printHeader("CREATE PURCHASE ORDER");
    int supId = promptInt("Supplier ID: ");
    std::string expDelivery = promptString("Expected Delivery Date (YYYY-MM-DD): ");
    std::string notes = promptString("PO Notes / Terms: ");

    std::vector<std::pair<int, std::pair<int, double>>> items;
    bool adding = true;
    while (adding) {
        int medId = promptInt("Medicine ID: ");
        int qty = promptInt("Quantity to Order: ");
        double cost = promptDouble("Unit Purchase Cost ($): ");
        items.push_back({medId, {qty, cost}});

        std::string c = promptString("Add another item? (y/n): ");
        if (c != "y" && c != "Y") adding = false;
    }

    std::string err;
    int poId = purchService_.createPurchaseOrder(supId, expDelivery, notes, items, authService_.getCurrentUser()->id, err);
    if (poId > 0) {
        std::cout << "\n[+] Purchase order successfully created with ID #" << poId << "!\n";
    } else {
        std::cout << "\n[!] Error creating PO: " << err << "\n";
    }
    pause();
}

void ConsoleApp::receiveShipmentWorkflow() {
    printHeader("RECEIVE SHIPMENT AGAINST PURCHASE ORDER");
    int poId = promptInt("Enter Purchase Order ID: ");
    auto poOpt = purchService_.getOrderById(poId);
    if (!poOpt) {
        std::cout << "[!] Purchase order not found.\n";
        pause();
        return;
    }

    std::cout << "Order Ref: " << poOpt->orderRef << " | Status: " << models::orderStatusToString(poOpt->status) << "\n";
    std::cout << "Items in PO:\n";
    for (const auto& it : poOpt->items) {
        auto med = invService_.getMedicineById(it.medicineId);
        std::cout << "  Item #" << it.id << ": " << (med ? med->name : "Medicine")
                  << " | Ordered: " << it.requestedQuantity << ", Already Received: " << it.receivedQuantity << "\n";
    }

    std::vector<services::ReceiveShipmentItem> toReceive;
    for (const auto& it : poOpt->items) {
        int remaining = it.requestedQuantity - it.receivedQuantity;
        if (remaining <= 0) continue;

        auto med = invService_.getMedicineById(it.medicineId);
        std::cout << "\nReceiving for " << (med ? med->name : "Medicine") << " (Remaining to receive: " << remaining << "):\n";
        int qty = promptInt("Enter Quantity Delivered in this shipment (0 to skip): ");
        if (qty > 0) {
            services::ReceiveShipmentItem rsi;
            rsi.orderItemId = it.id;
            rsi.medicineId = it.medicineId;
            rsi.quantityToReceive = qty;
            rsi.batchNumber = promptString("Manufacturer Batch Number on package: ");
            rsi.mfgDate = promptString("Mfg Date (YYYY-MM-DD): ");
            rsi.expiryDate = promptString("Expiry Date (YYYY-MM-DD): ");
            rsi.sellingPrice = promptDouble("Retail Selling Price ($): ");
            rsi.storageLocation = promptString("Put-away Storage Location: ");
            toReceive.push_back(rsi);
        }
    }

    if (toReceive.empty()) {
        std::cout << "No items received.\n";
        pause();
        return;
    }

    std::string err;
    bool ok = purchService_.receiveShipment(poId, toReceive, authService_.getCurrentUser()->id, err);
    if (ok) {
        std::cout << "\n[+] Shipment received! Batches created and inventory updated.\n";
    } else {
        std::cout << "\n[!] Error receiving shipment: " << err << "\n";
    }
    pause();
}

void ConsoleApp::manageRestockRequests() {
    printHeader("RESTOCK REQUESTS MANAGEMENT (FIFO QUEUE)");
    std::cout << "1. List All Restock Requests\n";
    std::cout << "2. Submit New Restock Request\n";
    std::cout << "3. Process Next Pending Request from FIFO Queue (Admin)\n";
    std::cout << "4. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-4): ");
    switch (choice) {
        case 1: {
            auto reqs = restockService_.listAllRequests();
            printHeader("RESTOCK REQUESTS (" + std::to_string(reqs.size()) + ")");
            for (const auto& r : reqs) {
                auto med = invService_.getMedicineById(r.medicineId);
                std::cout << r.requestRef << " | " << (med ? med->name : "Medicine")
                          << " | Qty: " << r.requestedQuantity
                          << " | Status: " << models::restockStatusToString(r.status)
                          << " | Reason: " << r.reason << "\n";
            }
            pause();
            break;
        }
        case 2: submitRestockWorkflow(); break;
        case 3: processRestockQueueFifoWorkflow(); break;
        case 4: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::submitRestockWorkflow() {
    printHeader("SUBMIT RESTOCK REQUEST");
    int medId = promptInt("Medicine ID needing restock: ");
    int qty = promptInt("Requested Restock Quantity: ");
    std::string reason = promptString("Reason for Request: ");

    std::string err;
    int reqId = restockService_.submitRequest(medId, qty, reason, authService_.getCurrentUser()->id, err);
    if (reqId > 0) {
        std::cout << "\n[+] Restock request submitted to queue! (Request ID #" << reqId << ")\n";
    } else {
        std::cout << "\n[!] Error submitting request: " << err << "\n";
    }
    pause();
}

void ConsoleApp::processRestockQueueFifoWorkflow() {
    printHeader("PROCESS NEXT PENDING RESTOCK REQUEST (FIFO)");
    auto nextOpt = restockService_.popNextPendingFromQueue();
    if (!nextOpt) {
        std::cout << "No pending restock requests in the FIFO queue!\n";
        pause();
        return;
    }

    const auto& req = *nextOpt;
    auto med = invService_.getMedicineById(req.medicineId);
    std::cout << "Processing Request: " << req.requestRef << "\n";
    std::cout << "Medicine: " << (med ? med->name : "Medicine") << "\n";
    std::cout << "Requested Quantity: " << req.requestedQuantity << "\n";
    std::cout << "Reason: " << req.reason << "\n";
    printDivider();

    std::cout << "1. Approve Request\n";
    std::cout << "2. Reject Request\n";
    std::cout << "3. Skip for Now\n";

    int choice = promptInt("Action (1-3): ");
    if (choice == 1) {
        std::string notes = promptString("Approval Notes: ");
        std::string err;
        restockService_.reviewRequest(req.id, models::RestockStatus::APPROVED, authService_.getCurrentUser()->id, notes, err);
        std::cout << "\n[+] Request Approved!\n";
    } else if (choice == 2) {
        std::string notes = promptString("Rejection Reason: ");
        std::string err;
        restockService_.reviewRequest(req.id, models::RestockStatus::REJECTED, authService_.getCurrentUser()->id, notes, err);
        std::cout << "\n[-] Request Rejected.\n";
    }
    pause();
}

void ConsoleApp::stockReconciliationWorkflow() {
    printHeader("PHYSICAL STOCK RECONCILIATION");
    std::cout << "Enter physical counts for batches to identify stock discrepancies.\n";

    auto batches = batchRepo_.listAll();
    std::vector<std::pair<int, int>> physicalCounts;

    for (const auto& b : batches) {
        if (b.quantity == 0 && b.status == models::BatchStatus::DEPLETED) continue;
        auto med = invService_.getMedicineById(b.medicineId);
        std::cout << "Batch #" << b.batchNumber << " (" << (med ? med->name : "Med") << ") | Recorded: " << b.quantity << "\n";
        std::string countStr = promptString("  Enter physical count (press Enter to accept recorded " + std::to_string(b.quantity) + "): ");
        int physical = b.quantity;
        if (!countStr.empty()) {
            try { physical = std::stoi(countStr); } catch (...) {}
        }
        physicalCounts.push_back({b.id, physical});
    }

    auto discrepancies = invService_.calculateReconciliation(physicalCounts);
    printHeader("RECONCILIATION DISCREPANCIES DETECTED: " + std::to_string(discrepancies.size()));

    if (discrepancies.empty()) {
        std::cout << "[+] All physical counts match recorded database values exactly!\n";
        pause();
        return;
    }

    std::cout << std::left << std::setw(20) << "Medicine"
              << std::left << std::setw(14) << "Batch #"
              << std::right << std::setw(10) << "System Qty"
              << std::right << std::setw(12) << "Physical Qty"
              << std::right << std::setw(10) << "Diff"
              << "\n";
    printDivider();
    for (const auto& d : discrepancies) {
        std::cout << std::left  << std::setw(20) << d.medicineName
                  << std::left  << std::setw(14) << d.batchNumber
                  << std::right << std::setw(10) << d.systemQuantity
                  << std::right << std::setw(12) << d.physicalQuantity
                  << std::right << std::setw(10) << (d.difference > 0 ? ("+" + std::to_string(d.difference)) : std::to_string(d.difference))
                  << "\n";
    }

    std::string apply = promptString("\nApply adjustments to sync database with physical count? (y/n): ");
    if (apply == "y" || apply == "Y") {
        for (const auto& d : discrepancies) {
            std::string err;
            invService_.applyReconciliationAdjustment(d.batchId, d.physicalQuantity, "Physical Count Audit", authService_.getCurrentUser()->id, err);
        }
        std::cout << "\n[+] All adjustments applied and logged in audit trail!\n";
    }
    pause();
}

void ConsoleApp::reportsAndExportsWorkflow() {
    printHeader("REPORTS & CSV EXPORT CENTER");
    std::cout << "1. Display Inventory Valuation Summary\n";
    std::cout << "2. Display Expiry & Quarantine Alert Report\n";
    std::cout << "3. Display Low-Stock Restock Report\n";
    std::cout << "4. Display Sales Performance Report\n";
    std::cout << "5. Export Inventory to CSV (inventory_export.csv)\n";
    std::cout << "6. Export Batches to CSV (batches_export.csv)\n";
    std::cout << "7. Export Sales to CSV (sales_export.csv)\n";
    std::cout << "8. Export Audit Logs to CSV (audit_export.csv)\n";
    std::cout << "9. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-9): ");
    switch (choice) {
        case 1: std::cout << reportService_.generateInventoryValuationReport(); pause(); break;
        case 2: std::cout << reportService_.generateExpiryAlertReport(utils::DateUtils::getCurrentDate(), 90); pause(); break;
        case 3: std::cout << reportService_.generateLowStockReport(); pause(); break;
        case 4: std::cout << reportService_.generateSalesSummaryReport(); pause(); break;
        case 5: {
            bool ok = reportService_.exportInventoryCsv("inventory_export.csv");
            std::cout << (ok ? "[+] Exported to inventory_export.csv\n" : "[!] Export failed\n");
            pause();
            break;
        }
        case 6: {
            bool ok = reportService_.exportBatchesCsv("batches_export.csv");
            std::cout << (ok ? "[+] Exported to batches_export.csv\n" : "[!] Export failed\n");
            pause();
            break;
        }
        case 7: {
            bool ok = reportService_.exportSalesCsv("sales_export.csv");
            std::cout << (ok ? "[+] Exported to sales_export.csv\n" : "[!] Export failed\n");
            pause();
            break;
        }
        case 8: {
            bool ok = reportService_.exportAuditLogsCsv("audit_export.csv");
            std::cout << (ok ? "[+] Exported to audit_export.csv\n" : "[!] Export failed\n");
            pause();
            break;
        }
        case 9: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::backupAndRestoreWorkflow() {
    printHeader("DATABASE BACKUP & DISASTER RECOVERY");
    std::cout << "1. Create Online Backup of Live Database\n";
    std::cout << "2. Verify Backup File Integrity\n";
    std::cout << "3. Restore Database from Backup File\n";
    std::cout << "4. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-4): ");
    switch (choice) {
        case 1: {
            std::string dest = promptString("Enter backup destination filename (e.g. backup_2026.db): ");
            if (dest.empty()) dest = "medifind_backup.db";
            std::string err;
            bool ok = database::DatabaseBackup::createBackup(db_, dest, err);
            if (ok) {
                std::cout << "\n[+] Live SQLite backup created and verified successfully at " << dest << "!\n";
            } else {
                std::cout << "\n[!] Backup failed: " << err << "\n";
            }
            pause();
            break;
        }
        case 2: {
            std::string file = promptString("Enter backup file to inspect: ");
            std::string err;
            bool ok = database::DatabaseBackup::verifyBackup(file, err);
            if (ok) std::cout << "\n[+] Backup file is valid and verified healthy.\n";
            else std::cout << "\n[!] Verification error: " << err << "\n";
            pause();
            break;
        }
        case 3: {
            std::string file = promptString("Enter backup file to restore from: ");
            std::string confirm = promptString("Are you sure you want to restore? This will overwrite the live database! (yes/no): ");
            if (confirm != "yes") {
                std::cout << "Restore aborted.\n";
                pause();
                return;
            }
            std::string err;
            bool ok = database::DatabaseBackup::restoreBackup(db_, file, err);
            if (ok) {
                std::cout << "\n[+] Database restored successfully! Refreshing caches...\n";
                invService_.refreshDsaIndexes();
                restockService_.refreshQueue();
            } else {
                std::cout << "\n[!] Restore failed: " << err << "\n";
            }
            pause();
            break;
        }
        case 4: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::manageUsersWorkflow() {
    printHeader("USER MANAGEMENT & ROLE PERMISSIONS");
    std::cout << "1. List All Users\n";
    std::cout << "2. Register New Employee / Administrator\n";
    std::cout << "3. Toggle User Active / Deactivated Status\n";
    std::cout << "4. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select option (1-4): ");
    switch (choice) {
        case 1: {
            auto users = userRepo_.listAll(true);
            printHeader("REGISTERED ACCOUNTS (" + std::to_string(users.size()) + ")");
            std::cout << std::left << std::setw(6)  << "ID"
                      << std::left << std::setw(16) << "Username"
                      << std::left << std::setw(24) << "Full Name"
                      << std::left << std::setw(16) << "Role"
                      << std::left << std::setw(10) << "Status"
                      << "\n";
            printDivider();
            for (const auto& u : users) {
                std::cout << std::left  << std::setw(6)  << u.id
                          << std::left  << std::setw(16) << u.username
                          << std::left  << std::setw(24) << u.fullName
                          << std::left  << std::setw(16) << models::userRoleToString(u.role)
                          << std::left  << std::setw(10) << (u.isActive ? "ACTIVE" : "DISABLED")
                          << "\n";
            }
            pause();
            break;
        }
        case 2: {
            std::string username = promptString("New Username: ");
            std::string password = promptString("New Password (min 6 chars): ");
            std::string fullName = promptString("Full Name: ");
            std::string roleStr = promptString("Role (ADMIN or EMPLOYEE): ");
            models::UserRole role = (roleStr == "ADMIN" || roleStr == "admin") ? models::UserRole::ADMINISTRATOR : models::UserRole::EMPLOYEE;

            auto res = authService_.registerUser(username, password, fullName, role);
            if (res.success) std::cout << "\n[+] User registered successfully!\n";
            else std::cout << "\n[!] Registration failed: " << res.message << "\n";
            pause();
            break;
        }
        case 3: {
            int userId = promptInt("Enter User ID to Toggle Status: ");
            auto uOpt = userRepo_.findById(userId);
            if (uOpt) {
                bool newStatus = !uOpt->isActive;
                userRepo_.setActiveStatus(userId, newStatus);
                std::cout << "\n[+] User #" << userId << " status updated to " << (newStatus ? "ACTIVE" : "DISABLED") << "!\n";
            } else {
                std::cout << "[!] User not found.\n";
            }
            pause();
            break;
        }
        case 4: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::dsaDemonstrationWorkflow() {
    printHeader("DATA STRUCTURES & ALGORITHMS (DSA) DEMONSTRATION");
    std::cout << "1. Hash Table: Fast O(1) Medicine Cache Lookup & Collision Stats\n";
    std::cout << "2. Binary Search Tree (BST): In-Order Lexicographical Traversal\n";
    std::cout << "3. FIFO Queue: Restock Request Queue Status & Processing\n";
    std::cout << "4. LIFO Stack: Breadcrumb Navigation & Undo History\n";
    std::cout << "5. Return to Main Menu\n";
    printDivider();

    int choice = promptInt("Select DSA structure to inspect (1-5): ");
    switch (choice) {
        case 1: {
            printHeader("CUSTOM HASH TABLE (SEPARATE CHAINING)");
            std::cout << "Demonstrates prime bucket sizing, load factor rehashing, and O(1) lookup.\n";
            int medId = promptInt("Enter Medicine ID to look up in HashTable: ");
            auto cached = invService_.getMedicineById(medId);
            if (cached) {
                std::cout << "\n[+] Found in HashTable in O(1) time!\n";
                std::cout << "  Name     : " << cached->name << "\n";
                std::cout << "  Brand    : " << cached->brand << "\n";
                std::cout << "  Category : " << cached->category << "\n";
            } else {
                std::cout << "[-] ID not found in HashTable.\n";
            }
            pause();
            break;
        }
        case 2: {
            printHeader("BINARY SEARCH TREE (BST) - IN-ORDER TRAVERSAL");
            std::cout << "Traversing BST yields medicines in guaranteed alphabetical order in O(N) time:\n";
            printDivider();
            auto inOrder = invService_.getMedicinesInOrderTraversal();
            for (size_t i = 0; i < inOrder.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << std::left << std::setw(30) << inOrder[i].first
                          << " (ID #" << inOrder[i].second << ")\n";
            }
            pause();
            break;
        }
        case 3: {
            printHeader("FIFO QUEUE - RESTOCK REQUEST PIPELINE");
            std::cout << "Pending requests waiting in FIFO order: " << restockService_.getPendingCount() << "\n";
            auto pending = restockService_.listPendingRequests();
            for (size_t i = 0; i < pending.size(); ++i) {
                std::cout << "  [Position " << (i + 1) << "] " << pending[i].requestRef
                          << " -> Medicine #" << pending[i].medicineId << " (Qty: " << pending[i].requestedQuantity << ")\n";
            }
            pause();
            break;
        }
        case 4: {
            printHeader("LIFO STACK - NAVIGATION BREADCRUMBS");
            std::cout << "Demonstrating LIFO stack depth: " << navigationHistory_.size() << " frames recorded.\n";
            std::cout << "Current top of stack: " << (navigationHistory_.empty() ? "None" : navigationHistory_.top()) << "\n";
            pause();
            break;
        }
        case 5: return;
        default: std::cout << "[!] Invalid choice.\n"; pause(); break;
    }
}

void ConsoleApp::changePasswordWorkflow() {
    printHeader("CHANGE PASSWORD");
    std::string oldPass = promptString("Enter Current Password: ");
    std::string newPass = promptString("Enter New Password (min 6 characters): ");

    std::string err;
    bool ok = authService_.changePassword(oldPass, newPass, err);
    if (ok) {
        std::cout << "\n[+] Password changed successfully! Updated salted hash stored in database.\n";
    } else {
        std::cout << "\n[!] Error: " << err << "\n";
    }
    pause();
}

} // namespace medifind::console
