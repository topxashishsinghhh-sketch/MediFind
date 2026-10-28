/**
 * MediFind Enterprise Pharmacy Management System
 * Client-Side Application Architecture & C++ SQLite3 API Bridge
 */

// ============================================================================
// Initial Master Datasets
// ============================================================================
const INITIAL_MEDICINES = [
  { id: 1, name: "Paracetamol 500mg", brand: "Crocin", strength: "500 mg", dosageForm: "Tablet", category: "Analgesic", barcode: "MED-890101", minStockThreshold: 100 },
  { id: 2, name: "Amoxicillin 250mg", brand: "Amoxil", strength: "250 mg", dosageForm: "Capsule", category: "Antibiotic", barcode: "MED-890102", minStockThreshold: 50 },
  { id: 3, name: "Ibuprofen 400mg", brand: "Brufen", strength: "400 mg", dosageForm: "Tablet", category: "NSAID", barcode: "MED-890103", minStockThreshold: 80 },
  { id: 4, name: "Cetirizine 10mg", brand: "Zyrtec", strength: "10 mg", dosageForm: "Tablet", category: "Antihistamine", barcode: "MED-890104", minStockThreshold: 60 },
  { id: 5, name: "Metformin 500mg", brand: "Glucophage", strength: "500 mg", dosageForm: "Tablet", category: "Antidiabetic", barcode: "MED-890105", minStockThreshold: 100 },
  { id: 6, name: "Atorvastatin 10mg", brand: "Lipitor", strength: "10 mg", dosageForm: "Tablet", category: "Cardiovascular", barcode: "MED-890106", minStockThreshold: 40 },
  { id: 7, name: "Omeprazole 20mg", brand: "Prilosec", strength: "20 mg", dosageForm: "Capsule", category: "Gastrointestinal", barcode: "MED-890107", minStockThreshold: 50 },
  { id: 8, name: "Azithromycin 500mg", brand: "Zithromax", strength: "500 mg", dosageForm: "Tablet", category: "Antibiotic", barcode: "MED-890108", minStockThreshold: 30 },
  { id: 9, name: "Ciprofloxacin 500mg", brand: "Cipro", strength: "500 mg", dosageForm: "Tablet", category: "Antibiotic", barcode: "MED-890109", minStockThreshold: 40 },
  { id: 10, name: "Pantoprazole 40mg", brand: "Pantocid", strength: "40 mg", dosageForm: "Tablet", category: "Gastrointestinal", barcode: "MED-890110", minStockThreshold: 50 }
];

const INITIAL_BATCHES = [
  { id: 1, medicineId: 1, batchNumber: "PAR-24-001", mfgDate: "2024-01-10", expiryDate: "2026-01-10", quantity: 200, purchaseCost: 1.20, sellingPrice: 2.50, status: "AVAILABLE" },
  { id: 2, medicineId: 1, batchNumber: "PAR-24-002", mfgDate: "2024-03-15", expiryDate: "2026-03-15", quantity: 300, purchaseCost: 1.25, sellingPrice: 2.50, status: "AVAILABLE" },
  { id: 3, medicineId: 1, batchNumber: "PAR-23-OLD", mfgDate: "2023-01-01", expiryDate: "2024-01-01", quantity: 50, purchaseCost: 1.00, sellingPrice: 2.00, status: "EXPIRED" },
  { id: 4, medicineId: 2, batchNumber: "AMX-24-001", mfgDate: "2024-02-01", expiryDate: "2025-08-01", quantity: 150, purchaseCost: 3.50, sellingPrice: 6.00, status: "AVAILABLE" },
  { id: 5, medicineId: 2, batchNumber: "AMX-24-002", mfgDate: "2024-04-10", expiryDate: "2026-04-10", quantity: 100, purchaseCost: 3.60, sellingPrice: 6.00, status: "AVAILABLE" },
  { id: 6, medicineId: 3, batchNumber: "IBU-24-001", mfgDate: "2024-01-20", expiryDate: "2026-06-20", quantity: 180, purchaseCost: 2.10, sellingPrice: 4.00, status: "AVAILABLE" },
  { id: 7, medicineId: 4, batchNumber: "CET-24-001", mfgDate: "2024-05-01", expiryDate: "2026-11-01", quantity: 45, purchaseCost: 0.80, sellingPrice: 1.80, status: "AVAILABLE" },
  { id: 8, medicineId: 5, batchNumber: "MET-24-001", mfgDate: "2024-02-15", expiryDate: "2026-08-15", quantity: 250, purchaseCost: 1.50, sellingPrice: 3.20, status: "AVAILABLE" },
  { id: 9, medicineId: 6, batchNumber: "ATO-24-001", mfgDate: "2024-03-01", expiryDate: "2025-09-01", quantity: 80, purchaseCost: 4.20, sellingPrice: 8.50, status: "AVAILABLE" },
  { id: 10, medicineId: 7, batchNumber: "OME-24-001", mfgDate: "2024-01-10", expiryDate: "2026-01-10", quantity: 120, purchaseCost: 2.80, sellingPrice: 5.50, status: "AVAILABLE" },
  { id: 11, medicineId: 8, batchNumber: "AZI-24-001", mfgDate: "2024-04-01", expiryDate: "2026-04-01", quantity: 60, purchaseCost: 8.00, sellingPrice: 14.00, status: "AVAILABLE" },
  { id: 12, medicineId: 9, batchNumber: "CIP-24-001", mfgDate: "2024-02-20", expiryDate: "2025-10-20", quantity: 70, purchaseCost: 5.00, sellingPrice: 9.50, status: "AVAILABLE" },
  { id: 13, medicineId: 10, batchNumber: "PAN-24-001", mfgDate: "2024-03-10", expiryDate: "2026-03-10", quantity: 90, purchaseCost: 3.10, sellingPrice: 6.20, status: "AVAILABLE" }
];

const INITIAL_SUPPLIERS = [
  { id: 1, name: "Cipla HealthCare Ltd", contactPerson: "Rajesh Sharma", email: "orders@cipla.com", phone: "+91 98200 11223" },
  { id: 2, name: "Sun Pharma Distributors", contactPerson: "Anita Patel", email: "supply@sunpharma.com", phone: "+91 98200 44556" },
  { id: 3, name: "Dr. Reddy's Logistics", contactPerson: "Vikram Rao", email: "b2b@drreddys.com", phone: "+91 98200 77889" }
];

const INITIAL_ORDERS = [
  { id: 1, poNumber: "PO-2026-001", supplierId: 1, orderDate: "2026-10-20", status: "APPROVED", totalAmount: 720.00, items: [{ medicineId: 1, qty: 300, cost: 1.20, batchNo: "PAR-24-003", mfgDate: "2024-06-01", expDate: "2026-12-01" }, { medicineId: 2, qty: 100, cost: 3.60, batchNo: "AMX-24-003", mfgDate: "2024-06-01", expDate: "2026-12-01" }] },
  { id: 2, poNumber: "PO-2026-002", supplierId: 2, orderDate: "2026-10-25", status: "RECEIVED", totalAmount: 420.00, items: [{ medicineId: 3, qty: 200, cost: 2.10, batchNo: "IBU-24-002", mfgDate: "2024-05-01", expDate: "2026-11-01" }] },
  { id: 3, poNumber: "PO-2026-003", supplierId: 3, orderDate: "2026-10-27", status: "DRAFT", totalAmount: 320.00, items: [{ medicineId: 8, qty: 40, cost: 8.00, batchNo: "AZI-24-002", mfgDate: "2024-05-01", expDate: "2026-11-01" }] }
];

const INITIAL_RESTOCK = [
  { id: 1, medicineId: 4, requestedQuantity: 100, requestedBy: 2, requestedByName: "Sarah Staff", reason: "Floor inventory critically low (45 units remaining)", status: "PENDING", date: "2026-10-26" },
  { id: 2, medicineId: 6, requestedQuantity: 60, requestedBy: 2, requestedByName: "Sarah Staff", reason: "High volume statin prescription spike", status: "APPROVED", date: "2026-10-25" }
];

const INITIAL_ADJUSTMENTS = [
  { id: 1, batchId: 3, medicineId: 1, currentQty: 60, proposedQty: 50, diffQty: -10, submittedBy: 2, submittedByName: "Sarah Staff", category: "Expired Quarantine", justification: "Identified expired packaging from back shelf, quarantined immediately.", status: "APPROVED", date: "2026-10-24" },
  { id: 2, batchId: 7, medicineId: 4, currentQty: 50, proposedQty: 45, diffQty: -5, submittedBy: 2, submittedByName: "Sarah Staff", category: "Damaged Stock", justification: "Water seepage on outer blister strips during shelf restocking.", status: "PENDING", date: "2026-10-28" }
];

const INITIAL_MOVEMENTS = [
  { id: 1, timestamp: "2026-10-20 09:15:00", type: "PURCHASE_IN", medicineId: 1, batchId: 2, quantity: 300, balanceAfter: 500, reference: "PO-2026-001 Delivery Receipt" },
  { id: 2, timestamp: "2026-10-22 14:30:00", type: "SALE_OUT", medicineId: 1, batchId: 1, quantity: -20, balanceAfter: 180, reference: "INV-2026-1001 Dispense" },
  { id: 3, timestamp: "2026-10-24 11:00:00", type: "ADJUSTMENT_WRITE_OFF", medicineId: 1, batchId: 3, quantity: -10, balanceAfter: 50, reference: "Audit Write-off Expired Quarantined" }
];

const INITIAL_USERS = [
  { id: 1, username: "admin", fullName: "System Administrator", email: "admin@pharmacy.com", role: "ADMINISTRATOR", active: true },
  { id: 2, username: "employee1", fullName: "Sarah Staff", email: "sarah.staff@pharmacy.com", role: "EMPLOYEE", active: true },
  { id: 3, username: "employee2", fullName: "Rahul Verma", email: "rahul.v@pharmacy.com", role: "EMPLOYEE", active: true }
];

const INITIAL_SETTINGS = {
  pharmacyName: "MediFind Central Pharmacy & Dispensary",
  gstin: "27AABCU9603R1ZM",
  dlNo: "DL-20B-MH-49201",
  address: "108 Healthcare Blvd, Medical District, Suite 400 | Phone: (555) 234-5678",
  currency: "$",
  defaultGstRate: 12,
  nearExpiryHorizonDays: 90
};

const INITIAL_SALES = [
  {
    invoiceNumber: "INV-2026-1001",
    timestamp: "2026-10-26 11:20:00",
    customerName: "Rahul Sharma",
    customerPhone: "+91 98765 11223",
    doctorRef: "Dr. A. Verma",
    paymentMethod: "UPI",
    items: [{ medicineId: 1, medicineName: "Paracetamol 500mg", batchNumber: "PAR-24-001", expiryDate: "2026-01-10", qty: 20, price: 2.50, subtotal: 50.00 }],
    subtotal: 50.00,
    gstRate: 12,
    cgst: 3.00,
    sgst: 3.00,
    grandTotal: 56.00,
    dispensedBy: "Sarah Staff"
  }
];

const INITIAL_NOTIFICATIONS = [
  { id: 1, title: "Critical Stock Warning", message: "Cetirizine 10mg is below threshold (45 left, minimum is 60).", type: "warning", time: "1 hour ago", read: false },
  { id: 2, title: "Expired Batch Quarantined", message: "Batch PAR-23-OLD expired on 2024-01-01 and cannot be sold.", type: "danger", time: "2 hours ago", read: false },
  { id: 3, title: "Stock Adjustment Pending", message: "Sarah Staff submitted a -5 unit adjustment on Cetirizine 10mg.", type: "info", time: "3 hours ago", read: false }
];

// ============================================================================
// MediFind Application Controller
// ============================================================================
class MediFindApp {
  constructor() {
    this.currentDate = "2026-10-28";
    this.currentUser = INITIAL_USERS[0]; // Default: admin

    this.medicines = [];
    this.batches = [];
    this.suppliers = [];
    this.orders = [];
    this.restock = [];
    this.adjustments = [];
    this.movements = [];
    this.audit = [];
    this.users = [];
    this.sales = [];
    this.notifications = [];
    this.settings = { ...INITIAL_SETTINGS };

    this.posCart = [];
    this.activeTab = "dashboard";

    this.chartCategory = null;
    this.chartExpiry = null;

    this.init();
  }

  init() {
    this.loadState();
    this.bindEvents();
    this.updateUserContextUI();
    this.switchTab("dashboard");
    this.renderAll();
    this.checkHealthAndSync();
  }

