
#include "TestRunner.h"
#include "../src/ExpressionParser.h"
#include "../src/TruthTable.h"
#include "../src/Minimization.h"
#include "../src/KarnaughMap.h"

bool test_minimization_sdnf() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    Minimization min;
    auto result = min.minimize_by_calculation(table, "sdnf");
    
    TEST_ASSERT(!result.expression.empty(), "Minimized expression should not be empty");
    TEST_ASSERT(!result.stages.empty(), "Should have gluing stages");
    
    return true;
}

bool test_minimization_sknf() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    Minimization min;
    auto result = min.minimize_by_calculation(table, "sknf");
    
    TEST_ASSERT(!result.expression.empty(), "Minimized expression should not be empty");
    
    return true;
}

bool test_minimization_table_method() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b|c&d");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    Minimization min;
    auto result = min.minimize_by_calculation_table(table, "sdnf");
    
    TEST_ASSERT(result.chart != nullptr, "Chart should not be null");
    TEST_ASSERT(!result.selected_implicants.empty(), "Should have selected implicants");
    
    return true;
}

bool test_karnaugh_map() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b|c&d");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    KarnaughMap km;
    auto result = km.minimize(table, "sdnf");
    
    TEST_ASSERT(!result.rendered_map.empty(), "Map should not be empty");
    TEST_ASSERT(!result.groups.empty(), "Should have groups");
    
    return true;
}

bool test_minimization_equivalence() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    Minimization min;
    KarnaughMap km;
    
    auto result_calc = min.minimize_by_calculation(table, "sdnf");
    auto result_karnaugh = km.minimize(table, "sdnf");
    
    // Оба метода должны дать одинаковый результат
    TEST_ASSERT(result_calc.expression == result_karnaugh.expression, 
        "Calculation and Karnaugh should give same result");
    
    return true;
}

void register_minimization_tests(TestRunner& runner) {
    std::cout << "\n[Тесты минимизации]" << std::endl;
    runner.run_test("Минимизация СДНФ (расчетный)", test_minimization_sdnf);
    runner.run_test("Минимизация СКНФ (расчетный)", test_minimization_sknf);
    runner.run_test("Минимизация (табличный)", test_minimization_table_method);
    runner.run_test("Карта Карно", test_karnaugh_map);
    runner.run_test("Эквивалентность методов", test_minimization_equivalence);
}
