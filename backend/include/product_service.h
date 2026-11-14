#pragma once
#include "product.h"
#include "repositories/product_repository.h"
#include <string>
#include <vector>
#include <optional>

class ProductService {
private:
    ProductRepository& productRepo;

public:
    explicit ProductService(ProductRepository& repo) : productRepo(repo) {}

    std::vector<Product> listAvailableProducts() {
        return productRepo.findByCategory(ProductCategory::Medicine);
    }

    std::vector<Product> listHealthcareProducts() {
        return productRepo.findByCategory(ProductCategory::Healthcare);
    }

    std::optional<Product> getProduct(int id) {
        return productRepo.findById(id);
    }

    int createProduct(const std::string& name, const std::string& brand,
                      ProductCategory category, const std::string& manufacturer,
                      const std::string& strength, const std::string& dosageForm,
                      bool prescriptionRequired, const std::string& description,
                      const std::string& imageUrl) {
        if (name.empty() || brand.empty() || manufacturer.empty()) {
            return -1;
        }
        if (productRepo.existsByNameAndBrand(name, brand)) {
            return -2; // duplicate
        }
        Product p;
        p.setId(0);
        p.name = name;
        p.brand = brand;
        p.category = category;
        p.manufacturer = manufacturer;
        p.strength = strength;
        p.dosageForm = dosageForm;
        p.prescriptionRequired = prescriptionRequired;
        p.description = description;
        p.imageUrl = imageUrl;
        p.isArchived = false;
        p.createdAt = currentTimeIso();
        return productRepo.create(p);
    }

    bool archiveProduct(int id) {
        auto opt = productRepo.findById(id);
        if (!opt.has_value()) return false;
        Product p = opt.value();
        p.isArchived = true;
        return productRepo.update(id, p);
    }

    bool unarchiveProduct(int id) {
        auto opt = productRepo.findById(id);
        if (!opt.has_value()) return false;
        Product p = opt.value();
        p.isArchived = false;
        return productRepo.update(id, p);
    }

    bool updateProduct(int id, const std::string& name, const std::string& brand,
                       const std::string& manufacturer, const std::string& strength,
                       const std::string& dosageForm, bool prescriptionRequired,
                       const std::string& description, const std::string& imageUrl) {
        auto opt = productRepo.findById(id);
        if (!opt.has_value()) return false;
        Product p = opt.value();
        p.name = name;
        p.brand = brand;
        p.manufacturer = manufacturer;
        p.strength = strength;
        p.dosageForm = dosageForm;
        p.prescriptionRequired = prescriptionRequired;
        p.description = description;
        p.imageUrl = imageUrl;
        return productRepo.update(id, p);
    }

private:
    static std::string currentTimeIso() {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&timeT), "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }
};

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
