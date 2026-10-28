/**
 * MediFind — Enterprise Medical Inventory Web Application
 * Complete state, FEFO algorithm, POS workflows, reports, and role-based control.
 */

// Initial Seed State matching SeedData.cpp
const INITIAL_MEDICINES = [
  { id: 1, name: "Paracetamol 500mg", brand: "Calpol", strength: "500 mg", dosageForm: "Tablet", category: "Analgesics", barcode: "MED-PARA-500", minStockThreshold: 20 },
  { id: 2, name: "Amoxicillin 250mg", brand: "Amoxil", strength: "250 mg", dosageForm: "Capsule", category: "Antibiotics", barcode: "MED-AMOX-250", minStockThreshold: 15 },
  { id: 3, name: "Cetirizine 10mg", brand: "Zyrtec", strength: "10 mg", dosageForm: "Tablet", category: "Antihistamines", barcode: "MED-CETI-010", minStockThreshold: 10 },
  { id: 4, name: "Metformin 500mg", brand: "Glucophage", strength: "500 mg", dosageForm: "Tablet", category: "Antidiabetics", barcode: "MED-METF-500", minStockThreshold: 25 },
  { id: 5, name: "Atorvastatin 20mg", brand: "Lipitor", strength: "20 mg", dosageForm: "Tablet", category: "Cardiovascular", barcode: "MED-ATOR-020", minStockThreshold: 15 },
  { id: 6, name: "Omeprazole 20mg", brand: "Prilosec", strength: "20 mg", dosageForm: "Capsule", category: "Gastrointestinal", barcode: "MED-OMEP-020", minStockThreshold: 10 },
  { id: 7, name: "Azithromycin 500mg", brand: "Zithromax", strength: "500 mg", dosageForm: "Tablet", category: "Antibiotics", barcode: "MED-AZIT-500", minStockThreshold: 10 },
  { id: 8, name: "Ibuprofen 400mg", brand: "Advil", strength: "400 mg", dosageForm: "Tablet", category: "Analgesics", barcode: "MED-IBUP-400", minStockThreshold: 15 },
  { id: 9, name: "Salbutamol Inhaler", brand: "Ventolin", strength: "100 mcg", dosageForm: "Inhaler", category: "Respiratory", barcode: "MED-SALB-100", minStockThreshold: 5 },
  { id: 10, name: "Insulin Glargine", brand: "Lantus", strength: "100 units/ml", dosageForm: "Injection", category: "Antidiabetics", barcode: "MED-INSU-100", minStockThreshold: 5 }
];

const INITIAL_BATCHES = [
  { id: 1, medicineId: 1, batchNumber: "PAR-24-A1", mfgDate: "2024-03-01", expiryDate: "2026-11-15", quantity: 150, purchaseCost: 1.20, sellingPrice: 2.50, status: "NEAR_EXPIRY" },
  { id: 2, medicineId: 1, batchNumber: "PAR-25-B2", mfgDate: "2025-01-10", expiryDate: "2027-08-20", quantity: 200, purchaseCost: 1.30, sellingPrice: 2.50, status: "AVAILABLE" },
  { id: 3, medicineId: 1, batchNumber: "PAR-23-OLD", mfgDate: "2023-01-01", expiryDate: "2025-01-01", quantity: 25, purchaseCost: 1.10, sellingPrice: 2.50, status: "EXPIRED" },
  { id: 4, medicineId: 2, batchNumber: "AMX-24-01", mfgDate: "2024-05-12", expiryDate: "2026-12-01", quantity: 80, purchaseCost: 3.50, sellingPrice: 6.00, status: "NEAR_EXPIRY" },
  { id: 5, medicineId: 2, batchNumber: "AMX-25-02", mfgDate: "2025-02-15", expiryDate: "2028-02-15", quantity: 120, purchaseCost: 3.80, sellingPrice: 6.50, status: "AVAILABLE" },
  { id: 6, medicineId: 3, batchNumber: "CET-24-01", mfgDate: "2024-06-01", expiryDate: "2027-05-30", quantity: 5, purchaseCost: 0.80, sellingPrice: 1.50, status: "AVAILABLE" },
  { id: 7, medicineId: 4, batchNumber: "MET-24-X1", mfgDate: "2024-04-10", expiryDate: "2027-04-01", quantity: 300, purchaseCost: 1.80, sellingPrice: 3.20, status: "AVAILABLE" },
  { id: 8, medicineId: 5, batchNumber: "ATR-24-01", mfgDate: "2024-07-20", expiryDate: "2027-07-15", quantity: 100, purchaseCost: 4.20, sellingPrice: 7.50, status: "AVAILABLE" },
  { id: 9, medicineId: 6, batchNumber: "OMP-24-A9", mfgDate: "2024-08-01", expiryDate: "2027-08-01", quantity: 140, purchaseCost: 2.50, sellingPrice: 4.80, status: "AVAILABLE" },
  { id: 10, medicineId: 7, batchNumber: "AZI-25-01", mfgDate: "2025-01-05", expiryDate: "2027-06-30", quantity: 60, purchaseCost: 5.00, sellingPrice: 9.00, status: "AVAILABLE" },
  { id: 11, medicineId: 8, batchNumber: "IBU-24-K2", mfgDate: "2024-09-15", expiryDate: "2027-09-15", quantity: 180, purchaseCost: 1.50, sellingPrice: 3.00, status: "AVAILABLE" },
  { id: 12, medicineId: 9, batchNumber: "SAL-24-V1", mfgDate: "2024-02-10", expiryDate: "2027-01-20", quantity: 40, purchaseCost: 8.00, sellingPrice: 14.50, status: "AVAILABLE" },
  { id: 13, medicineId: 10, batchNumber: "INS-24-GL", mfgDate: "2024-10-01", expiryDate: "2026-11-30", quantity: 30, purchaseCost: 22.00, sellingPrice: 38.00, status: "NEAR_EXPIRY" }
];

const INITIAL_SUPPLIERS = [
  { id: 1, name: "Pfizer Healthcare Logistics", contactPerson: "David Miller", email: "orders@pfizer-logistics.demo", phone: "+1-800-555-0191" },
  { id: 2, name: "Novartis Global Distribution", contactPerson: "Elena Rostova", email: "supply@novartis-dist.demo", phone: "+1-800-555-0144" },
  { id: 3, name: "Cipla Care Supplies", contactPerson: "Rajesh Sharma", email: "care@cipla-supplies.demo", phone: "+1-800-555-0177" }
];

const INITIAL_ORDERS = [
  { id: 1, orderRef: "PO-2026-0001", supplierName: "Pfizer Healthcare Logistics", orderDate: "2026-10-20", status: "RECEIVED", totalCost: 750.00 },
  { id: 2, orderRef: "PO-2026-0002", supplierName: "Novartis Global Distribution", orderDate: "2026-10-25", status: "APPROVED", totalCost: 1140.00 }
];

