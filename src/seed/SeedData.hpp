#pragma once

#include "database/DatabaseConnection.hpp"
#include "services/AuthService.hpp"
#include "services/InventoryService.hpp"
#include "services/PurchasingService.hpp"
#include "services/SalesService.hpp"

namespace medifind::seed {

class SeedData {
public:
    /**
     * @brief Seeds demo data into empty database.
     * Idempotent: Does not insert duplicates if users or medicines already exist.
     */
    static void seedDemoData(database::DatabaseConnection& db,
                             services::AuthService& authService,
                             services::InventoryService& invService,
                             services::PurchasingService& purchService,
                             services::SalesService& salesService);
};

} // namespace medifind::seed
