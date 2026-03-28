
#include "TestRunner.h"
#include "../src/ExpressionParser.h"
#include "../src/TruthTable.h"
#include "../src/PostClasses.h"

bool test_post_t0() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b");  // Сохраняет 0
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    PostClasses pc;
    auto result = pc.determine(table);
    
    TEST_ASSERT(result.t0 == true, "a&b should preserve 0");
    
    return true;
}

bool test_post_t1() {
    ExpressionParser parser;
    auto parsed = parser.parse("a|b");  // Сохраняет 1
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    PostClasses pc;
    auto result = pc.determine(table);
    
    TEST_ASSERT(result.t1 == true, "a|b should preserve 1");
    
    return true;
}

bool test_post_monotone() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b|c&d");  // Монотонная
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    PostClasses pc;
    auto result = pc.determine(table);
    
    TEST_ASSERT(result.m == true, "a&b|c&d should be monotone");
    
    return true;
}

bool test_post_not_linear() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b");  // Не линейная
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    PostClasses pc;
    auto result = pc.determine(table);
    
    TEST_ASSERT(result.l == false, "a&b should not be linear");
    
    return true;
}

void register_post_classes_tests(TestRunner& runner) {
    std::cout << "\n[Тесты классов Поста]" << std::endl;
    runner.run_test("Класс T0", test_post_t0);
    runner.run_test("Класс T1", test_post_t1);
    runner.run_test("Класс M (монотонность)", test_post_monotone);
    runner.run_test("Класс L (линейность)", test_post_not_linear);
}
