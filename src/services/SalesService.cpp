#include "services/SalesService.hpp"
#include "utils/DateUtils.hpp"

namespace medifind::services {

SalesService::SalesService(database::DatabaseConnection& db,
                           repositories::SaleRepository& saleRepo,
                           repositories::BatchRepository& batchRepo,
                           repositories::MedicineRepository& medRepo,
                           repositories::ReturnRepository& returnRepo,
                           repositories::StockMovementRepository& movementRepo,
                           repositories::AuditLogRepository& auditRepo)
    : db_(db),
      saleRepo_(saleRepo),
      batchRepo_(batchRepo),
      medRepo_(medRepo),
      returnRepo_(returnRepo),
      movementRepo_(movementRepo),
      auditRepo_(auditRepo),
      fefoAllocator_(batchRepo) {}

std::vector<FefoAllocationPlan> SalesService::previewCartAllocation(const std::vector<SaleCartItem>& cart, const std::string& currentDate) {
    std::vector<FefoAllocationPlan> plans;
    for (const auto& item : cart) {
        plans.push_back(fefoAllocator_.planAllocation(item.medicineId, item.requestedQuantity, currentDate));
    }
    return plans;
}

SaleTransactionResult SalesService::processSale(const std::vector<SaleCartItem>& cart,
                                                int soldByUserId,
                                                const std::string& paymentMethod,
                                                const std::string& customerName,
                                                const std::string& notes,
                                                const std::string& currentDate) {
    SaleTransactionResult result;
    if (cart.empty()) {
        result.success = false;
        result.message = "Sales cart is empty.";
        return result;
    }

    // 1. Plan and validate FEFO allocations for all cart items first
    std::vector<FefoAllocationPlan> plans;
    for (const auto& cartItem : cart) {
        auto plan = fefoAllocator_.planAllocation(cartItem.medicineId, cartItem.requestedQuantity, currentDate);
        if (!plan.isSuccessful) {
            auto med = medRepo_.findById(cartItem.medicineId);
            std::string medName = med ? med->name : ("ID " + std::to_string(cartItem.medicineId));
            result.success = false;
            result.message = "Cannot fulfill order for " + medName + ": " + plan.message;
            return result;
        }
        plans.push_back(plan);
    }

    // 2. Perform atomic database transaction
    database::TransactionGuard tx(db_);

    std::string invoiceNum = saleRepo_.generateNextInvoiceNumber();
    double grandTotal = 0.0;
    for (const auto& p : plans) {
        grandTotal += p.totalAmount;
    }

    models::Sale sale;
    sale.invoiceNumber = invoiceNum;
    sale.saleDate = utils::DateUtils::getCurrentDateTime();
    sale.soldByUserId = soldByUserId;
    sale.totalAmount = grandTotal;
    sale.paymentMethod = paymentMethod.empty() ? "CASH" : paymentMethod;
    sale.customerName = customerName;
    sale.notes = notes;

    for (const auto& plan : plans) {
        for (const auto& alloc : plan.items) {
            models::SaleItem si;
            si.medicineId = plan.medicineId;
            si.batchId = alloc.batchId;
            si.quantity = alloc.allocatedQuantity;
            si.unitPrice = alloc.unitPrice;
            si.subtotal = alloc.subtotal;
            sale.items.push_back(si);
            result.allocatedItems.push_back(alloc);
        }
    }

    int saleId = saleRepo_.createSaleWithItems(sale);
    if (saleId <= 0) {
        result.success = false;
        result.message = "Failed to create sales invoice record.";
        return result;
    }

    // 3. Decrement batches and record stock movements
    for (const auto& plan : plans) {
        for (const auto& alloc : plan.items) {
            auto batchOpt = batchRepo_.findById(alloc.batchId);
            if (!batchOpt || batchOpt->quantity < alloc.allocatedQuantity) {
                result.success = false;
                result.message = "Concurrent stock change detected for batch ID " + std::to_string(alloc.batchId);
                return result;
            }

            int newQty = batchOpt->quantity - alloc.allocatedQuantity;
            batchRepo_.updateQuantity(alloc.batchId, newQty);

            models::StockMovement sm;
            sm.medicineId = plan.medicineId;
            sm.batchId = alloc.batchId;
            sm.type = models::MovementType::SALE;
            sm.quantity = alloc.allocatedQuantity;
            sm.balanceAfter = newQty;
            sm.referenceType = "SALE";
            sm.referenceId = invoiceNum;
            sm.reason = "Sale fulfilled via FEFO allocation";
            sm.userId = soldByUserId;
            movementRepo_.recordMovement(sm);
        }
    }

    // 4. Audit Log
    models::AuditLog log;
    log.userId = soldByUserId;
    log.username = "Actor #" + std::to_string(soldByUserId);
    log.action = "SALE_COMPLETED";
    log.entityType = "SALE";
    log.entityId = invoiceNum;
    log.changeSummary = "Completed sale " + invoiceNum + " for total $" + std::to_string(grandTotal) + " (" + std::to_string(sale.items.size()) + " line items)";
    auditRepo_.logEvent(log);

    tx.commit();

    result.success = true;
    result.message = "Sale processed successfully.";
    result.invoiceNumber = invoiceNum;
    result.saleId = saleId;
    result.totalAmount = grandTotal;
    return result;
}

ReturnResult SalesService::processReturn(int saleId,
                                         int saleItemId,
                                         int returnQuantity,
                                         const std::string& reason,
                                         bool returnToSellableStock,
                                         int processedByUserId) {
    ReturnResult res;
    if (returnQuantity <= 0) {
        res.success = false;
        res.message = "Return quantity must be greater than zero.";
        return res;
    }

    auto saleOpt = saleRepo_.findById(saleId);
    if (!saleOpt) {
        res.success = false;
        res.message = "Sale invoice not found.";
        return res;
    }

    auto items = saleRepo_.getSaleItems(saleId);
    const models::SaleItem* targetItem = nullptr;
    for (const auto& it : items) {
        if (it.id == saleItemId) {
            targetItem = &it;
            break;
        }
    }

    if (!targetItem) {
        res.success = false;
        res.message = "Sale item not found in this invoice.";
        return res;
    }

    if (returnQuantity > targetItem->quantity) {
        res.success = false;
        res.message = "Cannot return more than originally purchased quantity (" + std::to_string(targetItem->quantity) + ").";
        return res;
    }

    database::TransactionGuard tx(db_);

    std::string retRef = returnRepo_.generateNextReturnRef();
    double refund = returnQuantity * targetItem->unitPrice;

    models::ReturnRecord rr;
    rr.returnRef = retRef;
    rr.saleId = saleId;
    rr.saleItemId = saleItemId;
    rr.medicineId = targetItem->medicineId;
    rr.batchId = targetItem->batchId;
    rr.quantity = returnQuantity;
    rr.reason = reason.empty() ? "Customer return" : reason;
    rr.returnToSellableStock = returnToSellableStock;
    rr.refundAmount = refund;
    rr.processedByUserId = processedByUserId;
    rr.returnDate = utils::DateUtils::getCurrentDateTime();

    int retId = returnRepo_.createReturn(rr);
    if (retId <= 0) {
        res.success = false;
        res.message = "Database error creating return record.";
        return res;
    }

    auto batchOpt = batchRepo_.findById(targetItem->batchId);
    int currentBatchQty = batchOpt ? batchOpt->quantity : 0;
    int newBatchQty = currentBatchQty;

    if (returnToSellableStock && batchOpt) {
        newBatchQty += returnQuantity;
        batchRepo_.updateQuantity(targetItem->batchId, newBatchQty);
    }

    models::StockMovement sm;
    sm.medicineId = targetItem->medicineId;
    sm.batchId = targetItem->batchId;
    sm.type = returnToSellableStock ? models::MovementType::RETURN_RESTOCK : models::MovementType::RETURN_DISPOSAL;
    sm.quantity = returnQuantity;
    sm.balanceAfter = newBatchQty;
    sm.referenceType = "RETURN";
    sm.referenceId = retRef;
    sm.reason = "Return: " + reason + (returnToSellableStock ? " (Restocked to batch)" : " (Disposed/Quarantined)");
    sm.userId = processedByUserId;
    movementRepo_.recordMovement(sm);

    models::AuditLog log;
    log.userId = processedByUserId;
    log.username = "Actor #" + std::to_string(processedByUserId);
    log.action = "RETURN_PROCESSED";
    log.entityType = "RETURN";
    log.entityId = retRef;
    log.changeSummary = "Processed return " + retRef + " for " + std::to_string(returnQuantity) + " units (Refund: $" + std::to_string(refund) + ")";
    auditRepo_.logEvent(log);

    tx.commit();

    res.success = true;
    res.message = "Return processed successfully.";
    res.returnRef = retRef;
    res.refundAmount = refund;
    return res;
}

std::optional<models::Sale> SalesService::getSaleById(int saleId) {
    return saleRepo_.findById(saleId);
}

std::optional<models::Sale> SalesService::getSaleByInvoice(const std::string& invoiceNumber) {
    return saleRepo_.findByInvoice(invoiceNumber);
}

std::vector<models::Sale> SalesService::listSales(const std::string& startDate, const std::string& endDate, int limit, int offset) {
    return saleRepo_.listSales(startDate, endDate, limit, offset);
}

double SalesService::getTotalSalesRevenue(const std::string& startDate, const std::string& endDate) {
    return saleRepo_.getTotalSalesRevenue(startDate, endDate);
}

} // namespace medifind::services
