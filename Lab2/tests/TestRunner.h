
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    
    TestResult(const std::string& n, bool p, const std::string& msg = "")
        : name(n), passed(p), message(msg) {}
};

class TestRunner {
private:
    std::vector<TestResult> results;
    int passed_count = 0;
    int failed_count = 0;
    
public:
    void run_test(const std::string& name, std::function<bool()> test_func) {
        try {
            bool passed = test_func();
            if (passed) {
                results.emplace_back(name, true, "✓");
                passed_count++;
                std::cout << "  ✓ " << name << std::endl;
            } else {
                results.emplace_back(name, false, "✗");
                failed_count++;
                std::cout << "  ✗ " << name << std::endl;
            }
        } catch (const std::exception& e) {
            results.emplace_back(name, false, std::string("Exception: ") + e.what());
            failed_count++;
            std::cout << "  ✗ " << name << " (Exception: " << e.what() << ")" << std::endl;
        }
    }
    
    void print_summary() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "ИТОГИ ТЕСТИРОВАНИЯ" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Всего тестов: " << (passed_count + failed_count) << std::endl;
        std::cout << "Пройдено: " << passed_count << " ✓" << std::endl;
        std::cout << "Провалено: " << failed_count << " ✗" << std::endl;
        std::cout << "========================================" << std::endl;
        
        if (failed_count > 0) {
            std::cout << "\nНеудачные тесты:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << ": " << result.message << std::endl;
                }
            }
        }
    }
    
    bool all_passed() const {
        return failed_count == 0;
    }
};

// Макросы для упрощения написания тестов
#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " << message << std::endl; \
        return false; \
    }

#define TEST_ASSERT_EQ(expected, actual, message) \
    if ((expected) != (actual)) { \
        std::cerr << "Assertion failed: " << message << std::endl; \
        std::cerr << "  Expected: " << expected << std::endl; \
        std::cerr << "  Actual: " << actual << std::endl; \
        return false; \
    }
