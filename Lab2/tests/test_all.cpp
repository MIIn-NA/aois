
#include "TestRunner.h"

// Объявления функций регистрации тестов
void register_truth_table_tests(TestRunner& runner);
void register_canonical_forms_tests(TestRunner& runner);
void register_post_classes_tests(TestRunner& runner);
void register_zhegalkin_tests(TestRunner& runner);
void register_minimization_tests(TestRunner& runner);
void register_integration_tests(TestRunner& runner);  // НОВОЕ!

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "ТЕСТИРОВАНИЕ ЛАБОРАТОРНОЙ РАБОТЫ 2" << std::endl;
    std::cout << "========================================" << std::endl;
    
    TestRunner runner;
    
    // Запуск всех тестов
    register_truth_table_tests(runner);
    register_canonical_forms_tests(runner);
    register_post_classes_tests(runner);
    register_zhegalkin_tests(runner);
    register_minimization_tests(runner);
    register_integration_tests(runner);  // НОВОЕ!
    
    // Вывод итогов
    runner.print_summary();
    
    return runner.all_passed() ? 0 : 1;
}
