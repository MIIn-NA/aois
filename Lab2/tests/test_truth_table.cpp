
#include "TestRunner.h"
#include "../src/ExpressionParser.h"
#include "../src/TruthTable.h"

bool test_truth_table_basic() {
    ExpressionParser parser;
    auto parsed = parser.parse("a|b");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    TEST_ASSERT_EQ(2, table.dimension, "Dimension should be 2");
    TEST_ASSERT_EQ(4, table.rows.size(), "Should have 4 rows");
    TEST_ASSERT_EQ(4, table.vector.size(), "Vector should have 4 elements");
    
    // Проверка таблицы истинности для a|b
    // 00 -> 0, 01 -> 1, 10 -> 1, 11 -> 1
    TEST_ASSERT_EQ(0, table.vector[0], "Row 0 should be 0");
    TEST_ASSERT_EQ(1, table.vector[1], "Row 1 should be 1");
    TEST_ASSERT_EQ(1, table.vector[2], "Row 2 should be 1");
    TEST_ASSERT_EQ(1, table.vector[3], "Row 3 should be 1");
    
    return true;
}

bool test_truth_table_and() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    // 00 -> 0, 01 -> 0, 10 -> 0, 11 -> 1
    TEST_ASSERT_EQ(0, table.vector[0], "Row 0 should be 0");
    TEST_ASSERT_EQ(0, table.vector[1], "Row 1 should be 0");
    TEST_ASSERT_EQ(0, table.vector[2], "Row 2 should be 0");
    TEST_ASSERT_EQ(1, table.vector[3], "Row 3 should be 1");
    
    return true;
}

bool test_truth_table_not() {
    ExpressionParser parser;
    auto parsed = parser.parse("!a");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    // 0 -> 1, 1 -> 0
    TEST_ASSERT_EQ(1, table.vector[0], "Row 0 should be 1");
    TEST_ASSERT_EQ(0, table.vector[1], "Row 1 should be 0");
    
    return true;
}

bool test_truth_table_implication() {
    ExpressionParser parser;
    auto parsed = parser.parse("a->b");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    // 00 -> 1, 01 -> 1, 10 -> 0, 11 -> 1
    TEST_ASSERT_EQ(1, table.vector[0], "Row 0 should be 1");
    TEST_ASSERT_EQ(1, table.vector[1], "Row 1 should be 1");
    TEST_ASSERT_EQ(0, table.vector[2], "Row 2 should be 0");
    TEST_ASSERT_EQ(1, table.vector[3], "Row 3 should be 1");
    
    return true;
}

bool test_truth_table_complex() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    TEST_ASSERT_EQ(3, table.dimension, "Dimension should be 3");
    TEST_ASSERT_EQ(8, table.rows.size(), "Should have 8 rows");
    
    // Вектор: 01110101
    TEST_ASSERT_EQ(0, table.vector[0], "Row 0");
    TEST_ASSERT_EQ(1, table.vector[1], "Row 1");
    TEST_ASSERT_EQ(1, table.vector[2], "Row 2");
    TEST_ASSERT_EQ(1, table.vector[3], "Row 3");
    TEST_ASSERT_EQ(0, table.vector[4], "Row 4");
    TEST_ASSERT_EQ(1, table.vector[5], "Row 5");
    TEST_ASSERT_EQ(0, table.vector[6], "Row 6");
    TEST_ASSERT_EQ(1, table.vector[7], "Row 7");
    
    return true;
}

void register_truth_table_tests(TestRunner& runner) {
    std::cout << "\n[Тесты таблиц истинности]" << std::endl;
    runner.run_test("Базовая таблица (a|b)", test_truth_table_basic);
    runner.run_test("Таблица И (a&b)", test_truth_table_and);
    runner.run_test("Таблица НЕ (!a)", test_truth_table_not);
    runner.run_test("Таблица импликации (a->b)", test_truth_table_implication);
    runner.run_test("Сложная формула (!(!a->!b)|c)", test_truth_table_complex);
}
