#pragma once
#include "product.h"
#include "repositories/product_repository.h"
#include <string>
#include <vector>
#include <optional>
#include <map>

// MedicineService - manages medicine details and prescription requirements
class MedicineService {
private:
    ProductRepository& productRepo;

public:
    explicit MedicineService(ProductRepository& repo) : productRepo(repo) {}

    std::vector<Product> listPrescriptionRequired() {
        std::vector<Product> result;
        for (auto& p : productRepo.findByCategory(ProductCategory::Medicine)) {
            if (p.prescriptionRequired && !p.isArchived) {
                result.push_back(p);
            }
        }
        return result;
    }

    std::vector<Product> listOverTheCounter() {
        std::vector<Product> result;
        for (auto& p : productRepo.findByCategory(ProductCategory::Medicine)) {
            if (!p.prescriptionRequired && !p.isArchived) {
                result.push_back(p);
            }
        }
        return result;
    }

    bool isPrescriptionRequired(int productId) {
        auto opt = productRepo.findById(productId);
        return opt.has_value() && opt.value().prescriptionRequired;
    }

    bool isMedicine(int productId) {
        auto opt = productRepo.findById(productId);
        return opt.has_value() && opt.value().category == ProductCategory::Medicine;
    }
};