  // --------------------------------------------------------------------------
  // Persistence & State Sync
  // --------------------------------------------------------------------------
  loadState() {
    try {
      const saved = localStorage.getItem("medifind_enterprise_v2");
      if (saved) {
        const data = JSON.parse(saved);
        this.medicines = data.medicines || INITIAL_MEDICINES;
        this.batches = data.batches || INITIAL_BATCHES;
        this.suppliers = data.suppliers || INITIAL_SUPPLIERS;
        this.orders = data.orders || INITIAL_ORDERS;
        this.restock = data.restock || INITIAL_RESTOCK;
        this.adjustments = data.adjustments || INITIAL_ADJUSTMENTS;
        this.movements = data.movements || INITIAL_MOVEMENTS;
        this.users = data.users || INITIAL_USERS;
        this.sales = data.sales || INITIAL_SALES;
        this.notifications = data.notifications || INITIAL_NOTIFICATIONS;
        this.settings = data.settings || INITIAL_SETTINGS;
        return;
      }
    } catch (e) {
      console.warn("Storage load error:", e);
    }

    this.medicines = [...INITIAL_MEDICINES];
    this.batches = [...INITIAL_BATCHES];
    this.suppliers = [...INITIAL_SUPPLIERS];
    this.orders = [...INITIAL_ORDERS];
    this.restock = [...INITIAL_RESTOCK];
    this.adjustments = [...INITIAL_ADJUSTMENTS];
    this.movements = [...INITIAL_MOVEMENTS];
    this.users = [...INITIAL_USERS];
    this.sales = [...INITIAL_SALES];
    this.notifications = [...INITIAL_NOTIFICATIONS];
    this.settings = { ...INITIAL_SETTINGS };
    this.saveState();
  }

  saveState() {
    try {
      const payload = {
        medicines: this.medicines,
        batches: this.batches,
        suppliers: this.suppliers,
        orders: this.orders,
        restock: this.restock,
        adjustments: this.adjustments,
        movements: this.movements,
        users: this.users,
        sales: this.sales,
        notifications: this.notifications,
        settings: this.settings
      };
      localStorage.setItem("medifind_enterprise_v2", JSON.stringify(payload));
    } catch (e) {
      console.error("Storage save failed:", e);
    }
  }

  async checkHealthAndSync() {
    try {
      const res = await fetch("/api/status");
      if (res.ok) {
        const stat = await res.json();
        console.log("[C++ Server Bridge] Connected to authoritative engine:", stat);
      }
    } catch (e) {
      // Local fallback active
    }
  }

  // --------------------------------------------------------------------------
  // Role & Permission Management
  // --------------------------------------------------------------------------
  isAdmin() {
    return this.currentUser && this.currentUser.role === "ADMINISTRATOR";
  }

  isEmployee() {
    return this.currentUser && this.currentUser.role === "EMPLOYEE";
  }

  switchUser(user) {
    this.currentUser = user;
    this.updateUserContextUI();
    this.showToast(`Switched active persona to ${user.fullName} (${user.role})`, "info");
    this.closeModal("modalAuth");
    this.switchTab("dashboard");
  }

  updateUserContextUI() {
    const isAdm = this.isAdmin();

    // User header in sidebar
    document.getElementById("userName").textContent = this.currentUser.fullName;
    document.getElementById("userAvatar").textContent = this.currentUser.fullName.charAt(0);
    const roleBadge = document.getElementById("userRoleBadge");
    roleBadge.textContent = this.currentUser.role;
    roleBadge.className = `role-pill ${isAdm ? "admin" : "employee"}`;

    // Show/hide sidebar nav groups and items based on role requirements
    document.querySelectorAll("[data-role-req]").forEach(el => {
      const req = el.getAttribute("data-role-req");
      if (req === "ADMINISTRATOR") {
        el.style.display = isAdm ? "" : "none";
      } else if (req === "EMPLOYEE") {
        el.style.display = !isAdm ? "" : "none";
      }
    });

    // Dashboard banner / widgets
    const adminGrid = document.getElementById("adminKpiGrid");
    const employeeBanner = document.getElementById("employeeShiftBanner");
    if (adminGrid) adminGrid.style.display = isAdm ? "grid" : "none";
    if (employeeBanner) employeeBanner.style.display = isAdm ? "none" : "block";

    // Update pending counters
    const pendingAdjCount = this.adjustments.filter(a => a.status === "PENDING").length;
    const badgeAdj = document.getElementById("badgePendingAdjustments");
    if (badgeAdj) {
      if (isAdm && pendingAdjCount > 0) {
        badgeAdj.textContent = pendingAdjCount;
        badgeAdj.style.display = "inline-block";
      } else {
        badgeAdj.style.display = "none";
      }
    }

    // Profile Tab details
    const pName = document.getElementById("profileFullName");
    const pUser = document.getElementById("profileUsername");
    const pRole = document.getElementById("profileRoleBadge");
    const pEmail = document.getElementById("profileEmail");
    const pAv = document.getElementById("profileLargeAvatar");
    const pPerms = document.getElementById("profilePermissions");

    if (pName) pName.textContent = this.currentUser.fullName;
    if (pUser) pUser.textContent = `Username: ${this.currentUser.username}`;
    if (pRole) {
      pRole.textContent = this.currentUser.role;
      pRole.className = `role-pill ${isAdm ? "admin" : "employee"}`;
    }
    if (pEmail) pEmail.value = this.currentUser.email;
    if (pAv) pAv.textContent = this.currentUser.fullName.charAt(0);
    if (pPerms) {
      pPerms.innerHTML = isAdm
        ? `<strong>Administrative Privileges:</strong><br>&bull; Full Inventory Catalog & Multi-Batch Management<br>&bull; Stock Adjustment & Reconciliation Approvals<br>&bull; Purchase Order Creation & Supplier Contract Management<br>&bull; System User Provisioning & Regulatory Audit Ledger Access<br>&bull; GST & Store Parameter Configuration`
        : `<strong>Operational Staff Privileges:</strong><br>&bull; Point of Sale (POS) Dispensing with Automated FEFO<br>&bull; Real-time Medicine Stock & Expiry Lookups<br>&bull; Inbound Delivery Receipt Verification against Approved POs<br>&bull; Stock Adjustment Request Submission (requires Admin sign-off)<br>&bull; Floor Replenishment & Restock Pipeline Submissions`;
    }
  }

  // --------------------------------------------------------------------------
  // Navigation & Routing
  // --------------------------------------------------------------------------
  switchTab(tabId) {
    // Permission Verification in Application Logic
    const adminOnlyTabs = ["movements", "orders", "sales-customers", "reports", "audit", "users", "settings"];
    if (adminOnlyTabs.includes(tabId) && !this.isAdmin()) {
      this.showToast(`Access Restricted: Page requires Administrator clearance.`, "danger");
      return;
    }

    if (tabId === "my-transactions" && this.isAdmin()) {
      tabId = "sales-customers";
    }

    this.activeTab = tabId;

    // Update Sidebar Active Class
    document.querySelectorAll(".nav-item").forEach(item => {
      item.classList.toggle("active", item.dataset.tab === tabId);
    });

    // Update Tab Viewports
    document.querySelectorAll(".tab-panel").forEach(panel => {
      panel.classList.toggle("active", panel.id === `tab-${tabId}`);
    });

    // Update Titles
    const titles = {
      dashboard: ["Operations Dashboard", "Real-time pharmacy inventory telemetry and FEFO automated allocation"],
      notifications: ["System Notifications", "Critical alerts, safety warnings, and pending operational requests"],
      inventory: ["Medicine Catalog", "Authoritative medicine registry, barcoding, and category filters"],
      batches: ["Batch & Expiry Management", "FEFO date-sorted batch queue with quarantine status controls"],
      movements: ["Stock Movements Ledger", "Complete immutable record of all incoming, outgoing, and adjusted units"],
      pos: ["Point of Sale (POS) & Dispense", "FEFO batch allocation, customer invoicing, and GST calculation"],
      receiving: ["Inbound Stock Receiving", "Receive and verify physical deliveries matching approved Purchase Orders"],
      adjustments: ["Stock Adjustment Requests", "Reconciliation pipeline requiring mandatory justification and Admin approval"],
      restock: ["Floor Restock Pipeline", "Replenishment request submissions and fulfillment tracking"],
      orders: ["Suppliers & Purchase Orders", "Supplier directory, procurement lifecycle, and order authorization"],
      "sales-customers": ["Sales & Customer Ledger", "Customer purchase histories, invoice reprints, and tax summaries"],
      "my-transactions": ["My Shift Transactions", "Personal sales and dispensings processed during your active session"],
      reports: ["Reports & Inventory Valuation", "Financial book valuation, retail revenue forecast, and regulatory audits"],
      audit: ["Regulatory Audit Trail", "Immutable compliance ledger tracking all user actions and changes"],
      users: ["User Management", "Staff accounts, role assignments, and authentication status"],
      settings: ["Pharmacy Settings", "Store profile, GSTIN, Drug License numbers, and tax configuration"],
      profile: ["My Profile", "User credentials, active shift telemetry, and permission manifest"]
    };

    if (titles[tabId]) {
      document.getElementById("pageTitle").textContent = titles[tabId][0];
      document.getElementById("pageSubtitle").textContent = titles[tabId][1];
    }

    // Refresh charts or sub-views if needed
    if (tabId === "dashboard") {
      this.renderDashboard();
    } else if (tabId === "inventory") {
      this.renderMedicinesTable();
    } else if (tabId === "batches") {
      this.renderBatchesTable();
    } else if (tabId === "pos") {
      this.refreshPosCatalog();
    } else if (tabId === "receiving") {
      this.renderReceivingPoSelect();
    } else if (tabId === "adjustments") {
      this.renderAdjustmentRequestsTable();
    } else if (tabId === "restock") {
      this.renderRestockTable();
    } else if (tabId === "orders") {
      this.renderOrdersTable();
      this.renderSuppliersTable();
    } else if (tabId === "sales-customers") {
      this.renderSalesLedgerTable();
    } else if (tabId === "my-transactions") {
      this.renderMyTransactionsTable();
    } else if (tabId === "movements") {
      this.renderFullMovementsTable();
    } else if (tabId === "audit") {
      this.renderAuditTable();
    } else if (tabId === "users") {
      this.renderUsersTable();
    } else if (tabId === "notifications") {
      this.renderNotificationsList();
    }

    // Mobile sidebar auto-close
    const sidebar = document.getElementById("appSidebar");
    if (sidebar && sidebar.classList.contains("mobile-open")) {
      sidebar.classList.remove("mobile-open");
    }
  }

  // --------------------------------------------------------------------------
  // Notifications System
  // --------------------------------------------------------------------------
  addNotification(title, message, type = "info") {
    const notif = {
      id: Date.now(),
      title,
      message,
      type,
      time: "Just now",
      read: false
    };
    this.notifications.unshift(notif);
    this.saveState();
    this.updateNotificationBadges();
  }

  updateNotificationBadges() {
    const unread = this.notifications.filter(n => !n.read).length;
    const topBadge = document.getElementById("topNotifBadge");
    const sideBadge = document.getElementById("sidebarNotifBadge");

    if (topBadge) {
      topBadge.textContent = unread;
      topBadge.style.display = unread > 0 ? "inline-block" : "none";
    }
    if (sideBadge) {
      sideBadge.textContent = unread;
      sideBadge.style.display = unread > 0 ? "inline-block" : "none";
    }
  }

  renderNotificationsList() {
    const container = document.getElementById("fullNotificationList");
    if (!container) return;
    container.innerHTML = "";

    if (this.notifications.length === 0) {
      container.innerHTML = `<div class="text-muted p-4 text-center">No notifications in center.</div>`;
      return;
    }

    this.notifications.forEach(n => {
      const div = document.createElement("div");
      div.className = `notif-item ${n.type}`;
      div.innerHTML = `
        <strong>${this.escapeHtml(n.title)}</strong>
        <span>${this.escapeHtml(n.message)}</span>
        <span class="notif-time">${n.time}</span>
      `;
      container.appendChild(div);
    });
  }

  renderNotificationDrawer() {
    const container = document.getElementById("drawerNotifBody");
    if (!container) return;
    container.innerHTML = "";

    if (this.notifications.length === 0) {
      container.innerHTML = `<div class="text-muted p-4 text-center">No new notifications.</div>`;
      return;
    }

    this.notifications.slice(0, 15).forEach(n => {
      const div = document.createElement("div");
      div.className = `notif-item ${n.type}`;
      div.innerHTML = `
        <strong>${this.escapeHtml(n.title)}</strong>
        <span>${this.escapeHtml(n.message)}</span>
        <span class="notif-time">${n.time}</span>
      `;
      container.appendChild(div);
    });
  }

