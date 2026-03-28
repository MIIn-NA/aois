
#include "TestRunner.h"
#include "../src/ExpressionParser.h"
#include "../src/TruthTable.h"
#include "../src/ZhegalkinPolynomial.h"

bool test_zhegalkin_xor() {
    ExpressionParser parser;
    auto parsed = parser.parse("a^b");  // XOR (но у нас нет ^, используем эквивалентность)
    
    // a~b это эквивалентность, а XOR это !(a~b)
    auto parsed2 = parser.parse("!(a~b)");
    
    TruthTable tt;
    auto table = tt.build(parsed2);
    
    ZhegalkinPolynomial zp;
    auto result = zp.build(table);
    
    // XOR = a ^ b в полиноме Жегалкина
    TEST_ASSERT(!result.expression.empty(), "Polynomial should not be empty");
    
    return true;
}

bool test_zhegalkin_and() {
    ExpressionParser parser;
    auto parsed = parser.parse("a&b");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    ZhegalkinPolynomial zp;
    auto result = zp.build(table);
    
    // a&b = a*b в полиноме Жегалкина
    TEST_ASSERT(result.expression.find("a") != std::string::npos, "Should contain 'a'");
    TEST_ASSERT(result.expression.find("b") != std::string::npos, "Should contain 'b'");
    
    return true;
}

bool test_zhegalkin_complex() {
    ExpressionParser parser;
    auto parsed = parser.parse("!(!a->!b)|c");
    
    TruthTable tt;
    auto table = tt.build(parsed);
    
    ZhegalkinPolynomial zp;
    auto result = zp.build(table);
    
    TEST_ASSERT(!result.expression.empty(), "Polynomial should not be empty");
    TEST_ASSERT(result.expression.find("c") != std::string::npos, "Should contain 'c'");
    
    return true;
}

void register_zhegalkin_tests(TestRunner& runner) {
    std::cout << "\n[Тесты полинома Жегалкина]" << std::endl;
    runner.run_test("XOR функция", test_zhegalkin_xor);
    runner.run_test("AND функция", test_zhegalkin_and);
    runner.run_test("Сложная функция", test_zhegalkin_complex);
}