const INITIAL_RESTOCK = [
  { id: 1, requestRef: "REQ-2026-001", medicineName: "Cetirizine 10mg", requestedQuantity: 50, submittedBy: "employee1", reason: "Stock depleted below critical threshold (5 units left)", status: "PENDING" }
];

const INITIAL_MOVEMENTS = [
  { timestamp: "2026-10-27 10:15:22", type: "SALE", medicineName: "Paracetamol 500mg", batchNumber: "PAR-24-A1", quantity: 10, balanceAfter: 150, reference: "INV-2026-00001" },
  { timestamp: "2026-10-26 14:00:10", type: "STOCK_IN", medicineName: "Insulin Glargine", batchNumber: "INS-24-GL", quantity: 30, balanceAfter: 30, reference: "REC-2026-99" },
  { timestamp: "2026-10-25 09:30:45", type: "ADJUSTMENT_NEGATIVE", medicineName: "Cetirizine 10mg", batchNumber: "CET-24-01", quantity: 2, balanceAfter: 5, reference: "ADJ-003" }
];

const INITIAL_AUDIT = [
  { timestamp: "2026-10-27 10:15:22", user: "employee1", action: "SALE_COMPLETED", entityType: "SALE", entityId: "INV-2026-00001", changeSummary: "Dispensed 10 units of Paracetamol 500mg via FEFO batch PAR-24-A1" },
  { timestamp: "2026-10-26 14:00:10", user: "admin", action: "STOCK_IN", entityType: "BATCH", entityId: "BAT-13", changeSummary: "Received shipment of 30 units Insulin Glargine" },
  { timestamp: "2026-10-25 09:30:45", user: "admin", action: "STOCK_ADJUSTMENT", entityType: "BATCH", entityId: "BAT-6", changeSummary: "Manual stock adjustment for Cetirizine 10mg: damaged blister pack" }
];

class MediFindApp {
  constructor() {
    this.currentUser = { id: 1, username: "admin", fullName: "System Administrator", role: "ADMINISTRATOR" };
    this.medicines = JSON.parse(localStorage.getItem("medifind_meds")) || INITIAL_MEDICINES;
    this.batches = JSON.parse(localStorage.getItem("medifind_batches")) || INITIAL_BATCHES;
    this.suppliers = INITIAL_SUPPLIERS;
    this.orders = INITIAL_ORDERS;
    this.restockRequests = INITIAL_RESTOCK;
    this.movements = INITIAL_MOVEMENTS;
    this.auditLogs = INITIAL_AUDIT;
    this.posCart = [];
    this.currentDate = "2026-10-28";
    this.categoryChart = null;
    this.expiryChart = null;

    this.init();
  }

  showToast(message, type = "info") {
    const stack = document.getElementById('toastStack');
    if (!stack) return;
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    const icon = type === 'success' ? '✓' : (type === 'danger' ? '⚠️' : 'ℹ');
    toast.innerHTML = `<strong>${icon}</strong> <span>${message}</span>`;
    stack.appendChild(toast);

    setTimeout(() => {
      toast.style.animation = 'toastSlideOut 0.25s forwards';
      setTimeout(() => toast.remove(), 250);
    }, 3500);
  }

  playAudioTone(frequency = 800, duration = 0.08, type = 'sine') {
    try {
      const ctx = new (window.AudioContext || window.webkitAudioContext)();
      const osc = ctx.createOscillator();
      const gain = ctx.createGain();
      osc.type = type;
      osc.frequency.setValueAtTime(frequency, ctx.currentTime);
      gain.gain.setValueAtTime(0.12, ctx.currentTime);
      gain.gain.exponentialRampToValueAtTime(0.001, ctx.currentTime + duration);
      osc.connect(gain);
      gain.connect(ctx.destination);
      osc.start();
      osc.stop(ctx.currentTime + duration);
    } catch (e) {
      // AudioContext unavailable or blocked by policy
    }
  }

  toggleTheme() {
    document.body.classList.toggle('dark-mode');
    const isDark = document.body.classList.contains('dark-mode');
    localStorage.setItem('medifind_theme', isDark ? 'dark' : 'light');
    document.getElementById('themeIcon').innerText = isDark ? '☀️' : '🌙';
    this.renderCharts();
  }

  save() {
    localStorage.setItem("medifind_meds", JSON.stringify(this.medicines));
    localStorage.setItem("medifind_batches", JSON.stringify(this.batches));
  }

  init() {
    this.setupNavigation();
    this.setupModals();
    this.setupEventHandlers();
    this.initDsaVisuals();
    this.refreshAll();
  }

  // --- NAVIGATION ---
  setupNavigation() {
    document.querySelectorAll('.nav-item').forEach(btn => {
      btn.addEventListener('click', (e) => {
        const tab = btn.dataset.tab;
        this.switchTab(tab);
      });
    });

    document.getElementById('btnOpenNewSale').addEventListener('click', () => {
      this.switchTab('pos');
    });

    document.getElementById('btnLogout').addEventListener('click', () => {
      this.openModal('modalAuth');
    });

    document.getElementById('btnSelectAdmin').addEventListener('click', () => {
      this.currentUser = { id: 1, username: "admin", fullName: "Administrator", role: "ADMINISTRATOR" };
      this.closeModal('modalAuth');
      this.refreshUserBadge();
      alert("Switched role to: ADMINISTRATOR (Full access)");
    });

    document.getElementById('btnSelectStaff').addEventListener('click', () => {
      this.currentUser = { id: 2, username: "employee1", fullName: "Staff Dispenser", role: "EMPLOYEE" };
      this.closeModal('modalAuth');
      this.refreshUserBadge();
      this.showToast("Switched role to: EMPLOYEE (Staff persona)", "info");
    });

    const themeBtn = document.getElementById('btnToggleTheme');
    if (themeBtn) {
      themeBtn.addEventListener('click', () => this.toggleTheme());
    }

    if (localStorage.getItem('medifind_theme') === 'dark') {
      document.body.classList.add('dark-mode');
      const icon = document.getElementById('themeIcon');
      if (icon) icon.innerText = '☀️';
    }
  }