  // --------------------------------------------------------------------------
  // Inventory & FEFO Queries
  // --------------------------------------------------------------------------
  getTotalStock(medId) {
    return this.batches
      .filter(b => b.medicineId === medId && b.status !== "EXPIRED" && b.expiryDate >= this.currentDate)
      .reduce((sum, b) => sum + b.quantity, 0);
  }

  getEligibleBatchesForMedicine(medId) {
    return this.batches
      .filter(b => b.medicineId === medId && b.status !== "EXPIRED" && b.expiryDate >= this.currentDate && b.quantity > 0)
      .sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate)); // FEFO Order
  }

  // --------------------------------------------------------------------------
  // Dashboard Metrics & Charts
  // --------------------------------------------------------------------------
  renderDashboard() {
    const activeMeds = this.medicines.length;
    const activeBatches = this.batches.filter(b => b.status !== "EXPIRED" && b.quantity > 0).length;
    const totalUnits = this.batches.reduce((sum, b) => sum + (b.status !== "EXPIRED" && b.expiryDate >= this.currentDate ? b.quantity : 0), 0);

    let costVal = 0;
    let retailVal = 0;
    this.batches.forEach(b => {
      if (b.status !== "EXPIRED" && b.expiryDate >= this.currentDate) {
        costVal += (b.quantity * b.purchaseCost);
        costVal = Math.round(costVal * 100) / 100;
        retailVal += (b.quantity * b.sellingPrice);
        retailVal = Math.round(retailVal * 100) / 100;
      }
    });

    const lowStockCount = this.medicines.filter(m => this.getTotalStock(m.id) <= m.minStockThreshold).length;
    const expiredCount = this.batches.filter(b => b.status === "EXPIRED" || b.expiryDate < this.currentDate).length;

    const elMeds = document.getElementById("kpiMeds");
    const elBatches = document.getElementById("kpiBatches");
    const elUnits = document.getElementById("kpiUnits");
    const elCost = document.getElementById("kpiCost");
    const elRetail = document.getElementById("kpiRetail");
    const elAlerts = document.getElementById("kpiAlerts");
    const elAlertsDetail = document.getElementById("kpiAlertsDetail");

    if (elMeds) elMeds.textContent = activeMeds;
    if (elBatches) elBatches.textContent = activeBatches;
    if (elUnits) elUnits.textContent = totalUnits.toLocaleString();
    if (elCost) elCost.textContent = `${this.settings.currency}${costVal.toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;
    if (elRetail) elRetail.textContent = `${this.settings.currency}${retailVal.toLocaleString(undefined, { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;
    if (elAlerts) elAlerts.textContent = lowStockCount + expiredCount;
    if (elAlertsDetail) elAlertsDetail.textContent = `${lowStockCount} low stock, ${expiredCount} expired/quarantined`;

    // Alerts Banner
    const alertBox = document.getElementById("dashboardAlertContainer");
    if (alertBox) {
      alertBox.innerHTML = "";
      if (expiredCount > 0) {
        alertBox.innerHTML += `
          <div style="background:#fee2e2; border-left:4px solid #dc2626; color:#991b1b; padding:12px 16px; border-radius:6px; margin-bottom:12px; display:flex; justify-content:space-between; align-items:center;">
            <div><strong>Urgent Expiry Hazard:</strong> ${expiredCount} batch(es) have expired and are safely quarantined by FEFO rules.</div>
            <button class="btn btn-sm btn-danger" onclick="window.mediApp.switchTab('batches')">Review Batches</button>
          </div>
        `;
      }
      if (lowStockCount > 0) {
        alertBox.innerHTML += `
          <div style="background:#fef3c7; border-left:4px solid #d97706; color:#92400e; padding:12px 16px; border-radius:6px; margin-bottom:12px; display:flex; justify-content:space-between; align-items:center;">
            <div><strong>Low Stock Warning:</strong> ${lowStockCount} medicine(s) are at or below minimum threshold.</div>
            <button class="btn btn-sm btn-secondary" onclick="window.mediApp.switchTab('restock')">Restock Pipeline</button>
          </div>
        `;
      }
    }

    // FEFO Preview Table
    this.renderFefoQueueTable();
    // Movements Table
    this.renderRecentMovementsTable();
    // Render Analytics Charts
    this.renderCharts();
  }

  renderFefoQueueTable() {
    const tbody = document.querySelector("#tblFefoPreview tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const activeBatches = this.batches.filter(b => b.quantity > 0);
    const sorted = [...activeBatches].sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate)).slice(0, 8);

    sorted.forEach(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const isExpired = b.expiryDate < this.currentDate;
      const daysLeft = Math.ceil((new Date(b.expiryDate) - new Date(this.currentDate)) / (1000 * 60 * 60 * 24));

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>${med ? this.escapeHtml(med.name) : "Unknown"}</strong></td>
        <td><code>${this.escapeHtml(b.batchNumber)}</code></td>
        <td>${b.expiryDate}</td>
        <td>${isExpired ? '<span class="badge badge-danger">EXPIRED</span>' : (daysLeft <= 90 ? `<span class="badge badge-warning">${daysLeft} days</span>` : `<span class="badge badge-success">${daysLeft} days</span>`)}</td>
        <td><strong>${b.quantity}</strong> units</td>
        <td>${this.settings.currency}${b.purchaseCost.toFixed(2)}</td>
        <td>${isExpired ? '<span class="status-pill expired">Quarantined</span>' : '<span class="status-pill available">Next in FEFO</span>'}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderRecentMovementsTable() {
    const tbody = document.querySelector("#tblRecentMovements tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const recent = [...this.movements].slice(-7).reverse();
    recent.forEach(m => {
      const med = this.medicines.find(x => x.id === m.medicineId);
      const batch = this.batches.find(x => x.id === m.batchId);
      const isPositive = m.quantity > 0;

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td style="font-size:12px; color:var(--text-muted);">${m.timestamp}</td>
        <td><span class="badge ${isPositive ? 'badge-success' : 'badge-danger'}">${m.type}</span></td>
        <td>${med ? this.escapeHtml(med.name) : `#${m.medicineId}`}</td>
        <td><code>${batch ? this.escapeHtml(batch.batchNumber) : '-'}</code></td>
        <td><strong style="color:${isPositive ? 'var(--success)' : 'var(--danger)'}">${isPositive ? '+' : ''}${m.quantity}</strong></td>
        <td>${m.balanceAfter !== undefined ? m.balanceAfter : '-'}</td>
        <td style="font-size:12px;">${this.escapeHtml(m.reference || '')}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderCharts() {
    // 1. Therapeutic Category Chart
    const catCounts = {};
    this.medicines.forEach(m => {
      catCounts[m.category] = (catCounts[m.category] || 0) + this.getTotalStock(m.id);
    });

    const catCtx = document.getElementById("chartCategory");
    if (catCtx) {
      if (this.chartCategory) this.chartCategory.destroy();
      this.chartCategory = new Chart(catCtx, {
        type: "doughnut",
        data: {
          labels: Object.keys(catCounts),
          datasets: [{
            data: Object.values(catCounts),
            backgroundColor: ["#0d9488", "#06b6d4", "#3b82f6", "#6366f1", "#10b981", "#f59e0b", "#ec4899", "#8b5cf6"],
            borderWidth: 2,
            borderColor: "#ffffff"
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          plugins: {
            legend: { position: "right", labels: { boxWidth: 12, font: { size: 11, family: "Plus Jakarta Sans" } } }
          }
        }
      });
    }

    // 2. Expiry Timeline Radar
    const expCtx = document.getElementById("chartExpiryTimeline");
    if (expCtx) {
      const activeBatches = this.batches.filter(b => b.quantity > 0);
      const sorted = [...activeBatches].sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate)).slice(0, 7);

      const labels = sorted.map(b => {
        const m = this.medicines.find(x => x.id === b.medicineId);
        return m ? `${m.name.split(" ")[0]} (${b.batchNumber})` : b.batchNumber;
      });
      const dataDays = sorted.map(b => {
        return Math.max(0, Math.ceil((new Date(b.expiryDate) - new Date(this.currentDate)) / (1000 * 60 * 60 * 24)));
      });

      if (this.chartExpiry) this.chartExpiry.destroy();
      this.chartExpiry = new Chart(expCtx, {
        type: "bar",
        data: {
          labels,
          datasets: [{
            label: "Days until Expiration",
            data: dataDays,
            backgroundColor: dataDays.map(d => d === 0 ? "#dc2626" : (d <= 90 ? "#f59e0b" : "#0d9488")),
            borderRadius: 4
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            y: { beginAtZero: true, title: { display: true, text: "Days Remaining" } }
          },
          plugins: {
            legend: { display: false }
          }
        }
      });
    }
  }

  // --------------------------------------------------------------------------
  // Medicine Inventory Tab
  // --------------------------------------------------------------------------
  renderMedicinesTable() {
    const tbody = document.querySelector("#tblMedicines tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const searchTxt = (document.getElementById("searchMedicine")?.value || "").toLowerCase();
    const catFilter = document.getElementById("filterCategory")?.value || "";

    // Populate category dropdown
    const catSelect = document.getElementById("filterCategory");
    if (catSelect && catSelect.options.length <= 1) {
      const cats = [...new Set(this.medicines.map(m => m.category))].sort();
      cats.forEach(c => {
        const opt = document.createElement("option");
        opt.value = c;
        opt.textContent = c;
        catSelect.appendChild(opt);
      });
    }

    const filtered = this.medicines.filter(m => {
      const matchSearch = m.name.toLowerCase().includes(searchTxt) ||
                          m.brand.toLowerCase().includes(searchTxt) ||
                          m.barcode.toLowerCase().includes(searchTxt);
      const matchCat = !catFilter || m.category === catFilter;
      return matchSearch && matchCat;
    });

    const isAdm = this.isAdmin();

    filtered.forEach(m => {
      const totalStock = this.getTotalStock(m.id);
      const isLow = totalStock <= m.minStockThreshold;

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>#${m.id}</strong></td>
        <td><strong>${this.escapeHtml(m.name)}</strong></td>
        <td>${this.escapeHtml(m.brand)}</td>
        <td>${this.escapeHtml(m.strength)}</td>
        <td>${this.escapeHtml(m.dosageForm)}</td>
        <td><span class="badge badge-info">${this.escapeHtml(m.category)}</span></td>
        <td><code>${this.escapeHtml(m.barcode)}</code></td>
        <td><strong style="color:${isLow ? 'var(--danger)' : 'var(--success)'}; font-size:14px;">${totalStock}</strong> units</td>
        <td>${m.minStockThreshold}</td>
        <td>
          <div class="btn-group">
            <button class="btn btn-sm btn-outline" onclick="window.mediApp.viewMedicineBatches(${m.id})">Batches</button>
            ${isAdm ? `<button class="btn btn-sm btn-primary" onclick="window.mediApp.openStockInForMed(${m.id})">+ Stock</button>` : ''}
          </div>
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  viewMedicineBatches(medId) {
    this.switchTab("batches");
    const searchInput = document.getElementById("searchBatch");
    const med = this.medicines.find(m => m.id === medId);
    if (searchInput && med) {
      searchInput.value = med.name;
      this.renderBatchesTable();
    }
  }

  openStockInForMed(medId) {
    if (!this.isAdmin()) {
      this.showToast("Direct stock-in is restricted to Administrators.", "danger");
      return;
    }
    const select = document.getElementById("stockInMedSelect");
    if (select) {
      select.innerHTML = "";
      this.medicines.forEach(m => {
        const opt = document.createElement("option");
        opt.value = m.id;
        opt.textContent = `${m.name} (${m.brand})`;
        if (m.id === medId) opt.selected = true;
        select.appendChild(opt);
      });
    }
    this.openModal("modalStockIn");
  }

  // --------------------------------------------------------------------------
  // Batch & FEFO Tab
  // --------------------------------------------------------------------------
  renderBatchesTable() {
    const tbody = document.querySelector("#tblBatches tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const searchTxt = (document.getElementById("searchBatch")?.value || "").toLowerCase();
    const statusFilter = document.getElementById("filterBatchStatus")?.value || "";

    const sorted = [...this.batches].sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate));

    const isAdm = this.isAdmin();

    sorted.forEach(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const isExpired = b.expiryDate < this.currentDate;
      const daysLeft = Math.ceil((new Date(b.expiryDate) - new Date(this.currentDate)) / (1000 * 60 * 60 * 24));

      let computedStatus = b.status;
      if (isExpired) computedStatus = "EXPIRED";
      else if (b.quantity === 0) computedStatus = "DEPLETED";
      else if (daysLeft <= 90) computedStatus = "NEAR_EXPIRY";

      if (statusFilter && computedStatus !== statusFilter) return;

      const medName = med ? med.name.toLowerCase() : "";
      if (searchTxt && !b.batchNumber.toLowerCase().includes(searchTxt) && !medName.includes(searchTxt)) {
        return;
      }

      let statusBadge = `<span class="status-pill available">Available</span>`;
      if (computedStatus === "EXPIRED") statusBadge = `<span class="status-pill expired">Expired (Quarantine)</span>`;
      else if (computedStatus === "NEAR_EXPIRY") statusBadge = `<span class="status-pill near-expiry">Near Expiry (${daysLeft}d)</span>`;
      else if (computedStatus === "DEPLETED") statusBadge = `<span class="status-pill depleted">Depleted</span>`;

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>#${b.id}</strong></td>
        <td><strong>${med ? this.escapeHtml(med.name) : 'Unknown'}</strong></td>
        <td><code>${this.escapeHtml(b.batchNumber)}</code></td>
        <td>${b.mfgDate}</td>
        <td><strong>${b.expiryDate}</strong></td>
        <td><strong>${b.quantity}</strong></td>
        <td>${this.settings.currency}${b.purchaseCost.toFixed(2)}</td>
        <td>${this.settings.currency}${b.sellingPrice.toFixed(2)}</td>
        <td>${statusBadge}</td>
        <td>
          <button class="btn btn-sm btn-outline" onclick="window.mediApp.requestBatchAdjustment(${b.id})">Adjust</button>
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  // --------------------------------------------------------------------------
  // Stock Adjustments Pipeline
  // --------------------------------------------------------------------------
  requestBatchAdjustment(batchId) {
    const batch = this.batches.find(b => b.id === batchId);
    if (!batch) return;

    const select = document.getElementById("adjBatchSelect");
    if (select) {
      select.innerHTML = "";
      this.batches.forEach(b => {
        const med = this.medicines.find(m => m.id === b.medicineId);
        const opt = document.createElement("option");
        opt.value = b.id;
        opt.textContent = `${b.batchNumber} - ${med ? med.name : 'Med'} (Qty: ${b.quantity})`;
        if (b.id === batchId) opt.selected = true;
        select.appendChild(opt);
      });
    }

    const curStock = document.getElementById("adjCurrentStock");
    if (curStock) curStock.value = batch.quantity;

    const newCount = document.getElementById("adjNewCount");
    if (newCount) newCount.value = batch.quantity;

    this.openModal("modalAdjustmentRequest");
  }

  renderAdjustmentRequestsTable() {
    const tbody = document.querySelector("#tblAdjustmentRequests tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const pendingCount = this.adjustments.filter(a => a.status === "PENDING").length;
    const countEl = document.getElementById("adjustmentPendingCount");
    if (countEl) countEl.textContent = `Pending Reviews: ${pendingCount}`;

    const isAdm = this.isAdmin();

    this.adjustments.forEach(adj => {
      const batch = this.batches.find(b => b.id === adj.batchId);
      const med = this.medicines.find(m => m.id === adj.medicineId);
      const diffSign = adj.diffQty > 0 ? `+${adj.diffQty}` : `${adj.diffQty}`;

      let statusBadge = `<span class="badge badge-warning">PENDING</span>`;
      if (adj.status === "APPROVED") statusBadge = `<span class="badge badge-success">APPROVED</span>`;
      else if (adj.status === "REJECTED") statusBadge = `<span class="badge badge-danger">REJECTED</span>`;

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>#ADJ-${adj.id}</strong></td>
        <td>${med ? this.escapeHtml(med.name) : '-'}</td>
        <td><code>${batch ? this.escapeHtml(batch.batchNumber) : '-'}</code></td>
        <td>${adj.currentQty}</td>
        <td><strong>${adj.proposedQty}</strong></td>
        <td><strong style="color:${adj.diffQty < 0 ? 'var(--danger)' : 'var(--success)'}">${diffSign}</strong></td>
        <td>${this.escapeHtml(adj.submittedByName || 'Staff')}</td>
        <td style="max-width:220px; font-size:12px;"><strong>${this.escapeHtml(adj.category)}:</strong> ${this.escapeHtml(adj.justification)}</td>
        <td>${statusBadge}</td>
        <td>
          ${isAdm && adj.status === "PENDING" ? `
            <div class="btn-group">
              <button class="btn btn-sm btn-success" onclick="window.mediApp.approveAdjustment(${adj.id})">Approve</button>
              <button class="btn btn-sm btn-danger" onclick="window.mediApp.rejectAdjustment(${adj.id})">Reject</button>
            </div>
          ` : `<span class="text-muted" style="font-size:11px;">Completed</span>`}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  approveAdjustment(adjId) {
    if (!this.isAdmin()) {
      this.showToast("Only an Administrator can approve stock adjustments.", "danger");
      return;
    }
    const adj = this.adjustments.find(a => a.id === adjId);
    if (!adj || adj.status !== "PENDING") return;

    const batch = this.batches.find(b => b.id === adj.batchId);
    if (!batch) return;

    // Apply count mutation
    const oldQty = batch.quantity;
    batch.quantity = adj.proposedQty;
    if (batch.quantity === 0) batch.status = "DEPLETED";
    adj.status = "APPROVED";

    // Record Stock Movement
    this.movements.push({
      id: this.movements.length + 1,
      timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
      type: "ADJUSTMENT_WRITE_OFF",
      medicineId: adj.medicineId,
      batchId: adj.batchId,
      quantity: adj.diffQty,
      balanceAfter: batch.quantity,
      reference: `ADJ-${adj.id} Approved: ${adj.category} (${adj.justification})`
    });

    this.saveState();
    this.showToast(`Adjustment #ADJ-${adj.id} approved. Inventory reconciled to ${batch.quantity} units.`, "success");
    this.renderAdjustmentRequestsTable();
    this.updateUserContextUI();
  }

  rejectAdjustment(adjId) {
    if (!this.isAdmin()) {
      this.showToast("Only an Administrator can reject adjustments.", "danger");
      return;
    }
    const adj = this.adjustments.find(a => a.id === adjId);
    if (!adj) return;
    adj.status = "REJECTED";
    this.saveState();
    this.showToast(`Adjustment #ADJ-${adj.id} rejected. No stock modified.`, "warning");
    this.renderAdjustmentRequestsTable();
  }

  // --------------------------------------------------------------------------
  // Point of Sale (POS) & GST Invoicing Engine
  // --------------------------------------------------------------------------
  refreshPosCatalog() {
    const select = document.getElementById("posSelectMed");
    if (!select) return;
    select.innerHTML = "";

    this.medicines.forEach(m => {
      const stock = this.getTotalStock(m.id);
      const opt = document.createElement("option");
      opt.value = m.id;
      opt.textContent = `${m.name} (${m.brand}) — Stock: ${stock} units`;
      select.appendChild(opt);
    });

    this.updatePosSelectedMedStock();
  }

  updatePosSelectedMedStock() {
    const select = document.getElementById("posSelectMed");
    if (!select || !select.value) return;
    const medId = parseInt(select.value, 10);
    const stock = this.getTotalStock(medId);
    const stockInput = document.getElementById("posAvailableStock");
    if (stockInput) stockInput.value = `${stock} units`;

    this.previewFefoAllocation();
  }

  previewFefoAllocation() {
    const select = document.getElementById("posSelectMed");
    const qtyInput = document.getElementById("posQuantity");
    const previewBox = document.getElementById("posFefoExplanation");
    if (!select || !qtyInput || !previewBox) return;

    const medId = parseInt(select.value, 10);
    const reqQty = parseInt(qtyInput.value, 10) || 0;
    const med = this.medicines.find(m => m.id === medId);

    if (!med || reqQty <= 0) {
      previewBox.innerHTML = `Select an item and quantity to preview allocation.`;
      return;
    }

    const batches = this.getEligibleBatchesForMedicine(medId);
    const totalAvail = batches.reduce((sum, b) => sum + b.quantity, 0);

    if (totalAvail < reqQty) {
      previewBox.innerHTML = `<span class="text-danger"><strong>Insufficient Valid Stock:</strong> Only ${totalAvail} units available (excluding expired batches). Cannot allocate ${reqQty} units.</span>`;
      return;
    }

    let remaining = reqQty;
    const allocationLines = [];

    for (let b of batches) {
      if (remaining <= 0) break;
      const take = Math.min(b.quantity, remaining);
      allocationLines.push(`&bull; Take <strong>${take}</strong> units from Batch <code>${b.batchNumber}</code> (Expires: <strong>${b.expiryDate}</strong>, Rate: ${this.settings.currency}${b.sellingPrice.toFixed(2)})`);
      remaining -= take;
    }

    previewBox.innerHTML = `<strong>FEFO Allocation Breakdown:</strong><br>${allocationLines.join("<br>")}`;
  }

  handlePosAddToCart() {
    const select = document.getElementById("posSelectMed");
    const qtyInput = document.getElementById("posQuantity");
    if (!select || !qtyInput) return;

    const medId = parseInt(select.value, 10);
    const reqQty = parseInt(qtyInput.value, 10);

    if (isNaN(reqQty) || reqQty <= 0) {
      this.showToast("Enter a valid quantity greater than 0.", "warning");
      return;
    }

    const med = this.medicines.find(m => m.id === medId);
    const eligibleBatches = this.getEligibleBatchesForMedicine(medId);

    // Calculate how many already committed in cart for each batch
    const committedPerBatch = {};
    this.posCart.forEach(item => {
      committedPerBatch[item.batchId] = (committedPerBatch[item.batchId] || 0) + item.qty;
    });

    let remaining = reqQty;
    let addedItems = [];

    for (let b of eligibleBatches) {
      if (remaining <= 0) break;
      const alreadyCommitted = committedPerBatch[b.id] || 0;
      const availableInBatch = b.quantity - alreadyCommitted;
      if (availableInBatch <= 0) continue;

      const take = Math.min(availableInBatch, remaining);

      // Check if item for this batch already in cart
      const existing = this.posCart.find(c => c.batchId === b.id);
      if (existing) {
        existing.qty += take;
        existing.subtotal = existing.qty * existing.unitPrice;
      } else {
        this.posCart.push({
          medicineId: med.id,
          medicineName: med.name,
          batchId: b.id,
          batchNumber: b.batchNumber,
          expiryDate: b.expiryDate,
          qty: take,
          unitPrice: b.sellingPrice,
          subtotal: take * b.sellingPrice
        });
      }

      committedPerBatch[b.id] = (committedPerBatch[b.id] || 0) + take;
      remaining -= take;
    }

    if (remaining > 0) {
      this.showToast(`Could only allocate part of request (${reqQty - remaining}/${reqQty} units). Remainder exceeds available stock.`, "warning");
    } else {
      this.showToast(`Allocated ${reqQty} units of ${med.name} via FEFO rules.`, "success");
    }

    this.renderPosCart();
  }

  renderPosCart() {
    const tbody = document.querySelector("#tblPosCart tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    let subtotal = 0;

    this.posCart.forEach((item, idx) => {
      subtotal += item.subtotal;
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>${this.escapeHtml(item.medicineName)}</strong></td>
        <td><code>${this.escapeHtml(item.batchNumber)}</code></td>
        <td>${item.expiryDate}</td>
        <td><strong>${item.qty}</strong></td>
        <td>${this.settings.currency}${item.unitPrice.toFixed(2)}</td>
        <td><strong>${this.settings.currency}${item.subtotal.toFixed(2)}</strong></td>
        <td><button class="btn btn-sm btn-outline text-danger" onclick="window.mediApp.removePosCartItem(${idx})">&times;</button></td>
      `;
      tbody.appendChild(tr);
    });

    // GST Calculation
    const gstRateSelect = document.getElementById("posGstRate");
    const gstRate = parseFloat(gstRateSelect ? gstRateSelect.value : 12);
    const halfRate = gstRate / 2;

    const cgst = Math.round((subtotal * (halfRate / 100)) * 100) / 100;
    const sgst = Math.round((subtotal * (halfRate / 100)) * 100) / 100;
    const grandTotal = Math.round((subtotal + cgst + sgst) * 100) / 100;

    const elSub = document.getElementById("posSubtotal");
    const elCgstLabel = document.getElementById("posCgstLabel");
    const elCgst = document.getElementById("posCgstAmount");
    const elSgstLabel = document.getElementById("posSgstLabel");
    const elSgst = document.getElementById("posSgstAmount");
    const elGrand = document.getElementById("posGrandTotal");

    if (elSub) elSub.textContent = `${this.settings.currency}${subtotal.toFixed(2)}`;
    if (elCgstLabel) elCgstLabel.textContent = `CGST (${halfRate}%):`;
    if (elCgst) elCgst.textContent = `${this.settings.currency}${cgst.toFixed(2)}`;
    if (elSgstLabel) elSgstLabel.textContent = `SGST (${halfRate}%):`;
    if (elSgst) elSgst.textContent = `${this.settings.currency}${sgst.toFixed(2)}`;
    if (elGrand) elGrand.textContent = `${this.settings.currency}${grandTotal.toFixed(2)}`;
  }

  removePosCartItem(idx) {
    this.posCart.splice(idx, 1);
    this.renderPosCart();
  }

  clearPosCart() {
    this.posCart = [];
    this.renderPosCart();
  }

  handlePosCheckout() {
    if (this.posCart.length === 0) {
      this.showToast("Cart is empty. Add medicines first.", "warning");
      return;
    }

    const customerName = document.getElementById("posCustomerName")?.value || "Walk-in Customer";
    const customerPhone = document.getElementById("posCustomerPhone")?.value || "";
    const doctorRef = document.getElementById("posDoctorRef")?.value || "General Dispensary";
    const paymentMethod = document.getElementById("posPaymentMethod")?.value || "CASH";
    const gstRate = parseFloat(document.getElementById("posGstRate")?.value || 12);

    // Verify stock availability
    for (let item of this.posCart) {
      const b = this.batches.find(x => x.id === item.batchId);
      if (!b || b.quantity < item.qty) {
        this.showToast(`Batch ${item.batchNumber} has insufficient stock to fulfill sale.`, "danger");
        return;
      }
      if (b.expiryDate < this.currentDate) {
        this.showToast(`Safety Violation: Batch ${item.batchNumber} is expired. Dispensing halted.`, "danger");
        return;
      }
    }

    // Process deduction
    const invoiceNum = `INV-${Date.now().toString().slice(-6)}`;
    const nowTime = new Date().toISOString().replace('T', ' ').substring(0, 19);

    let subtotal = 0;
    this.posCart.forEach(item => {
      subtotal += item.subtotal;
      const b = this.batches.find(x => x.id === item.batchId);
      b.quantity -= item.qty;
      if (b.quantity === 0) b.status = "DEPLETED";

      // Log Movement
      this.movements.push({
        id: this.movements.length + 1,
        timestamp: nowTime,
        type: "SALE_OUT",
        medicineId: item.medicineId,
        batchId: item.batchId,
        quantity: -item.qty,
        balanceAfter: b.quantity,
        reference: `${invoiceNum} Dispense to ${customerName}`
      });
    });

    const halfRate = gstRate / 2;
    const cgst = Math.round((subtotal * (halfRate / 100)) * 100) / 100;
    const sgst = Math.round((subtotal * (halfRate / 100)) * 100) / 100;
    const grandTotal = Math.round((subtotal + cgst + sgst) * 100) / 100;

    const saleRecord = {
      invoiceNumber: invoiceNum,
      timestamp: nowTime,
      customerName,
      customerPhone,
      doctorRef,
      paymentMethod,
      items: [...this.posCart],
      subtotal,
      gstRate,
      cgst,
      sgst,
      grandTotal,
      dispensedBy: this.currentUser.fullName
    };

    this.sales.unshift(saleRecord);
    this.saveState();

    this.showToast(`Sale confirmed! Invoice ${invoiceNum} generated.`, "success");
    this.renderInvoiceModal(saleRecord);
    this.clearPosCart();
    this.updateUserContextUI();
  }

  renderInvoiceModal(sale) {
    const container = document.getElementById("invoiceReceiptBody");
    if (!container) return;

    let itemsHtml = "";
    sale.items.forEach(it => {
      itemsHtml += `
        <tr>
          <td><strong>${this.escapeHtml(it.medicineName)}</strong></td>
          <td><code>${this.escapeHtml(it.batchNumber)}</code></td>
          <td>${it.expiryDate}</td>
          <td>${it.qty}</td>
          <td>${this.settings.currency}${it.unitPrice.toFixed(2)}</td>
          <td>${this.settings.currency}${it.subtotal.toFixed(2)}</td>
        </tr>
      `;
    });

    const halfRate = sale.gstRate / 2;

    container.innerHTML = `
      <div class="gst-invoice">
        <div class="invoice-header">
          <div class="invoice-pharmacy">
            <h2>${this.escapeHtml(this.settings.pharmacyName)}</h2>
            <p>${this.escapeHtml(this.settings.address)}</p>
            <p><strong>GSTIN:</strong> ${this.escapeHtml(this.settings.gstin)} | <strong>DL No:</strong> ${this.escapeHtml(this.settings.dlNo)}</p>
          </div>
          <div class="invoice-meta">
            <p>Tax Invoice</p>
            <strong>${sale.invoiceNumber}</strong>
            <p style="color:var(--text-muted); font-size:11px;">${sale.timestamp}</p>
          </div>
        </div>

        <div class="invoice-customer-bar">
          <div><strong>Patient / Customer:</strong> ${this.escapeHtml(sale.customerName)} ${sale.customerPhone ? `(${this.escapeHtml(sale.customerPhone)})` : ''}</div>
          <div><strong>Doctor Reference:</strong> ${this.escapeHtml(sale.doctorRef)}</div>
          <div><strong>Payment Mode:</strong> ${sale.paymentMethod}</div>
        </div>

        <table class="invoice-table">
          <thead>
            <tr>
              <th>Medicine Description</th>
              <th>Batch #</th>
              <th>Expiry</th>
              <th>Qty</th>
              <th>Unit Rate</th>
              <th>Amount (${this.settings.currency})</th>
            </tr>
          </thead>
          <tbody>
            ${itemsHtml}
          </tbody>
        </table>

        <div class="invoice-tax-summary">
          <div class="row">
            <span>Taxable Subtotal:</span>
            <strong>${this.settings.currency}${sale.subtotal.toFixed(2)}</strong>
          </div>
          <div class="row">
            <span>CGST (${halfRate}%):</span>
            <strong>${this.settings.currency}${sale.cgst.toFixed(2)}</strong>
          </div>
          <div class="row">
            <span>SGST (${halfRate}%):</span>
            <strong>${this.settings.currency}${sale.sgst.toFixed(2)}</strong>
          </div>
          <div class="row grand-total">
            <span>Total Payable:</span>
            <strong>${this.settings.currency}${sale.grandTotal.toFixed(2)}</strong>
          </div>
        </div>

        <div class="invoice-footer-notes">
          <div>
            <p>&bull; Medicines once sold cannot be returned without original batch receipt.</p>
            <p>&bull; Dispensed strictly in compliance with FEFO medical protocol.</p>
          </div>
          <div class="invoice-signature-line">
            <p>Authorized Pharmacist</p>
            <strong>${this.escapeHtml(sale.dispensedBy)}</strong>
          </div>
        </div>
      </div>
    `;

    this.openModal("modalInvoice");
  }

  // --------------------------------------------------------------------------
  // Inbound Stock Receiving
  // --------------------------------------------------------------------------
  renderReceivingPoSelect() {
    const select = document.getElementById("receivingPoSelect");
    if (!select) return;
    select.innerHTML = '<option value="">-- Choose Approved Purchase Order --</option>';

    const approvedPos = this.orders.filter(o => o.status === "APPROVED");
    approvedPos.forEach(po => {
      const sup = this.suppliers.find(s => s.id === po.supplierId);
      const opt = document.createElement("option");
      opt.value = po.id;
      opt.textContent = `${po.poNumber} — Supplier: ${sup ? sup.name : 'Unknown'} ($${po.totalAmount.toFixed(2)})`;
      select.appendChild(opt);
    });

    this.renderReceivingItemsTable();
  }

  renderReceivingItemsTable() {
    const poSelect = document.getElementById("receivingPoSelect");
    const tbody = document.querySelector("#tblReceivingItems tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const poId = parseInt(poSelect?.value, 10);
    const po = this.orders.find(o => o.id === poId);

    if (!po || !po.items || po.items.length === 0) {
      tbody.innerHTML = `<tr><td colspan="8" class="text-muted text-center p-3">Select an approved Purchase Order above to load items.</td></tr>`;
      return;
    }

    po.items.forEach((it, idx) => {
      const med = this.medicines.find(m => m.id === it.medicineId);
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>${med ? this.escapeHtml(med.name) : 'Med #' + it.medicineId}</strong></td>
        <td>${it.qty} units</td>
        <td><input type="number" class="form-input receiving-qty" data-idx="${idx}" value="${it.qty}" style="width:80px;"></td>
        <td><input type="text" class="form-input receiving-batch" data-idx="${idx}" value="${it.batchNo || 'REC-' + Date.now().toString().slice(-4)}" style="width:120px;"></td>
        <td><input type="date" class="form-input receiving-mfg" data-idx="${idx}" value="${it.mfgDate || '2024-06-01'}" style="width:140px;"></td>
        <td><input type="date" class="form-input receiving-exp" data-idx="${idx}" value="${it.expDate || '2026-12-01'}" style="width:140px;"></td>
        <td><input type="number" step="0.01" class="form-input receiving-cost" data-idx="${idx}" value="${it.cost}" style="width:80px;"></td>
        <td><input type="number" step="0.01" class="form-input receiving-retail" data-idx="${idx}" value="${(it.cost * 1.8).toFixed(2)}" style="width:80px;"></td>
      `;
      tbody.appendChild(tr);
    });
  }

  handleSubmitReceiving() {
    const poSelect = document.getElementById("receivingPoSelect");
    const poId = parseInt(poSelect?.value, 10);
    const po = this.orders.find(o => o.id === poId);

    if (!po) {
      this.showToast("Select an approved Purchase Order first.", "warning");
      return;
    }

    const challanNo = document.getElementById("receivingChallanNo")?.value || `DC-${Date.now().toString().slice(-5)}`;
    const qtyInputs = document.querySelectorAll(".receiving-qty");
    const batchInputs = document.querySelectorAll(".receiving-batch");
    const mfgInputs = document.querySelectorAll(".receiving-mfg");
    const expInputs = document.querySelectorAll(".receiving-exp");
    const costInputs = document.querySelectorAll(".receiving-cost");
    const retailInputs = document.querySelectorAll(".receiving-retail");

    let receivedCount = 0;

    po.items.forEach((it, idx) => {
      const qty = parseInt(qtyInputs[idx].value, 10) || 0;
      const batchNo = batchInputs[idx].value.trim() || `BAT-${Date.now()}`;
      const mfgDate = mfgInputs[idx].value || "2024-06-01";
      const expDate = expInputs[idx].value || "2026-12-01";
      const cost = parseFloat(costInputs[idx].value) || it.cost;
      const retail = parseFloat(retailInputs[idx].value) || cost * 1.8;

      if (qty > 0) {
        // Create new batch
        const newBatch = {
          id: this.batches.length + 1,
          medicineId: it.medicineId,
          batchNumber: batchNo,
          mfgDate,
          expiryDate: expDate,
          quantity: qty,
          purchaseCost: cost,
          sellingPrice: retail,
          status: "AVAILABLE"
        };
        this.batches.push(newBatch);

        // Record stock movement
        this.movements.push({
          id: this.movements.length + 1,
          timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
          type: "PURCHASE_IN",
          medicineId: it.medicineId,
          batchId: newBatch.id,
          quantity: qty,
          balanceAfter: this.getTotalStock(it.medicineId),
          reference: `PO Receipt ${po.poNumber} (Challan: ${challanNo})`
        });

        receivedCount += qty;
      }
    });

    po.status = "RECEIVED";
    this.saveState();

    this.showToast(`Stock Receipt Completed! Added ${receivedCount} units across batches into inventory.`, "success");
    this.renderReceivingPoSelect();
    this.updateUserContextUI();
  }

  // --------------------------------------------------------------------------
  // Suppliers & Purchase Orders (Admin)
  // --------------------------------------------------------------------------
  renderSuppliersTable() {
    const tbody = document.querySelector("#tblSuppliers tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    this.suppliers.forEach(s => {
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>#${s.id}</strong></td>
        <td><strong>${this.escapeHtml(s.name)}</strong></td>
        <td>${this.escapeHtml(s.contactPerson)}</td>
        <td>${this.escapeHtml(s.email)}</td>
        <td>${this.escapeHtml(s.phone)}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderOrdersTable() {
    const tbody = document.querySelector("#tblOrders tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const isAdm = this.isAdmin();

    this.orders.forEach(po => {
      const sup = this.suppliers.find(s => s.id === po.supplierId);
      let statusBadge = `<span class="status-pill pending">${po.status}</span>`;
      if (po.status === "APPROVED") statusBadge = `<span class="status-pill approved">APPROVED</span>`;
      else if (po.status === "RECEIVED") statusBadge = `<span class="status-pill received">RECEIVED</span>`;

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><code>${this.escapeHtml(po.poNumber)}</code></td>
        <td>${sup ? this.escapeHtml(sup.name) : '-'}</td>
        <td>${po.orderDate}</td>
        <td>${statusBadge}</td>
        <td><strong>${this.settings.currency}${po.totalAmount.toFixed(2)}</strong></td>
        <td>
          ${isAdm && po.status === "DRAFT" ? `<button class="btn btn-sm btn-success" onclick="window.mediApp.approvePo(${po.id})">Approve PO</button>` : ''}
          ${po.status === "APPROVED" ? `<button class="btn btn-sm btn-primary" onclick="window.mediApp.openPoReceiving(${po.id})">Receive</button>` : ''}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  approvePo(poId) {
    if (!this.isAdmin()) {
      this.showToast("Only an Administrator can approve purchase orders.", "danger");
      return;
    }
    const po = this.orders.find(o => o.id === poId);
    if (!po) return;
    po.status = "APPROVED";
    this.saveState();
    this.showToast(`Purchase Order ${po.poNumber} approved. Ready for stock delivery receipt.`, "success");
    this.renderOrdersTable();
  }

  openPoReceiving(poId) {
    this.switchTab("receiving");
    const select = document.getElementById("receivingPoSelect");
    if (select) {
      select.value = poId;
      this.renderReceivingItemsTable();
    }
  }

  // --------------------------------------------------------------------------
  // Restock Pipeline
  // --------------------------------------------------------------------------
  renderRestockTable() {
    const tbody = document.querySelector("#tblRestock tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const isAdm = this.isAdmin();

    this.restock.forEach(r => {
      const med = this.medicines.find(m => m.id === r.medicineId);
      let statusBadge = `<span class="status-pill pending">PENDING</span>`;
      if (r.status === "APPROVED") statusBadge = `<span class="status-pill approved">APPROVED</span>`;
      else if (r.status === "FULFILLED") statusBadge = `<span class="status-pill available">FULFILLED</span>`;
      else if (r.status === "REJECTED") statusBadge = `<span class="status-pill rejected">REJECTED</span>`;

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>#REQ-${r.id}</strong></td>
        <td><strong>${med ? this.escapeHtml(med.name) : '-'}</strong></td>
        <td><strong>${r.requestedQuantity}</strong> units</td>
        <td>${this.escapeHtml(r.requestedByName || 'Staff')}</td>
        <td style="font-size:12px;">${this.escapeHtml(r.reason)}</td>
        <td>${statusBadge}</td>
        <td>
          ${isAdm && r.status === "PENDING" ? `
            <div class="btn-group">
              <button class="btn btn-sm btn-success" onclick="window.mediApp.approveRestock(${r.id})">Approve</button>
              <button class="btn btn-sm btn-danger" onclick="window.mediApp.rejectRestock(${r.id})">Reject</button>
            </div>
          ` : `<span class="text-muted" style="font-size:11px;">${r.status}</span>`}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  approveRestock(id) {
    if (!this.isAdmin()) {
      this.showToast("Administrator role required to approve restock requests.", "danger");
      return;
    }
    const r = this.restock.find(x => x.id === id);
    if (!r) return;
    r.status = "APPROVED";
    this.saveState();
    this.showToast(`Restock Request #REQ-${id} approved. Added to procurement pipeline.`, "success");
    this.renderRestockTable();
  }

  rejectRestock(id) {
    if (!this.isAdmin()) {
      this.showToast("Administrator role required to reject restock requests.", "danger");
      return;
    }
    const r = this.restock.find(x => x.id === id);
    if (!r) return;
    r.status = "REJECTED";
    this.saveState();
    this.showToast(`Restock Request #REQ-${id} rejected.`, "warning");
    this.renderRestockTable();
  }

  // --------------------------------------------------------------------------
  // Sales Ledger & My Transactions
  // --------------------------------------------------------------------------
  renderSalesLedgerTable() {
    const tbody = document.querySelector("#tblSalesLedger tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const searchTxt = (document.getElementById("searchSalesCustomers")?.value || "").toLowerCase();

    this.sales.forEach(sale => {
      if (searchTxt && !sale.customerName.toLowerCase().includes(searchTxt) &&
          !sale.invoiceNumber.toLowerCase().includes(searchTxt) &&
          !(sale.customerPhone || "").includes(searchTxt)) {
        return;
      }

      const totalItems = sale.items.reduce((s, it) => s + it.qty, 0);

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>${sale.invoiceNumber}</strong></td>
        <td style="font-size:12px; color:var(--text-muted);">${sale.timestamp}</td>
        <td><strong>${this.escapeHtml(sale.customerName)}</strong></td>
        <td>${this.escapeHtml(sale.customerPhone || '-')}</td>
        <td>${totalItems} units</td>
        <td><span class="badge badge-info">${sale.paymentMethod}</span></td>
        <td>${this.settings.currency}${sale.subtotal.toFixed(2)}</td>
        <td>${this.settings.currency}${(sale.cgst + sale.sgst).toFixed(2)}</td>
        <td><strong style="color:var(--primary);">${this.settings.currency}${sale.grandTotal.toFixed(2)}</strong></td>
        <td>${this.escapeHtml(sale.dispensedBy)}</td>
        <td><button class="btn btn-sm btn-outline" onclick="window.mediApp.viewInvoiceByNumber('${sale.invoiceNumber}')">Invoice</button></td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderMyTransactionsTable() {
    const tbody = document.querySelector("#tblMyTransactions tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const mySales = this.sales.filter(s => s.dispensedBy === this.currentUser.fullName);

    if (mySales.length === 0) {
      tbody.innerHTML = `<tr><td colspan="7" class="text-muted text-center p-3">No sales processed yet during your active session.</td></tr>`;
      return;
    }

    mySales.forEach(sale => {
      const totalUnits = sale.items.reduce((s, it) => s + it.qty, 0);
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td><strong>${sale.invoiceNumber}</strong></td>
        <td>${sale.timestamp.split(' ')[1]}</td>
        <td>${this.escapeHtml(sale.customerName)}</td>
        <td>${totalUnits}</td>
        <td>${sale.paymentMethod}</td>
        <td><strong>${this.settings.currency}${sale.grandTotal.toFixed(2)}</strong></td>
        <td><button class="btn btn-sm btn-outline" onclick="window.mediApp.viewInvoiceByNumber('${sale.invoiceNumber}')">Print</button></td>
      `;
      tbody.appendChild(tr);
    });
  }

  viewInvoiceByNumber(invNo) {
    const sale = this.sales.find(s => s.invoiceNumber === invNo);
    if (sale) {
      this.renderInvoiceModal(sale);
    }
  }

  // --------------------------------------------------------------------------
  // Audit & Stock Movements
  // --------------------------------------------------------------------------
  renderFullMovementsTable() {
    const tbody = document.querySelector("#tblFullMovements tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    const searchTxt = (document.getElementById("searchMovements")?.value || "").toLowerCase();

    const sorted = [...this.movements].reverse();
    sorted.forEach(m => {
      const med = this.medicines.find(x => x.id === m.medicineId);
      const batch = this.batches.find(x => x.id === m.batchId);
      const isPositive = m.quantity > 0;

      const medName = med ? med.name.toLowerCase() : "";
      if (searchTxt && !medName.includes(searchTxt) && !m.type.toLowerCase().includes(searchTxt) && !(m.reference || "").toLowerCase().includes(searchTxt)) {
        return;
      }

      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td>#${m.id}</td>
        <td style="font-size:12px; color:var(--text-muted);">${m.timestamp}</td>
        <td><span class="badge ${isPositive ? 'badge-success' : 'badge-danger'}">${m.type}</span></td>
        <td><strong>${med ? this.escapeHtml(med.name) : '#' + m.medicineId}</strong></td>
        <td><code>${batch ? this.escapeHtml(batch.batchNumber) : '-'}</code></td>
        <td><strong style="color:${isPositive ? 'var(--success)' : 'var(--danger)'}">${isPositive ? '+' : ''}${m.quantity}</strong></td>
        <td><strong>${m.balanceAfter !== undefined ? m.balanceAfter : '-'}</strong></td>
        <td style="font-size:12px;">${this.escapeHtml(m.reference || '')}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderAuditTable() {
    const tbody = document.querySelector("#tblAudit tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    // Synthesize audit trail from movements and adjustments
    const auditEntries = [];
    this.movements.forEach(m => {
      auditEntries.push({
        timestamp: m.timestamp,
        user: "System / Staff",
        action: m.type,
        entityType: "STOCK_BATCH",
        entityId: m.batchId,
        details: `${m.quantity > 0 ? '+' : ''}${m.quantity} units (Balance: ${m.balanceAfter}) — ${m.reference}`
      });
    });

    auditEntries.reverse();

    auditEntries.forEach(a => {
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td style="font-size:12px; color:var(--text-muted);">${a.timestamp}</td>
        <td><strong>${this.escapeHtml(a.user)}</strong></td>
        <td><span class="badge badge-info">${a.action}</span></td>
        <td>${a.entityType}</td>
        <td>#${a.entityId}</td>
        <td style="font-size:12px;">${this.escapeHtml(a.details)}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  // --------------------------------------------------------------------------
  // User Management
  // --------------------------------------------------------------------------
  renderUsersTable() {
    const tbody = document.querySelector("#tblUsers tbody");
    if (!tbody) return;
    tbody.innerHTML = "";

    this.users.forEach(u => {
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td>#${u.id}</td>
        <td><code>${this.escapeHtml(u.username)}</code></td>
        <td><strong>${this.escapeHtml(u.fullName)}</strong></td>
        <td>${this.escapeHtml(u.email)}</td>
        <td><span class="role-pill ${u.role === 'ADMINISTRATOR' ? 'admin' : 'employee'}">${u.role}</span></td>
        <td><span class="badge ${u.active ? 'badge-success' : 'badge-danger'}">${u.active ? 'ACTIVE' : 'SUSPENDED'}</span></td>
        <td>
          ${u.id !== this.currentUser.id ? `
            <button class="btn btn-sm btn-outline" onclick="window.mediApp.toggleUserStatus(${u.id})">
              ${u.active ? 'Disable' : 'Enable'}
            </button>
          ` : '<span class="text-muted" style="font-size:11px;">Current Session</span>'}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  toggleUserStatus(userId) {
    const u = this.users.find(x => x.id === userId);
    if (u) {
      u.active = !u.active;
      this.saveState();
      this.showToast(`User ${u.username} marked as ${u.active ? 'Active' : 'Suspended'}.`, "info");
      this.renderUsersTable();
    }
  }

  // --------------------------------------------------------------------------
  // CSV Exports & Native Reports
  // --------------------------------------------------------------------------
  async generateValuationReport() {
    const pre = document.getElementById("reportPreviewContent");
    const title = document.getElementById("reportTitle");
    if (title) title.textContent = "📦 Authoritative Inventory Valuation Report";
    if (pre) pre.textContent = "Querying C++ SQLite engine...";

    try {
      const res = await fetch("/api/reports/valuation");
      if (res.ok) {
        const json = await res.json();
        if (pre) pre.textContent = json.report;
        return;
      }
    } catch (e) {
      // client fallback below
    }

    let report = `========================================================================================\n`;
    report += `                     MEDIFIND AUTHORITATIVE INVENTORY VALUATION REPORT                   \n`;
    report += `========================================================================================\n`;
    report += `Generated At: ${new Date().toISOString().replace('T', ' ').substring(0, 19)} | FEFO Valuation Engine\n`;
    report += `Pharmacy: ${this.settings.pharmacyName} | GSTIN: ${this.settings.gstin}\n`;
    report += `----------------------------------------------------------------------------------------\n\n`;

    let totalCost = 0;
    let totalRetail = 0;
    let totalUnits = 0;

    report += `ID  MEDICINE NAME                 TOTAL UNITS    COST PRICE     RETAIL VALUE   MARGIN (%)\n`;
    report += `----------------------------------------------------------------------------------------\n`;

    this.medicines.forEach(m => {
      const batches = this.batches.filter(b => b.medicineId === m.id && b.status !== "EXPIRED" && b.expiryDate >= this.currentDate);
      const units = batches.reduce((sum, b) => sum + b.quantity, 0);
      const medCost = batches.reduce((sum, b) => sum + (b.quantity * b.purchaseCost), 0);
      const medRetail = batches.reduce((sum, b) => sum + (b.quantity * b.sellingPrice), 0);
      const margin = medCost > 0 ? (((medRetail - medCost) / medCost) * 100).toFixed(1) : "0.0";

      totalCost += medCost;
      totalRetail += medRetail;
      totalUnits += units;

      report += `${m.id.toString().padEnd(4)}${m.name.padEnd(30)}${units.toString().padEnd(15)}${this.settings.currency}${medCost.toFixed(2).padEnd(14)}${this.settings.currency}${medRetail.toFixed(2).padEnd(14)}${margin}%\n`;
    });

    report += `----------------------------------------------------------------------------------------\n`;
    report += `TOTAL ACTIVE INVENTORY UNITS : ${totalUnits.toLocaleString()}\n`;
    report += `TOTAL ASSET BOOK VALUE (COST): ${this.settings.currency}${totalCost.toFixed(2)}\n`;
    report += `PROJECTED GROSS RETAIL VALUE : ${this.settings.currency}${totalRetail.toFixed(2)}\n`;
    report += `ESTIMATED GROSS PROFIT SPREAD: ${this.settings.currency}${(totalRetail - totalCost).toFixed(2)}\n`;
    report += `========================================================================================\n`;

    if (pre) pre.textContent = report;
  }

  async generateExpiryReport() {
    const pre = document.getElementById("reportPreviewContent");
    const title = document.getElementById("reportTitle");
    if (title) title.textContent = "⚠️ Regulatory Expiry & Safety Hazard Report (90-Day Horizon)";
    if (pre) pre.textContent = "Querying C++ SQLite engine...";

    try {
      const res = await fetch("/api/reports/expiry");
      if (res.ok) {
        const json = await res.json();
        if (pre) pre.textContent = json.report;
        return;
      }
    } catch (e) {
      // client fallback below
    }

    let report = `========================================================================================\n`;
    report += `                 MEDIFIND EXPIRY HAZARD & FEFO COMPLIANCE AUDIT REPORT                  \n`;
    report += `========================================================================================\n`;
    report += `Audit Date: ${this.currentDate} | Look-Ahead Horizon: 90 Days\n\n`;

    const sorted = [...this.batches].sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate));

    report += `BATCH NO     MEDICINE                        EXPIRY DATE   DAYS LEFT  QTY    STATUS\n`;
    report += `----------------------------------------------------------------------------------------\n`;

    sorted.forEach(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const daysLeft = Math.ceil((new Date(b.expiryDate) - new Date(this.currentDate)) / (1000 * 60 * 60 * 24));
      const isExp = b.expiryDate < this.currentDate;

      if (isExp || daysLeft <= 90) {
        const statusText = isExp ? "EXPIRED (QUARANTINED)" : `EXPIRING (${daysLeft}d)`;
        report += `${b.batchNumber.padEnd(13)}${(med ? med.name : 'Unknown').padEnd(32)}${b.expiryDate.padEnd(14)}${daysLeft.toString().padEnd(11)}${b.quantity.toString().padEnd(7)}${statusText}\n`;
      }
    });

    report += `========================================================================================\n`;
    report += `* All expired inventory is programmatically blocked from POS dispatch via FEFO engine.\n`;

    if (pre) pre.textContent = report;
  }

  exportCsv(filename, headers, rows) {
    const csvContent = "data:text/csv;charset=utf-8," + [headers.join(","), ...rows.map(e => e.join(","))].join("\n");
    const encodedUri = encodeURI(csvContent);
    const link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", filename);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    this.showToast(`Exported ${filename} successfully.`, "success");
  }

  exportMedicinesCsv() {
    const headers = ["ID", "Name", "Brand", "Strength", "DosageForm", "Category", "Barcode", "TotalStock", "Threshold"];
    const rows = this.medicines.map(m => [
      m.id,
      `"${m.name}"`,
      `"${m.brand}"`,
      `"${m.strength}"`,
      `"${m.dosageForm}"`,
      `"${m.category}"`,
      m.barcode,
      this.getTotalStock(m.id),
      m.minStockThreshold
    ]);
    this.exportCsv("medifind_medicines.csv", headers, rows);
  }

  exportBatchesCsv() {
    const headers = ["BatchID", "MedicineID", "BatchNumber", "MfgDate", "ExpiryDate", "Quantity", "PurchaseCost", "SellingPrice", "Status"];
    const rows = this.batches.map(b => [
      b.id,
      b.medicineId,
      `"${b.batchNumber}"`,
      b.mfgDate,
      b.expiryDate,
      b.quantity,
      b.purchaseCost,
      b.sellingPrice,
      b.status
    ]);
    this.exportCsv("medifind_batches.csv", headers, rows);
  }

  exportSalesCsv() {
    const headers = ["InvoiceNumber", "Timestamp", "CustomerName", "CustomerPhone", "PaymentMethod", "Subtotal", "GST", "GrandTotal", "DispensedBy"];
    const rows = this.sales.map(s => [
      s.invoiceNumber,
      `"${s.timestamp}"`,
      `"${s.customerName}"`,
      `"${s.customerPhone || ''}"`,
      s.paymentMethod,
      s.subtotal,
      (s.cgst + s.sgst),
      s.grandTotal,
      `"${s.dispensedBy}"`
    ]);
    this.exportCsv("medifind_sales.csv", headers, rows);
  }

  exportMovementsCsv() {
    const headers = ["MovementID", "Timestamp", "Type", "MedicineID", "BatchID", "Quantity", "BalanceAfter", "Reference"];
    const rows = this.movements.map(m => [
      m.id,
      `"${m.timestamp}"`,
      m.type,
      m.medicineId,
      m.batchId,
      m.quantity,
      m.balanceAfter,
      `"${m.reference || ''}"`
    ]);
    this.exportCsv("medifind_movements.csv", headers, rows);
  }

  // --------------------------------------------------------------------------
  // Event Listeners & Modals
  // --------------------------------------------------------------------------
  bindEvents() {
    // Sidebar Navigation Click
    document.querySelectorAll(".nav-item").forEach(btn => {
      btn.addEventListener("click", () => {
        const tab = btn.dataset.tab;
        if (tab) this.switchTab(tab);
      });
    });

    // Mobile Sidebar Toggle
    document.getElementById("btnToggleSidebar")?.addEventListener("click", () => {
      document.getElementById("appSidebar")?.classList.toggle("mobile-open");
    });

    // Notifications Drawer Toggle
    document.getElementById("btnOpenNotifDrawer")?.addEventListener("click", () => {
      this.renderNotificationDrawer();
      document.getElementById("notificationDrawer")?.classList.add("open");
    });
    document.getElementById("btnCloseNotifDrawer")?.addEventListener("click", () => {
      document.getElementById("notificationDrawer")?.classList.remove("open");
    });

    // Clear All Notifs
    document.getElementById("btnClearAllNotifs")?.addEventListener("click", () => {
      this.notifications.forEach(n => n.read = true);
      this.saveState();
      this.updateNotificationBadges();
      this.renderNotificationsList();
      this.showToast("All notifications marked as read.", "info");
    });

    // Quick POS Button
    document.getElementById("btnOpenNewSale")?.addEventListener("click", () => {
      this.switchTab("pos");
    });

    // Refresh Movements
    document.getElementById("btnRefreshMovements")?.addEventListener("click", () => {
      this.renderRecentMovementsTable();
      this.showToast("Stock movements ledger refreshed.", "info");
    });

    // Search and Filter Listeners
    document.getElementById("searchMedicine")?.addEventListener("input", () => this.renderMedicinesTable());
    document.getElementById("filterCategory")?.addEventListener("change", () => this.renderMedicinesTable());
    document.getElementById("searchBatch")?.addEventListener("input", () => this.renderBatchesTable());
    document.getElementById("filterBatchStatus")?.addEventListener("change", () => this.renderBatchesTable());
    document.getElementById("searchSalesCustomers")?.addEventListener("input", () => this.renderSalesLedgerTable());
    document.getElementById("searchMovements")?.addEventListener("input", () => this.renderFullMovementsTable());
    document.getElementById("searchAudit")?.addEventListener("input", () => this.renderAuditTable());

    // POS Interactions
    document.getElementById("posSelectMed")?.addEventListener("change", () => this.updatePosSelectedMedStock());
    document.getElementById("posQuantity")?.addEventListener("input", () => this.previewFefoAllocation());
    document.getElementById("posGstRate")?.addEventListener("change", () => this.renderPosCart());
    document.getElementById("btnPosAddToCart")?.addEventListener("click", () => this.handlePosAddToCart());
    document.getElementById("btnPosClearCart")?.addEventListener("click", () => this.clearPosCart());
    document.getElementById("btnPosCheckout")?.addEventListener("click", () => this.handlePosCheckout());

    // Barcode Lookup in POS
    document.getElementById("btnPosBarcodeLookup")?.addEventListener("click", () => {
      const barcode = document.getElementById("posBarcodeScan")?.value.trim().toUpperCase();
      if (!barcode) return;
      const med = this.medicines.find(m => m.barcode.toUpperCase() === barcode);
      if (med) {
        const select = document.getElementById("posSelectMed");
        if (select) {
          select.value = med.id;
          this.updatePosSelectedMedStock();
          this.showToast(`Found: ${med.name}`, "success");
        }
      } else {
        this.showToast(`No medicine matched barcode: ${barcode}`, "warning");
      }
    });

    // Receiving Interactivity
    document.getElementById("receivingPoSelect")?.addEventListener("change", () => this.renderReceivingItemsTable());
    document.getElementById("btnSubmitReceiving")?.addEventListener("click", () => this.handleSubmitReceiving());

    // Modal Trigger Buttons
    document.getElementById("btnOpenAddMed")?.addEventListener("click", () => {
      if (!this.isAdmin()) return;
      this.openModal("modalAddMed");
    });

    document.getElementById("btnOpenStockIn")?.addEventListener("click", () => {
      if (!this.isAdmin()) return;
      this.openStockInForMed(this.medicines[0]?.id || 1);
    });

    document.getElementById("btnOpenAdjustmentRequest")?.addEventListener("click", () => {
      this.requestBatchAdjustment(this.batches[0]?.id || 1);
    });

    document.getElementById("btnOpenSubmitRestock")?.addEventListener("click", () => {
      const select = document.getElementById("restockMedSelect");
      if (select) {
        select.innerHTML = "";
        this.medicines.forEach(m => {
          const opt = document.createElement("option");
          opt.value = m.id;
          opt.textContent = `${m.name} (${m.brand})`;
          select.appendChild(opt);
        });
      }
      this.openModal("modalSubmitRestock");
    });

    document.getElementById("btnOpenCreatePo")?.addEventListener("click", () => {
      if (!this.isAdmin()) return;
      const supSelect = document.getElementById("poSupplierSelect");
      const medSelect = document.getElementById("poMedicineSelect");
      if (supSelect && medSelect) {
        supSelect.innerHTML = "";
        this.suppliers.forEach(s => {
          const opt = document.createElement("option");
          opt.value = s.id;
          opt.textContent = s.name;
          supSelect.appendChild(opt);
        });
        medSelect.innerHTML = "";
        this.medicines.forEach(m => {
          const opt = document.createElement("option");
          opt.value = m.id;
          opt.textContent = `${m.name} (${m.brand})`;
          medSelect.appendChild(opt);
        });
      }
      this.openModal("modalCreatePo");
    });

    document.getElementById("btnOpenAddSupplier")?.addEventListener("click", () => {
      if (!this.isAdmin()) return;
      this.openModal("modalAddSupplier");
    });

    document.getElementById("btnOpenAddUser")?.addEventListener("click", () => {
      if (!this.isAdmin()) return;
      this.openModal("modalAddUser");
    });

    // Save Form Submissions
    document.getElementById("btnSaveMedicine")?.addEventListener("click", () => this.handleSaveMedicine());
    document.getElementById("btnSaveStockIn")?.addEventListener("click", () => this.handleSaveStockIn());
    document.getElementById("btnSubmitAdjustmentReq")?.addEventListener("click", () => this.handleSubmitAdjustmentReq());
    document.getElementById("btnSaveRestockReq")?.addEventListener("click", () => this.handleSaveRestockReq());
    document.getElementById("btnSavePo")?.addEventListener("click", () => this.handleSavePo());
    document.getElementById("btnSaveSupplier")?.addEventListener("click", () => this.handleSaveSupplier());
    document.getElementById("btnSaveUser")?.addEventListener("click", () => this.handleSaveUser());

    // Settings Save
    document.getElementById("btnSaveSettings")?.addEventListener("click", () => {
      if (!this.isAdmin()) return;
      this.settings.pharmacyName = document.getElementById("settingPharmName")?.value || this.settings.pharmacyName;
      this.settings.gstin = document.getElementById("settingGstin")?.value || this.settings.gstin;
      this.settings.dlNo = document.getElementById("settingDlNo")?.value || this.settings.dlNo;
      this.settings.address = document.getElementById("settingAddress")?.value || this.settings.address;
      this.settings.currency = document.getElementById("settingCurrency")?.value || this.settings.currency;
      this.settings.defaultGstRate = parseFloat(document.getElementById("settingDefaultGst")?.value || 12);
      this.saveState();
      this.showToast("Pharmacy and GST settings saved.", "success");
    });

    // Reports Generation
    document.getElementById("btnGenerateValuation")?.addEventListener("click", () => this.generateValuationReport());
    document.getElementById("btnGenerateExpiry")?.addEventListener("click", () => this.generateExpiryReport());
    document.getElementById("btnPrintReport")?.addEventListener("click", () => window.print());

    // CSV Exports
    document.getElementById("btnExportMedsCsv")?.addEventListener("click", () => this.exportMedicinesCsv());
    document.getElementById("btnExportBatchesCsv")?.addEventListener("click", () => this.exportBatchesCsv());
    document.getElementById("btnExportSalesCsv")?.addEventListener("click", () => this.exportSalesCsv());
    document.getElementById("btnExportMovementsCsv")?.addEventListener("click", () => this.exportMovementsCsv());
    document.getElementById("btnExportAuditCsv")?.addEventListener("click", () => this.exportMovementsCsv());

    // Modal Close
    document.querySelectorAll("[data-close]").forEach(el => {
      el.addEventListener("click", () => {
        const id = el.dataset.close;
        if (id) this.closeModal(id);
      });
    });

    // Theme Toggle
    document.getElementById("btnToggleTheme")?.addEventListener("click", () => {
      document.body.classList.toggle("dark-mode");
      const icon = document.getElementById("themeIcon");
      if (icon) icon.textContent = document.body.classList.contains("dark-mode") ? "☀️" : "🌙";
    });

    // User Switcher & Logout
    document.getElementById("btnLogout")?.addEventListener("click", () => this.openModal("modalAuth"));
    document.getElementById("sidebarUserProfile")?.addEventListener("click", () => this.switchTab("profile"));

    document.getElementById("btnSelectAdmin")?.addEventListener("click", () => {
      this.switchUser(this.users.find(u => u.username === "admin") || INITIAL_USERS[0]);
    });
    document.getElementById("btnSelectStaff")?.addEventListener("click", () => {
      this.switchUser(this.users.find(u => u.username === "employee1") || INITIAL_USERS[1]);
    });
  }

  handleSaveMedicine() {
    const name = document.getElementById("newMedName")?.value.trim();
    const brand = document.getElementById("newMedBrand")?.value.trim() || "Generic";
    const strength = document.getElementById("newMedStrength")?.value.trim() || "Standard";
    const form = document.getElementById("newMedForm")?.value.trim() || "Tablet";
    const cat = document.getElementById("newMedCat")?.value.trim() || "General";
    const barcode = document.getElementById("newMedBarcode")?.value.trim() || `MED-${Date.now().toString().slice(-6)}`;
    const min = parseInt(document.getElementById("newMedMin")?.value, 10) || 20;

    if (!name) {
      this.showToast("Medicine name is required.", "warning");
      return;
    }

    const newMed = {
      id: this.medicines.length + 1,
      name,
      brand,
      strength,
      dosageForm: form,
      category: cat,
      barcode,
      minStockThreshold: min
    };

    this.medicines.push(newMed);
    this.saveState();
    this.closeModal("modalAddMed");
    this.showToast(`Medicine ${name} registered.`, "success");
    this.renderMedicinesTable();
  }

  handleSaveStockIn() {
    const medId = parseInt(document.getElementById("stockInMedSelect")?.value, 10);
    const batchNo = document.getElementById("stockInBatchNo")?.value.trim();
    const qty = parseInt(document.getElementById("stockInQty")?.value, 10);
    const mfg = document.getElementById("stockInMfg")?.value || "2024-05-01";
    const exp = document.getElementById("stockInExp")?.value;
    const cost = parseFloat(document.getElementById("stockInCost")?.value) || 2.0;
    const price = parseFloat(document.getElementById("stockInPrice")?.value) || 4.0;

    if (!batchNo || isNaN(qty) || qty <= 0 || !exp) {
      this.showToast("Batch number, valid quantity, and expiry date are required.", "warning");
      return;
    }

    const newBatch = {
      id: this.batches.length + 1,
      medicineId: medId,
      batchNumber: batchNo,
      mfgDate: mfg,
      expiryDate: exp,
      quantity: qty,
      purchaseCost: cost,
      sellingPrice: price,
      status: "AVAILABLE"
    };

    this.batches.push(newBatch);

    this.movements.push({
      id: this.movements.length + 1,
      timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
      type: "PURCHASE_IN",
      medicineId: medId,
      batchId: newBatch.id,
      quantity: qty,
      balanceAfter: this.getTotalStock(medId),
      reference: `Direct Stock-In ${batchNo}`
    });

    this.saveState();
    this.closeModal("modalStockIn");
    this.showToast(`Batch ${batchNo} stocked in (${qty} units).`, "success");
    this.renderBatchesTable();
    this.updateUserContextUI();
  }

  handleSubmitAdjustmentReq() {
    const batchId = parseInt(document.getElementById("adjBatchSelect")?.value, 10);
    const newCount = parseInt(document.getElementById("adjNewCount")?.value, 10);
    const cat = document.getElementById("adjReasonCategory")?.value;
    const justification = document.getElementById("adjJustification")?.value.trim();

    if (isNaN(newCount) || newCount < 0) {
      this.showToast("Physical count must be a non-negative number.", "warning");
      return;
    }
    if (!justification) {
      this.showToast("Mandatory audit justification is required.", "warning");
      return;
    }

    const batch = this.batches.find(b => b.id === batchId);
    if (!batch) return;

    const diff = newCount - batch.quantity;
    const newAdj = {
      id: this.adjustments.length + 1,
      batchId: batch.id,
      medicineId: batch.medicineId,
      currentQty: batch.quantity,
      proposedQty: newCount,
      diffQty: diff,
      submittedBy: this.currentUser.id,
      submittedByName: this.currentUser.fullName,
      category: cat,
      justification,
      status: "PENDING",
      date: this.currentDate
    };

    this.adjustments.push(newAdj);
    this.addNotification("Adjustment Submitted", `${this.currentUser.fullName} requested ${diff > 0 ? '+' : ''}${diff} adjustment on ${batch.batchNumber}.`, "info");

    this.saveState();
    this.closeModal("modalAdjustmentRequest");
    this.showToast("Adjustment request submitted for Administrator approval.", "success");
    this.renderAdjustmentRequestsTable();
    this.updateUserContextUI();
  }

  handleSaveRestockReq() {
    const medId = parseInt(document.getElementById("restockMedSelect")?.value, 10);
    const qty = parseInt(document.getElementById("restockQuantity")?.value, 10);
    const reason = document.getElementById("restockReason")?.value.trim();

    if (isNaN(qty) || qty <= 0 || !reason) {
      this.showToast("Quantity and reason are required.", "warning");
      return;
    }

    const newReq = {
      id: this.restock.length + 1,
      medicineId: medId,
      requestedQuantity: qty,
      requestedBy: this.currentUser.id,
      requestedByName: this.currentUser.fullName,
      reason,
      status: "PENDING",
      date: this.currentDate
    };

    this.restock.push(newReq);
    this.addNotification("Restock Request", `${this.currentUser.fullName} requested ${qty} units of medicine #${medId}.`, "warning");

    this.saveState();
    this.closeModal("modalSubmitRestock");
    this.showToast("Restock request submitted to queue.", "success");
    this.renderRestockTable();
  }

  handleSavePo() {
    const supId = parseInt(document.getElementById("poSupplierSelect")?.value, 10);
    const medId = parseInt(document.getElementById("poMedicineSelect")?.value, 10);
    const qty = parseInt(document.getElementById("poQuantity")?.value, 10);
    const cost = parseFloat(document.getElementById("poUnitCost")?.value) || 2.0;

    if (isNaN(qty) || qty <= 0) {
      this.showToast("Valid quantity is required.", "warning");
      return;
    }

    const poNumber = `PO-2026-${(this.orders.length + 1).toString().padStart(3, '0')}`;
    const total = qty * cost;

    const newPo = {
      id: this.orders.length + 1,
      poNumber,
      supplierId: supId,
      orderDate: this.currentDate,
      status: "APPROVED",
      totalAmount: total,
      items: [{ medicineId: medId, qty, cost, batchNo: `POB-${Date.now().toString().slice(-4)}`, mfgDate: "2024-06-01", expDate: "2026-12-01" }]
    };

    this.orders.push(newPo);
    this.saveState();
    this.closeModal("modalCreatePo");
    this.showToast(`Purchase Order ${poNumber} created & approved. Ready for receiving.`, "success");
    this.renderOrdersTable();
  }

  handleSaveSupplier() {
    const name = document.getElementById("supName")?.value.trim();
    const contact = document.getElementById("supContact")?.value.trim() || "";
    const email = document.getElementById("supEmail")?.value.trim() || "";
    const phone = document.getElementById("supPhone")?.value.trim() || "";

    if (!name) {
      this.showToast("Company name is required.", "warning");
      return;
    }

    const newSup = {
      id: this.suppliers.length + 1,
      name,
      contactPerson: contact,
      email,
      phone
    };

    this.suppliers.push(newSup);
    this.saveState();
    this.closeModal("modalAddSupplier");
    this.showToast(`Supplier ${name} registered.`, "success");
    this.renderSuppliersTable();
  }

  handleSaveUser() {
    const username = document.getElementById("newUsername")?.value.trim();
    const fullName = document.getElementById("newUserFullName")?.value.trim();
    const email = document.getElementById("newUserEmail")?.value.trim() || `${username}@pharmacy.com`;
    const role = document.getElementById("newUserRole")?.value;

    if (!username || !fullName) {
      this.showToast("Username and full name are required.", "warning");
      return;
    }

    const newUser = {
      id: this.users.length + 1,
      username,
      fullName,
      email,
      role,
      active: true
    };

    this.users.push(newUser);
    this.saveState();
    this.closeModal("modalAddUser");
    this.showToast(`User ${username} created.`, "success");
    this.renderUsersTable();
  }

  // --------------------------------------------------------------------------
  // UI Helpers: Modals & Toasts
  // --------------------------------------------------------------------------
  openModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) modal.classList.add("active");
  }

  closeModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) modal.classList.remove("active");
  }

  showToast(message, type = "info") {
    const stack = document.getElementById("toastStack");
    if (!stack) return;

    const toast = document.createElement("div");
    toast.className = `toast ${type}`;
    toast.innerHTML = `
      <div style="flex:1;">${this.escapeHtml(message)}</div>
      <button style="background:none; border:none; color:inherit; cursor:pointer; font-weight:700;" onclick="this.parentElement.remove()">&times;</button>
    `;

    stack.appendChild(toast);
    setTimeout(() => {
      if (toast.parentElement) toast.remove();
    }, 4500);
  }

  escapeHtml(str) {
    if (!str) return "";
    return String(str)
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;")
      .replace(/'/g, "&#039;");
  }

  renderAll() {
    this.renderDashboard();
    this.updateNotificationBadges();
  }
}

// Instantiate on DOMContentLoaded
document.addEventListener("DOMContentLoaded", () => {
  window.mediApp = new MediFindApp();
});
