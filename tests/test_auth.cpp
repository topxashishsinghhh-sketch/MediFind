#include "TestFramework.hpp"
#include "database/DatabaseConnection.hpp"
#include "database/SchemaMigrations.hpp"
#include "repositories/UserRepository.hpp"
#include "repositories/AuditLogRepository.hpp"
#include "services/AuthService.hpp"
#include <cstdio>

using namespace medifind;

TEST(AuthSuite, UserCreationAndAuthentication) {
    const std::string dbFile = "test_auth.db";
    std::remove(dbFile.c_str());

    {
        database::DatabaseConnection db(dbFile);
        database::SchemaMigrations::applyMigrations(db);

        repositories::UserRepository userRepo(db);
        repositories::AuditLogRepository auditRepo(db);
        services::AuthService auth(userRepo, auditRepo);

        // 1. Bootstrap admin
        ASSERT_TRUE(auth.bootstrapAdminIfEmpty("sysadmin", "SecureAdminPass!1", "Head Admin"));
        ASSERT_FALSE(auth.bootstrapAdminIfEmpty("another", "Pass", "Another")); // Idempotent check

        // 2. Login fails on wrong password
        auto failLogin = auth.login("sysadmin", "WrongPass");
        ASSERT_FALSE(failLogin.success);
        ASSERT_FALSE(auth.isAuthenticated());

        // 3. Login succeeds on right password
        auto okLogin = auth.login("sysadmin", "SecureAdminPass!1");
        ASSERT_TRUE(okLogin.success);
        ASSERT_TRUE(auth.isAuthenticated());
        ASSERT_TRUE(auth.isAdmin());

        // 4. Register Employee
        auto regEmp = auth.registerUser("tech1", "StaffPass!2", "Alex Morgan", models::UserRole::EMPLOYEE);
        ASSERT_TRUE(regEmp.success);
        ASSERT_EQ(regEmp.user->role, models::UserRole::EMPLOYEE);

        // 5. Change Password
        std::string err;
        ASSERT_TRUE(auth.changePassword("SecureAdminPass!1", "NewSuperAdminPass!9", err));
        ASSERT_FALSE(auth.changePassword("WrongOld", "Whatever", err));

        auth.logout();
        ASSERT_FALSE(auth.isAuthenticated());

        // Verify new password works
        auto reLogin = auth.login("sysadmin", "NewSuperAdminPass!9");
        ASSERT_TRUE(reLogin.success);
    }

    std::remove(dbFile.c_str());
}