  switchTab(tabId) {
    document.querySelectorAll('.nav-item').forEach(b => b.classList.remove('active'));
    document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));

    const targetNav = document.querySelector(`.nav-item[data-tab="${tabId}"]`);
    const targetPanel = document.getElementById(`tab-${tabId}`);

    if (targetNav) targetNav.classList.add('active');
    if (targetPanel) targetPanel.classList.add('active');

    // Update title
    const titles = {
      dashboard: "Executive Operations Dashboard",
      inventory: "Medicine Master Catalog",
      batches: "Batch Inventory & FEFO Expiry Tracking",
      pos: "Point of Sale (POS) — FEFO Clinical Dispenser",
      orders: "Healthcare Suppliers & Purchase Orders",
      restock: "Floor Restock Request Queue (FIFO)",
      dsa: "Data Structures & Algorithms Diagnostic Suite",
      audit: "Immutable Ledger & Audit Trail",
      reports: "Analytics, Financial Valuations & Exports"
    };

    document.getElementById('pageTitle').innerText = titles[tabId] || "MediFind Suite";
  }

  refreshUserBadge() {
    const isAdm = this.currentUser.role === "ADMINISTRATOR";
    document.getElementById('userAvatar').innerText = isAdm ? "A" : "E";
    document.getElementById('userName').innerText = this.currentUser.fullName;
    const badge = document.getElementById('userRoleBadge');
    badge.innerText = isAdm ? "ADMIN" : "STAFF";
    badge.className = `role-pill ${isAdm ? "admin" : "employee"}`;
  }

  // --- REFRESH VIEWS ---
  refreshAll() {
    this.refreshUserBadge();
    this.updateDashboardMetrics();
    this.renderCharts();
    this.renderFefoTable();
    this.renderRecentMovements();
    this.renderMedicinesTable();
    this.renderBatchesTable();
    this.renderPosSelectOptions();
    this.renderSuppliersTable();
    this.renderOrdersTable();
    this.renderRestockTable();
    this.renderAuditTable();
    this.updateDsaComponents();
  }

  renderCharts() {
    if (typeof Chart === 'undefined') return;

    const isDark = document.body.classList.contains('dark-mode');
    const textColor = isDark ? '#9ca3af' : '#64748b';

    // 1. Category Chart
    const catCounts = {};
    this.medicines.forEach(m => {
      catCounts[m.category] = (catCounts[m.category] || 0) + this.getTotalStock(m.id);
    });

    const catCtx = document.getElementById('chartCategory');
    if (catCtx) {
      if (this.categoryChart) this.categoryChart.destroy();
      this.categoryChart = new Chart(catCtx, {
        type: 'doughnut',
        data: {
          labels: Object.keys(catCounts),
          datasets: [{
            data: Object.values(catCounts),
            backgroundColor: [
              '#3b82f6', '#06b6d4', '#10b981', '#f59e0b',
              '#8b5cf6', '#ec4899', '#14b8a6', '#64748b'
            ],
            borderWidth: 2,
            borderColor: isDark ? '#111827' : '#ffffff'
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          plugins: {
            legend: {
              position: 'right',
              labels: { color: textColor, font: { family: 'Plus Jakarta Sans', size: 11 } }
            }
          }
        }
      });
    }

    // 2. Expiry Timeline Chart
    const activeBatches = this.batches.filter(b => b.quantity > 0);
    const sortedBatches = [...activeBatches].sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate)).slice(0, 7);

    const expCtx = document.getElementById('chartExpiryTimeline');
    if (expCtx) {
      if (this.expiryChart) this.expiryChart.destroy();
      this.expiryChart = new Chart(expCtx, {
        type: 'bar',
        data: {
          labels: sortedBatches.map(b => b.batchNumber),
          datasets: [{
            label: 'Shelf Units Remaining',
            data: sortedBatches.map(b => b.quantity),
            backgroundColor: sortedBatches.map(b => b.expiryDate < this.currentDate ? '#ef4444' : (b.status === 'NEAR_EXPIRY' ? '#f59e0b' : '#3b82f6')),
            borderRadius: 6
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          plugins: {
            legend: { display: false }
          },
          scales: {
            x: {
              ticks: { color: textColor, font: { size: 10 } },
              grid: { display: false }
            },
            y: {
              ticks: { color: textColor },
              grid: { color: isDark ? '#1f2937' : '#f1f5f9' }
            }
          }
        }
      });
    }
  }

  // --- DASHBOARD ---
  updateDashboardMetrics() {
    const activeMeds = this.medicines.length;
    const activeBatches = this.batches.filter(b => b.status !== "EXPIRED" && b.quantity > 0).length;
    const totalUnits = this.batches.reduce((sum, b) => sum + (b.status !== "EXPIRED" ? b.quantity : 0), 0);

    let costVal = 0, retailVal = 0;
    this.batches.forEach(b => {
      if (b.status !== "EXPIRED") {
        costVal += b.quantity * b.purchaseCost;
        retailVal += b.quantity * b.sellingPrice;
      }
    });

    const lowStockCount = this.medicines.filter(m => this.getTotalStock(m.id) <= m.minStockThreshold).length;
    const expiredCount = this.batches.filter(b => b.status === "EXPIRED" || b.expiryDate < this.currentDate).length;

    document.getElementById('kpiMeds').innerText = activeMeds;
    document.getElementById('kpiBatches').innerText = activeBatches;
    document.getElementById('kpiUnits').innerText = totalUnits.toLocaleString();
    document.getElementById('kpiCost').innerText = `$${costVal.toFixed(2)}`;
    document.getElementById('kpiRetail').innerText = `$${retailVal.toFixed(2)}`;
    document.getElementById('kpiAlerts').innerText = `${lowStockCount + expiredCount}`;
    document.getElementById('kpiAlertsDetail').innerText = `${lowStockCount} low stock, ${expiredCount} expired batches`;

    // Render alert banners
    const alertBox = document.getElementById('dashboardAlertContainer');
    alertBox.innerHTML = '';
    if (lowStockCount > 0 || expiredCount > 0) {
      alertBox.innerHTML = `
        <div style="background: #fef2f2; border-left: 4px solid #ef4444; padding: 12px 16px; border-radius: 8px; margin-bottom: 20px; display: flex; justify-content: space-between; align-items: center;">
          <div>
            <strong style="color: #991b1b;">Telemetry Alert Detected:</strong>
            <span style="color: #b91c1c; font-size: 13px; margin-left: 8px;">
              ${lowStockCount > 0 ? `${lowStockCount} medicines below reorder threshold.` : ''}
              ${expiredCount > 0 ? ` ${expiredCount} batch quarantined due to expiration.` : ''}
            </span>
          </div>
          <button class="btn btn-sm btn-outline text-danger" onclick="app.switchTab('batches')">Inspect Batches</button>
        </div>
      `;
    }
  }

  getTotalStock(medId) {
    return this.batches
      .filter(b => b.medicineId === medId && b.status !== "EXPIRED" && b.expiryDate > this.currentDate)
      .reduce((sum, b) => sum + b.quantity, 0);
  }

  // --- FEFO TABLE ON DASHBOARD ---
  renderFefoTable() {
    const tbody = document.querySelector('#tblFefoPreview tbody');
    tbody.innerHTML = '';

    // Active batches sorted by expiry ASC (FEFO)
    const sorted = [...this.batches]
      .filter(b => b.quantity > 0)
      .sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate));

    sorted.slice(0, 6).forEach(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const isExpired = b.expiryDate < this.currentDate;
      const daysLeft = Math.ceil((new Date(b.expiryDate) - new Date(this.currentDate)) / (1000 * 60 * 60 * 24));

      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td><strong>${med ? med.name : 'Unknown'}</strong></td>
        <td><code>${b.batchNumber}</code></td>
        <td>${b.expiryDate}</td>
        <td>${isExpired ? '<span class="text-danger font-bold">EXPIRED</span>' : `${daysLeft} days`}</td>
        <td><strong>${b.quantity}</strong></td>
        <td>$${b.purchaseCost.toFixed(2)}</td>
        <td>${this.renderBadge(isExpired ? 'EXPIRED' : b.status)}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderRecentMovements() {
    const tbody = document.querySelector('#tblRecentMovements tbody');
    tbody.innerHTML = '';
    this.movements.slice(0, 5).forEach(m => {
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td><small class="text-muted">${m.timestamp}</small></td>
        <td><span class="badge ${m.type === 'SALE' ? 'badge-primary' : (m.type === 'STOCK_IN' ? 'badge-success' : 'badge-warning')}">${m.type}</span></td>
        <td>${m.medicineName}</td>
        <td><code>${m.batchNumber}</code></td>
        <td><strong>${m.quantity}</strong></td>
        <td>${m.balanceAfter}</td>
        <td>${m.reference}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderBadge(status) {
    if (status === 'AVAILABLE') return `<span class="badge badge-success">Available</span>`;
    if (status === 'NEAR_EXPIRY') return `<span class="badge badge-warning">Near Expiry</span>`;
    if (status === 'EXPIRED') return `<span class="badge badge-danger">Expired</span>`;
    if (status === 'DEPLETED') return `<span class="badge badge-info">Depleted</span>`;
    return `<span class="badge">${status}</span>`;
  }

  // --- MEDICINE CATALOG TAB ---
  renderMedicinesTable() {
    const tbody = document.querySelector('#tblMedicines tbody');
    tbody.innerHTML = '';

    const filterTxt = document.getElementById('searchMedicine').value.toLowerCase();
    const filterCat = document.getElementById('filterCategory').value;

    const filtered = this.medicines.filter(m => {
      const matchesSearch = m.name.toLowerCase().includes(filterTxt) || m.brand.toLowerCase().includes(filterTxt) || m.barcode.toLowerCase().includes(filterTxt);
      const matchesCat = !filterCat || m.category === filterCat;
      return matchesSearch && matchesCat;
    });

    filtered.forEach(m => {
      const stock = this.getTotalStock(m.id);
      const isLow = stock <= m.minStockThreshold;

      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td>${m.id}</td>
        <td><strong>${m.name}</strong></td>
        <td>${m.brand}</td>
        <td>${m.strength}</td>
        <td>${m.dosageForm}</td>
        <td><span class="badge badge-info">${m.category}</span></td>
        <td><code>${m.barcode}</code></td>
        <td><strong class="${isLow ? 'text-danger' : 'text-success'}">${stock} ${isLow ? '(LOW)' : ''}</strong></td>
        <td>${m.minStockThreshold}</td>
        <td>
          <button class="btn btn-sm btn-outline" onclick="app.openStockInForMed(${m.id})">Stock-In</button>
        </td>
      `;
      tbody.appendChild(tr);
    });

    // Populate category dropdown if needed
    const catSelect = document.getElementById('filterCategory');
    if (catSelect.options.length === 1) {
      const cats = [...new Set(this.medicines.map(m => m.category))];
      cats.forEach(c => {
        const opt = document.createElement('option');
        opt.value = c;
        opt.innerText = c;
        catSelect.appendChild(opt);
      });
    }
  }

  // --- BATCHES TAB ---
  renderBatchesTable() {
    const tbody = document.querySelector('#tblBatches tbody');
    tbody.innerHTML = '';

    const search = document.getElementById('searchBatch').value.toLowerCase();
    const statusFilter = document.getElementById('filterBatchStatus').value;

    const filtered = this.batches.filter(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const medName = med ? med.name.toLowerCase() : '';
      const matchesSearch = b.batchNumber.toLowerCase().includes(search) || medName.includes(search);
      const isExpired = b.expiryDate < this.currentDate;
      const actualStatus = isExpired ? 'EXPIRED' : b.status;
      const matchesStatus = !statusFilter || actualStatus === statusFilter;
      return matchesSearch && matchesStatus;
    });

    filtered.forEach(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const isExpired = b.expiryDate < this.currentDate;

      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td>${b.id}</td>
        <td><strong>${med ? med.name : 'Unknown'}</strong></td>
        <td><code>${b.batchNumber}</code></td>
        <td>${b.mfgDate}</td>
        <td>${b.expiryDate}</td>
        <td><strong>${b.quantity}</strong></td>
        <td>$${b.purchaseCost.toFixed(2)}</td>
        <td>$${b.sellingPrice.toFixed(2)}</td>
        <td>${this.renderBadge(isExpired ? 'EXPIRED' : b.status)}</td>
        <td>
          ${this.currentUser.role === 'ADMINISTRATOR' ?
            `<button class="btn btn-sm btn-outline" onclick="app.openAdjustModal(${b.id})">Adjust Stock</button>` :
            `<span class="text-muted">Admin Only</span>`}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  // --- POS DISPENSER WITH FEFO ALLOCATION ---
  renderPosSelectOptions() {
    const select = document.getElementById('posSelectMed');
    select.innerHTML = '';

    this.medicines.forEach(m => {
      const stock = this.getTotalStock(m.id);
      if (stock > 0) {
        const opt = document.createElement('option');
        opt.value = m.id;
        opt.innerText = `${m.name} (${m.strength}) — In Stock: ${stock}`;
        select.appendChild(opt);
      }
    });

    this.updatePosSelectedMedicine();
  }

  updatePosSelectedMedicine() {
    const medId = parseInt(document.getElementById('posSelectMed').value);
    const stock = this.getTotalStock(medId);
    document.getElementById('posAvailableStock').value = `${stock} units`;
    this.previewFefoAllocation();
  }

  previewFefoAllocation() {
    const medId = parseInt(document.getElementById('posSelectMed').value);
    const qty = parseInt(document.getElementById('posQuantity').value) || 1;
    const box = document.getElementById('posFefoExplanation');

    if (!medId) {
      box.innerHTML = `Select a medicine above.`;
      return;
    }

    // Run FEFO calculation on available batches
    const eligibleBatches = this.batches
      .filter(b => b.medicineId === medId && b.quantity > 0 && b.expiryDate > this.currentDate && b.status !== 'EXPIRED')
      .sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate));

    let remaining = qty;
    let allocationPlan = [];

    for (let b of eligibleBatches) {
      if (remaining <= 0) break;
      const take = Math.min(b.quantity, remaining);
      allocationPlan.push({ batch: b, take });
      remaining -= take;
    }

    if (remaining > 0) {
      box.innerHTML = `<span class="text-danger font-bold">⚠️ Insufficient stock! Requested ${qty}, but only ${qty - remaining} available in valid non-expired batches.</span>`;
    } else {
      box.innerHTML = `
        <div style="font-size: 12.5px;">
          <strong>FEFO Rule Applied:</strong> Earliest expiring batches will be debited first:
          <ul style="margin-left: 18px; margin-top: 6px;">
            ${allocationPlan.map(p => `<li>Batch <code>${p.batch.batchNumber}</code> (Expires ${p.batch.expiryDate}): <strong>${p.take} units</strong> @ $${p.batch.sellingPrice.toFixed(2)}</li>`).join('')}
          </ul>
        </div>
      `;
    }
  }

  addToCart() {
    const medId = parseInt(document.getElementById('posSelectMed').value);
    const reqQty = parseInt(document.getElementById('posQuantity').value) || 1;

    if (reqQty <= 0) {
      alert("Please enter a valid quantity greater than zero.");
      return;
    }

    // Allocate FEFO
    const eligibleBatches = this.batches
      .filter(b => b.medicineId === medId && b.quantity > 0 && b.expiryDate > this.currentDate && b.status !== 'EXPIRED')
      .sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate));

    // Calculate how much already in cart for this batch
    let alreadyInCart = {};
    this.posCart.forEach(item => {
      alreadyInCart[item.batchId] = (alreadyInCart[item.batchId] || 0) + item.qty;
    });

    let remaining = reqQty;
    let addedItems = [];

    for (let b of eligibleBatches) {
      if (remaining <= 0) break;
      const currentAvailable = b.quantity - (alreadyInCart[b.id] || 0);
      if (currentAvailable <= 0) continue;

      const take = Math.min(currentAvailable, remaining);
      const med = this.medicines.find(m => m.id === medId);

      addedItems.push({
        medicineId: medId,
        medicineName: med.name,
        batchId: b.id,
        batchNumber: b.batchNumber,
        expiryDate: b.expiryDate,
        unitPrice: b.sellingPrice,
        qty: take,
        subtotal: take * b.sellingPrice
      });

      remaining -= take;
    }

    if (remaining > 0) {
      this.playAudioTone(300, 0.15, 'sawtooth');
      this.showToast(`Cannot satisfy full order under FEFO policy. ${remaining} units unavailable.`, 'danger');
      return;
    }

    this.posCart.push(...addedItems);
    this.playAudioTone(1050, 0.06, 'sine');
    this.showToast(`Added to cart with FEFO batch allocation preview.`, 'success');
    this.renderPosCart();
  }

  renderPosCart() {
    const tbody = document.querySelector('#tblPosCart tbody');
    tbody.innerHTML = '';
    let total = 0;

    this.posCart.forEach((item, index) => {
      total += item.subtotal;
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td><strong>${item.medicineName}</strong></td>
        <td><code>${item.batchNumber}</code></td>
        <td>${item.expiryDate}</td>
        <td>${item.qty}</td>
        <td>$${item.unitPrice.toFixed(2)}</td>
        <td><strong>$${item.subtotal.toFixed(2)}</strong></td>
        <td><button class="btn btn-sm btn-outline text-danger" onclick="app.removeCartItem(${index})">&times;</button></td>
      `;
      tbody.appendChild(tr);
    });

    document.getElementById('posSubtotal').innerText = `$${total.toFixed(2)}`;
    document.getElementById('posGrandTotal').innerText = `$${total.toFixed(2)}`;
  }

  removeCartItem(idx) {
    this.posCart.splice(idx, 1);
    this.renderPosCart();
  }

  clearCart() {
    this.posCart = [];
    this.renderPosCart();
  }

  checkout() {
    if (this.posCart.length === 0) {
      alert("Cart is empty! Add medicines to checkout.");
      return;
    }

    const customer = document.getElementById('posCustomerName').value.trim() || "Walk-In Patient";
    const method = document.getElementById('posPaymentMethod').value;
    const invoiceNum = `INV-2026-${Math.floor(10000 + Math.random() * 90000)}`;

    let grandTotal = 0;

    // Deduct batches atomically
    this.posCart.forEach(item => {
      const b = this.batches.find(b => b.id === item.batchId);
      if (b) {
        b.quantity -= item.qty;
        if (b.quantity === 0) b.status = "DEPLETED";

        // Log movement
        this.movements.unshift({
          timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
          type: "SALE",
          medicineName: item.medicineName,
          batchNumber: item.batchNumber,
          quantity: item.qty,
          balanceAfter: b.quantity,
          reference: invoiceNum
        });
      }
      grandTotal += item.subtotal;
    });

    // Add audit log
    this.auditLogs.unshift({
      timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
      user: this.currentUser.username,
      action: "SALE_COMPLETED",
      entityType: "SALE",
      entityId: invoiceNum,
      changeSummary: `Processed dispense for ${customer}: $${grandTotal.toFixed(2)} (${this.posCart.length} lines)`
    });

    this.save();

    // Render receipt in modal
    const receiptHtml = `
      <div style="font-family: monospace; padding: 10px; border: 1px solid #cbd5e1; border-radius: 6px; background: white;">
        <div style="text-align: center; border-bottom: 1px dashed #cbd5e1; padding-bottom: 8px;">
          <h2 style="font-size: 18px; margin: 0;">🏥 MEDIFIND CLINICAL PHARMACY</h2>
          <p style="font-size: 11px; color: #64748b; margin: 2px 0;">Official Dispensation & FEFO Allocation Receipt</p>
          <p style="font-size: 11px;">Invoice Ref: <strong>${invoiceNum}</strong> | Date: ${new Date().toLocaleDateString()}</p>
          <p style="font-size: 11px;">Customer: <strong>${customer}</strong> | Payment: ${method}</p>
        </div>
        <table style="width: 100%; font-size: 12px; margin: 12px 0; border-collapse: collapse;">
          <thead>
            <tr style="border-bottom: 1px solid #000; text-align: left;">
              <th>Item / Strength</th>
              <th>Batch</th>
              <th>Expiry</th>
              <th>Qty</th>
              <th>Price</th>
              <th>Total</th>
            </tr>
          </thead>
          <tbody>
            ${this.posCart.map(i => `
              <tr>
                <td>${i.medicineName}</td>
                <td>${i.batchNumber}</td>
                <td>${i.expiryDate}</td>
                <td>${i.qty}</td>
                <td>$${i.unitPrice.toFixed(2)}</td>
                <td>$${i.subtotal.toFixed(2)}</td>
              </tr>
            `).join('')}
          </tbody>
        </table>
        <div style="border-top: 1px dashed #cbd5e1; padding-top: 8px; text-align: right;">
          <p style="font-size: 16px; font-weight: bold;">Grand Total: $${grandTotal.toFixed(2)}</p>
          <p style="font-size: 10px; color: #64748b; margin-top: 4px;">Dispensed in full compliance with FEFO guidelines.</p>
        </div>
      </div>
    `;

    document.getElementById('invoiceReceiptBody').innerHTML = receiptHtml;
    this.openModal('modalInvoice');

    this.playAudioTone(1200, 0.12, 'triangle');
    setTimeout(() => this.playAudioTone(1600, 0.18, 'sine'), 100);
    this.showToast(`Dispense confirmed: Invoice ${invoiceNum} generated!`, 'success');

    this.clearCart();
    this.refreshAll();
  }

  // --- SUPPLIERS & ORDERS ---
  renderSuppliersTable() {
    const tbody = document.querySelector('#tblSuppliers tbody');
    tbody.innerHTML = '';
    this.suppliers.forEach(s => {
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td>${s.id}</td>
        <td><strong>${s.name}</strong></td>
        <td>${s.contactPerson}</td>
        <td>${s.email}</td>
        <td>${s.phone}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  renderOrdersTable() {
    const tbody = document.querySelector('#tblOrders tbody');
    tbody.innerHTML = '';
    this.orders.forEach(o => {
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td><code>${o.orderRef}</code></td>
        <td>${o.supplierName}</td>
        <td>${o.orderDate}</td>
        <td><span class="badge ${o.status === 'RECEIVED' ? 'badge-success' : 'badge-info'}">${o.status}</span></td>
        <td><strong>$${o.totalCost.toFixed(2)}</strong></td>
        <td>
          ${o.status !== 'RECEIVED' ? `<button class="btn btn-sm btn-outline text-success" onclick="app.receiveOrder(${o.id})">Mark Received</button>` : `<span class="text-muted">Stocked</span>`}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  receiveOrder(orderId) {
    const o = this.orders.find(o => o.id === orderId);
    if (!o) return;
    o.status = "RECEIVED";
    alert(`Order ${o.orderRef} marked as RECEIVED. Stock batches updated in system.`);
    this.refreshAll();
  }

  // --- RESTOCK REQUESTS ---
  renderRestockTable() {
    const tbody = document.querySelector('#tblRestock tbody');
    tbody.innerHTML = '';
    this.restockRequests.forEach(r => {
      const isAdm = this.currentUser.role === 'ADMINISTRATOR';
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td><code>${r.requestRef}</code></td>
        <td><strong>${r.medicineName}</strong></td>
        <td>${r.requestedQuantity}</td>
        <td>${r.submittedBy}</td>
        <td>${r.reason}</td>
        <td><span class="badge ${r.status === 'APPROVED' ? 'badge-success' : (r.status === 'REJECTED' ? 'badge-danger' : 'badge-warning')}">${r.status}</span></td>
        <td>
          ${r.status === 'PENDING' && isAdm ? `
            <button class="btn btn-sm btn-success" onclick="app.approveRestock(${r.id})">Approve</button>
            <button class="btn btn-sm btn-outline text-danger" onclick="app.rejectRestock(${r.id})">Reject</button>
          ` : `<span class="text-muted">No Actions</span>`}
        </td>
      `;
      tbody.appendChild(tr);
    });
  }

  approveRestock(reqId) {
    const r = this.restockRequests.find(x => x.id === reqId);
    if (r) {
      r.status = "APPROVED";
      this.auditLogs.unshift({
        timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
        user: this.currentUser.username,
        action: "RESTOCK_APPROVED",
        entityType: "RESTOCK",
        entityId: r.requestRef,
        changeSummary: `Administrator approved restock request for ${r.requestedQuantity} units of ${r.medicineName}`
      });
      alert(`Restock Request ${r.requestRef} Approved! Converted into vendor purchase pipeline.`);
      this.refreshAll();
    }
  }

  rejectRestock(reqId) {
    const r = this.restockRequests.find(x => x.id === reqId);
    if (r) {
      r.status = "REJECTED";
      this.refreshAll();
    }
  }

  // --- AUDIT TRAIL ---
  renderAuditTable() {
    const tbody = document.querySelector('#tblAudit tbody');
    tbody.innerHTML = '';
    const q = document.getElementById('searchAudit').value.toLowerCase();

    const filtered = this.auditLogs.filter(l =>
      l.user.toLowerCase().includes(q) ||
      l.action.toLowerCase().includes(q) ||
      l.changeSummary.toLowerCase().includes(q)
    );

    filtered.forEach(l => {
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td><small class="text-muted">${l.timestamp}</small></td>
        <td><strong>${l.user}</strong></td>
        <td><span class="badge badge-primary">${l.action}</span></td>
        <td>${l.entityType}</td>
        <td><code>${l.entityId}</code></td>
        <td>${l.changeSummary}</td>
      `;
      tbody.appendChild(tr);
    });
  }

  // --- MODALS & WORKFLOWS ---
  setupModals() {
    document.querySelectorAll('[data-close]').forEach(b => {
      b.addEventListener('click', () => {
        this.closeModal(b.dataset.close);
      });
    });

    document.getElementById('btnOpenAddMed').addEventListener('click', () => {
      if (this.currentUser.role !== 'ADMINISTRATOR') {
        alert("Permission Denied: Only administrators can add new medicines.");
        return;
      }
      this.openModal('modalAddMed');
    });

    document.getElementById('btnOpenStockIn').addEventListener('click', () => {
      this.openStockInModal();
    });

    document.getElementById('btnOpenSubmitRestock').addEventListener('click', () => {
      const medName = prompt("Enter Medicine Name to restock:");
      if (!medName) return;
      const qty = parseInt(prompt("Enter Requested Quantity:", "50")) || 50;
      const reason = prompt("Reason for restock:", "Inventory falling low") || "Routine floor restock";

      this.restockRequests.unshift({
        id: this.restockRequests.length + 1,
        requestRef: `REQ-2026-${Math.floor(100 + Math.random() * 900)}`,
        medicineName: medName,
        requestedQuantity: qty,
        submittedBy: this.currentUser.username,
        reason: reason,
        status: "PENDING"
      });

      alert("Restock request queued in FIFO review queue.");
      this.refreshAll();
    });

    // Save Medicine
    document.getElementById('btnSaveMedicine').addEventListener('click', () => {
      const name = document.getElementById('newMedName').value.trim();
      if (!name) { alert("Medicine name required"); return; }

      const newId = this.medicines.length + 1;
      const newMed = {
        id: newId,
        name: name,
        brand: document.getElementById('newMedBrand').value.trim() || "Generic",
        strength: document.getElementById('newMedStrength').value.trim() || "500 mg",
        dosageForm: document.getElementById('newMedForm').value.trim() || "Tablet",
        category: document.getElementById('newMedCat').value.trim() || "General",
        barcode: document.getElementById('newMedBarcode').value.trim() || `MED-BAR-${newId}`,
        minStockThreshold: parseInt(document.getElementById('newMedMin').value) || 10
      };

      this.medicines.push(newMed);
      this.save();
      this.closeModal('modalAddMed');
      alert(`Medicine '${name}' registered successfully!`);
      this.refreshAll();
    });

    // Save Stock-In
    document.getElementById('btnSaveStockIn').addEventListener('click', () => {
      const medId = parseInt(document.getElementById('stockInMedSelect').value);
      const batchNo = document.getElementById('stockInBatchNo').value.trim();
      const qty = parseInt(document.getElementById('stockInQty').value) || 0;
      const mfg = document.getElementById('stockInMfg').value || "2025-01-01";
      const exp = document.getElementById('stockInExp').value;
      const cost = parseFloat(document.getElementById('stockInCost').value) || 1.0;
      const price = parseFloat(document.getElementById('stockInPrice').value) || 2.0;

      if (!batchNo || !exp || qty <= 0) {
        alert("Please complete required batch number, valid expiry date, and quantity.");
        return;
      }

      const isExpired = exp < this.currentDate;
      const isNearExpiry = (new Date(exp) - new Date(this.currentDate)) / (1000 * 60 * 60 * 24) <= 90;

      const newBatch = {
        id: this.batches.length + 1,
        medicineId: medId,
        batchNumber: batchNo,
        mfgDate: mfg,
        expiryDate: exp,
        quantity: qty,
        purchaseCost: cost,
        sellingPrice: price,
        status: isExpired ? "EXPIRED" : (isNearExpiry ? "NEAR_EXPIRY" : "AVAILABLE")
      };

      this.batches.push(newBatch);

      const med = this.medicines.find(m => m.id === medId);
      this.movements.unshift({
        timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
        type: "STOCK_IN",
        medicineName: med ? med.name : "Med",
        batchNumber: batchNo,
        quantity: qty,
        balanceAfter: qty,
        reference: `REC-${batchNo}`
      });

      this.save();
      this.closeModal('modalStockIn');
      alert(`Stocked in ${qty} units of batch ${batchNo}!`);
      this.refreshAll();
    });

    // Adjust Stock Confirmation
    document.getElementById('btnConfirmAdjust').addEventListener('click', () => {
      if (this.currentUser.role !== 'ADMINISTRATOR') {
        alert("Permission Denied: Only administrators can authorize manual stock adjustments.");
        return;
      }

      const batchId = this.currentAdjustBatchId;
      const newQty = parseInt(document.getElementById('adjustNewQty').value);
      const reason = document.getElementById('adjustReason').value.trim();

      if (isNaN(newQty) || newQty < 0 || !reason) {
        alert("Please specify a non-negative quantity and a formal audit justification.");
        return;
      }

      const b = this.batches.find(x => x.id === batchId);
      if (b) {
        const diff = newQty - b.quantity;
        b.quantity = newQty;
        if (b.quantity === 0) b.status = "DEPLETED";

        const med = this.medicines.find(m => m.id === b.medicineId);
        this.movements.unshift({
          timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
          type: diff > 0 ? "ADJUSTMENT_POSITIVE" : "ADJUSTMENT_NEGATIVE",
          medicineName: med ? med.name : "Med",
          batchNumber: b.batchNumber,
          quantity: Math.abs(diff),
          balanceAfter: newQty,
          reference: `ADJ-${b.id}`
        });

        this.auditLogs.unshift({
          timestamp: new Date().toISOString().replace('T', ' ').substring(0, 19),
          user: this.currentUser.username,
          action: "STOCK_ADJUSTMENT",
          entityType: "BATCH",
          entityId: b.batchNumber,
          changeSummary: `Adjusted batch ${b.batchNumber} from ${b.quantity - diff} to ${newQty} (Diff: ${diff}). Reason: ${reason}`
        });

        this.save();
        this.closeModal('modalAdjust');
        alert("Stock adjustment successfully applied and audited.");
        this.refreshAll();
      }
    });
  }

  openModal(id) {
    document.getElementById(id).classList.add('active');
  }

  closeModal(id) {
    document.getElementById(id).classList.remove('active');
  }

  openStockInForMed(medId) {
    this.openStockInModal(medId);
  }

  openStockInModal(selectedMedId = null) {
    const sel = document.getElementById('stockInMedSelect');
    sel.innerHTML = '';
    this.medicines.forEach(m => {
      const opt = document.createElement('option');
      opt.value = m.id;
      opt.innerText = `${m.name} (${m.strength})`;
      if (m.id === selectedMedId) opt.selected = true;
      sel.appendChild(opt);
    });
    this.openModal('modalStockIn');
  }

  openAdjustModal(batchId) {
    this.currentAdjustBatchId = batchId;
    const b = this.batches.find(x => x.id === batchId);
    const med = this.medicines.find(m => m.id === b.medicineId);

    document.getElementById('adjustBatchInfo').innerText = `Adjusting ${med ? med.name : 'Med'} (Batch ${b.batchNumber}) — Current System Balance: ${b.quantity} units`;
    document.getElementById('adjustNewQty').value = b.quantity;
    document.getElementById('adjustReason').value = '';
    this.openModal('modalAdjust');
  }

  // --- REPORTS & EXPORTS ---
  setupEventHandlers() {
    document.getElementById('searchMedicine').addEventListener('input', () => this.renderMedicinesTable());
    document.getElementById('filterCategory').addEventListener('change', () => this.renderMedicinesTable());
    document.getElementById('searchBatch').addEventListener('input', () => this.renderBatchesTable());
    document.getElementById('filterBatchStatus').addEventListener('change', () => this.renderBatchesTable());
    document.getElementById('posSelectMed').addEventListener('change', () => this.updatePosSelectedMedicine());
    document.getElementById('posQuantity').addEventListener('input', () => this.previewFefoAllocation());
    document.getElementById('btnPosAddToCart').addEventListener('click', () => this.addToCart());
    document.getElementById('btnPosClearCart').addEventListener('click', () => this.clearCart());
    document.getElementById('btnPosCheckout').addEventListener('click', () => this.checkout());
    document.getElementById('searchAudit').addEventListener('input', () => this.renderAuditTable());
    document.getElementById('btnRefreshMovements').addEventListener('click', () => this.renderRecentMovements());

    // Reports buttons
    document.getElementById('btnGenerateValuation').addEventListener('click', () => this.showValuationReport());
    document.getElementById('btnGenerateExpiry').addEventListener('click', () => this.showExpiryReport());
    document.getElementById('btnExportMedsCsv').addEventListener('click', () => this.exportMedsCsv());
    document.getElementById('btnExportBatchesCsv').addEventListener('click', () => this.exportBatchesCsv());
    document.getElementById('btnExportSalesCsv').addEventListener('click', () => this.exportSalesCsv());
    document.getElementById('btnExportAudit').addEventListener('click', () => this.exportAuditCsv());
    document.getElementById('btnExportQuick').addEventListener('click', () => this.exportBatchesCsv());
    document.getElementById('btnPrintReport').addEventListener('click', () => window.print());

    // BST Search input
    document.getElementById('bstPrefixInput').addEventListener('input', (e) => {
      const val = e.target.value;
      const res = window.mediDsa.bst.findByPrefix(val);
      const box = document.getElementById('bstResults');
      if (res.length === 0) {
        box.innerHTML = `<span class="text-danger">No medicine names match prefix '${val}'</span>`;
      } else {
        box.innerHTML = res.map(r => `<div>[ID #${r.id}] <strong>${r.name}</strong></div>`).join('');
      }
    });
  }

  showValuationReport() {
    document.getElementById('reportTitle').innerText = "Inventory Valuation Report";
    let cost = 0, retail = 0;
    this.batches.forEach(b => {
      if (b.status !== 'EXPIRED') {
        cost += b.quantity * b.purchaseCost;
        retail += b.quantity * b.sellingPrice;
      }
    });

    const report = `
========================================================================
                  MEDIFIND INVENTORY VALUATION REPORT                   
========================================================================
As of Date: ${new Date().toLocaleDateString()}
Valuation Policy: Cost-Basis (Asset Value) vs Projected Retail Turnover

TOTAL ACTIVE MEDICINES:       ${this.medicines.length}
TOTAL INVENTORY SHELF UNITS:  ${this.batches.reduce((s, b) => s + b.quantity, 0)} units

------------------------------------------------------------------------
VALUATION METRICS:
Total Purchase Cost Asset Value:  $${cost.toFixed(2)}
Total Projected Retail Value:     $${retail.toFixed(2)}
Projected Gross Dispensary Margin: $${(retail - cost).toFixed(2)} (${((retail - cost)/retail * 100).toFixed(1)}%)

SUMMARY BY MEDICINE:
${this.medicines.map(m => {
  const mBatches = this.batches.filter(b => b.medicineId === m.id);
  const mQty = mBatches.reduce((s, b) => s + b.quantity, 0);
  const mCost = mBatches.reduce((s, b) => s + b.quantity * b.purchaseCost, 0);
  return ` * [ID ${m.id}] ${m.name.padEnd(25)} | Units: ${String(mQty).padStart(4)} | Value: $${mCost.toFixed(2)}`;
}).join('\n')}
========================================================================
Report generated by MediFind Enterprise Engine.
    `;
    document.getElementById('reportPreviewContent').innerText = report;
  }

  showExpiryReport() {
    document.getElementById('reportTitle').innerText = "Safety & Expiry Alert Report";
    const near = this.batches.filter(b => b.status === 'NEAR_EXPIRY');
    const expired = this.batches.filter(b => b.status === 'EXPIRED' || b.expiryDate < this.currentDate);

    const report = `
========================================================================
                  MEDIFIND PHARMACY SAFETY & EXPIRY REPORT              
========================================================================
Reference Date: ${this.currentDate}
Safety Window: 90 Days

[1] CRITICAL: EXPIRED BATCHES (QUARANTINED FROM FEFO DISPENSARY):
${expired.length === 0 ? "  None. All active batches are within valid shelf life." :
  expired.map(b => {
    const med = this.medicines.find(m => m.id === b.medicineId);
    return `  ! BATCH ${b.batchNumber}: ${med ? med.name : 'Med'} | Expired: ${b.expiryDate} | Qty: ${b.quantity} units (QUARANTINED)`;
  }).join('\n')}

[2] WARNING: NEAR EXPIRY BATCHES (<= 90 DAYS REMAINING):
${near.length === 0 ? "  None." :
  near.map(b => {
    const med = this.medicines.find(m => m.id === b.medicineId);
    return `  * BATCH ${b.batchNumber}: ${med ? med.name : 'Med'} | Expiry: ${b.expiryDate} | Current Qty: ${b.quantity} units`;
  }).join('\n')}

========================================================================
    `;
    document.getElementById('reportPreviewContent').innerText = report;
  }

  // --- CSV DOWNLOAD HELPERS ---
  downloadCsv(filename, csvContent) {
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.setAttribute("href", url);
    link.setAttribute("download", filename);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  }

  exportMedsCsv() {
    let csv = "ID,Name,Brand,Strength,Form,Category,Barcode,MinStockThreshold\n";
    this.medicines.forEach(m => {
      csv += `${m.id},"${m.name}","${m.brand}","${m.strength}","${m.dosageForm}","${m.category}","${m.barcode}",${m.minStockThreshold}\n`;
    });
    this.downloadCsv("medifind_medicines.csv", csv);
  }

  exportBatchesCsv() {
    let csv = "BatchID,MedicineID,MedicineName,BatchNumber,MfgDate,ExpiryDate,Quantity,PurchaseCost,SellingPrice,Status\n";
    this.batches.forEach(b => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      csv += `${b.id},${b.medicineId},"${med ? med.name : ''}","${b.batchNumber}","${b.mfgDate}","${b.expiryDate}",${b.quantity},${b.purchaseCost},${b.sellingPrice},"${b.status}"\n`;
    });
    this.downloadCsv("medifind_batches.csv", csv);
  }

  exportSalesCsv() {
    let csv = "Timestamp,Type,Medicine,BatchNumber,Quantity,BalanceAfter,Reference\n";
    this.movements.forEach(m => {
      csv += `"${m.timestamp}","${m.type}","${m.medicineName}","${m.batchNumber}",${m.quantity},${m.balanceAfter},"${m.reference}"\n`;
    });
    this.downloadCsv("medifind_sales_movements.csv", csv);
  }

  exportAuditCsv() {
    let csv = "Timestamp,User,Action,EntityType,EntityID,ChangeSummary\n";
    this.auditLogs.forEach(l => {
      csv += `"${l.timestamp}","${l.user}","${l.action}","${l.entityType}","${l.entityId}","${l.changeSummary}"\n`;
    });
    this.downloadCsv("medifind_audit_log.csv", csv);
  }

  // --- DSA VISUALIZERS ---
  initDsaVisuals() {
    // Populate BST and Hash Table with initial medicines
    this.medicines.forEach(m => {
      window.mediDsa.hashTable.insert(m.id, m);
      window.mediDsa.bst.insert(m.name, m.id);
    });

    window.mediDsa.hashTable.render('hashTableVis');

    // Trigger initial BST search
    const initialBst = window.mediDsa.bst.findByPrefix('');
    document.getElementById('bstResults').innerHTML = initialBst.slice(0, 6).map(r => `<div>[ID #${r.id}] <strong>${r.name}</strong></div>`).join('');
  }

  updateDsaComponents() {
    const queueBox = document.getElementById('fefoQueueVis');
    if (!queueBox) return;
    queueBox.innerHTML = '';

    // Show top 3 batches prioritized under FEFO
    const fefoQueue = [...this.batches]
      .filter(b => b.quantity > 0 && b.expiryDate > this.currentDate && b.status !== 'EXPIRED')
      .sort((a, b) => new Date(a.expiryDate) - new Date(b.expiryDate));

    fefoQueue.slice(0, 4).forEach((b, idx) => {
      const med = this.medicines.find(m => m.id === b.medicineId);
      const pill = document.createElement('div');
      pill.className = `fefo-pill priority-${idx + 1}`;
      pill.innerHTML = `
        <div>
          <strong>#${idx + 1} Priority: ${med ? med.name : 'Med'}</strong>
          <div class="text-muted" style="font-size:11px;">Batch ${b.batchNumber} | Expiry: ${b.expiryDate}</div>
        </div>
        <div>
          <span class="badge badge-primary">${b.quantity} units</span>
        </div>
      `;
      queueBox.appendChild(pill);
    });
  }
}

// Instantiate global app
window.app = new MediFindApp();
