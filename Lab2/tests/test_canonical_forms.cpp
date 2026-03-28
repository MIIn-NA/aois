
#include "TestRunner.h"
#include "../src/ExpressionParser.h"
#include "../src/TruthTable.h"
#include "../src/CanonicalForms.h"

bool test_sdnf_basic() {
    ExpressionParser parser;
    auto parsed = parser.parse("a|b");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    CanonicalForms cf;
    auto result = cf.build(table);
    
    // СДНФ для a|b: (!a&b)|(a&!b)|(a&b)
    TEST_ASSERT(!result.sdnf.empty(), "SDNF should not be empty");
    TEST_ASSERT_EQ(3, result.sdnf_numeric.size(), "SDNF should have 3 terms");
    
    return true;
}

bool test_sknf_basic() {
    ExpressionParser parser;
    auto parsed = parser.parse("a|b");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    CanonicalForms cf;
    auto result = cf.build(table);
    
    // СКНФ для a|b: (a|b)
    TEST_ASSERT(!result.sknf.empty(), "SKNF should not be empty");
    TEST_ASSERT_EQ(1, result.sknf_numeric.size(), "SKNF should have 1 term");
    
    return true;
}

bool test_index_form() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    CanonicalForms cf;
    auto result = cf.build(table);
    
    TEST_ASSERT_EQ("01110101", result.index_vector, "Index vector should match");
    TEST_ASSERT_EQ(117, result.index_number, "Index number should be 117");
    
    return true;
}

bool test_numeric_forms() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    CanonicalForms cf;
    auto result = cf.build(table);
    
    // СДНФ: (1, 2, 3, 5, 7)
    TEST_ASSERT_EQ(5, result.sdnf_numeric.size(), "SDNF should have 5 terms");
    
    // СКНФ: (0, 4, 6)
    TEST_ASSERT_EQ(3, result.sknf_numeric.size(), "SKNF should have 3 terms");
    
    return true;
}

void register_canonical_forms_tests(TestRunner& runner) {
    std::cout << "\n[Тесты канонических форм]" << std::endl;
    runner.run_test("СДНФ базовая", test_sdnf_basic);
    runner.run_test("СКНФ базовая", test_sknf_basic);
    runner.run_test("Индексная форма", test_index_form);
    runner.run_test("Числовые формы", test_numeric_forms);
}
