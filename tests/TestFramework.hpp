#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

namespace medifind::test {

struct TestCase {
    std::string suiteName;
    std::string testName;
    std::function<void()> func;
};

class TestRunner {
private:
    std::vector<TestCase> tests_;
    size_t passedCount_{0};
    size_t failedCount_{0};

    TestRunner() = default;

public:
    static TestRunner& instance() {
        static TestRunner r;
        return r;
    }

    void registerTest(const std::string& suite, const std::string& name, std::function<void()> f) {
        tests_.push_back({suite, name, f});
    }

    int runAll() {
        passedCount_ = 0;
        failedCount_ = 0;
        std::cout << "\n=======================================================\n";
        std::cout << "        MEDIFIND AUTOMATED TEST SUITE RUNNER           \n";
        std::cout << "=======================================================\n";

        for (const auto& t : tests_) {
            std::cout << "[ RUN      ] " << t.suiteName << "::" << t.testName << std::flush;
            try {
                t.func();
                std::cout << " -> [ PASS ]\n";
                ++passedCount_;
            } catch (const std::exception& ex) {
                std::cout << " -> [ FAIL ]: " << ex.what() << "\n";
                ++failedCount_;
            } catch (...) {
                std::cout << " -> [ FAIL ]: Unknown fatal error\n";
                ++failedCount_;
            }
        }

        std::cout << "=======================================================\n";
        std::cout << "TEST RESULTS: " << passedCount_ << " passed, " << failedCount_ << " failed.\n";
        std::cout << "=======================================================\n\n";

        return (failedCount_ == 0) ? 0 : 1;
    }
};

struct TestRegistrar {
    TestRegistrar(const std::string& suite, const std::string& name, std::function<void()> f) {
        TestRunner::instance().registerTest(suite, name, f);
    }
};

#define TEST(Suite, Name) \
    void Suite##_##Name##_Test(); \
    static ::medifind::test::TestRegistrar Suite##_##Name##_reg(#Suite, #Name, Suite##_##Name##_Test); \
    void Suite##_##Name##_Test()

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error(std::string("Assertion failed: ") + #cond + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while(0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(val1, val2) \
    do { \
        if (!((val1) == (val2))) { \
            throw std::runtime_error(std::string("Assertion failed: ") + #val1 + " == " + #val2 + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while(0)

#define ASSERT_NEAR(val1, val2, eps) \
    do { \
        if (std::abs((val1) - (val2)) > (eps)) { \
            throw std::runtime_error(std::string("Assertion failed: |") + #val1 + " - " + #val2 + "| <= " + #eps + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while(0)

} // namespace medifind::test
