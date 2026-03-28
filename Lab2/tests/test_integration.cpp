
#include "TestRunner.h"
#include "../src/Analyzer.h"
#include "../src/ReportBuilder.h"

bool test_analyzer_basic() {
    Analyzer analyzer;
    auto result = analyzer.analyze("a|b");
    
    TEST_ASSERT(!result.parsed_expression.source.empty(), "Source should not be empty");
    TEST_ASSERT(result.truth_table.rows.size() == 4, "Should have 4 rows");
    TEST_ASSERT(!result.canonical_forms.sdnf.empty(), "SDNF should not be empty");
    TEST_ASSERT(!result.canonical_forms.sknf.empty(), "SKNF should not be empty");
    
    return true;
}

bool test_analyzer_complex() {
    Analyzer analyzer;
    auto result = analyzer.analyze("!(!a->!b)|c");
    
    TEST_ASSERT(result.truth_table.dimension == 3, "Dimension should be 3");
    TEST_ASSERT(result.truth_table.rows.size() == 8, "Should have 8 rows");
    TEST_ASSERT(result.canonical_forms.index_number == 117, "Index should be 117");
    
    return true;
}

bool test_analyzer_post_classes() {
    Analyzer analyzer;
    auto result = analyzer.analyze("a&b");
    
    TEST_ASSERT(result.post_classes.t0 == true, "Should preserve 0");
    TEST_ASSERT(result.post_classes.t1 == false, "Should not preserve 1");
    
    return true;
}

bool test_analyzer_zhegalkin() {
    Analyzer analyzer;
    auto result = analyzer.analyze("a&b|c&d");
    
    TEST_ASSERT(!result.zhegalkin_polynomial.expression.empty(), "Polynomial should not be empty");
    
    return true;
}

bool test_analyzer_derivatives() {
    Analyzer analyzer;
    auto result = analyzer.analyze("a|b");
    
    TEST_ASSERT(!result.derivatives.empty(), "Should have derivatives");
    TEST_ASSERT(result.derivatives.size() >= 2, "Should have at least 2 derivatives");
    
    return true;
}

bool test_analyzer_minimization() {
    Analyzer analyzer;
    auto result = analyzer.analyze("!(!a->!b)|c");
    
    TEST_ASSERT(!result.sdnf_calculation_minimization.expression.empty(), "SDNF minimization should not be empty");
    TEST_ASSERT(!result.sknf_calculation_minimization.expression.empty(), "SKNF minimization should not be empty");
    TEST_ASSERT(!result.sdnf_karnaugh_minimization.expression.empty(), "Karnaugh minimization should not be empty");
    
    return true;
}

bool test_analyzer_fictitious() {
    Analyzer analyzer;
    auto result = analyzer.analyze("a&b");
    
    // В a&b нет фиктивных переменных
    TEST_ASSERT(result.fictitious_variables.empty(), "Should have no fictitious variables");
    
    return true;
}

bool test_report_builder() {
    Analyzer analyzer;
    ReportBuilder builder;
    
    auto result = analyzer.analyze("a|b&c");
    std::string report = builder.build(result);
    
    TEST_ASSERT(!report.empty(), "Report should not be empty");
    TEST_ASSERT(report.find("Таблица истинности") != std::string::npos, "Should contain truth table");
    TEST_ASSERT(report.find("СДНФ") != std::string::npos, "Should contain SDNF");
    TEST_ASSERT(report.find("СКНФ") != std::string::npos, "Should contain SKNF");
    TEST_ASSERT(report.find("Классы Поста") != std::string::npos, "Should contain Post classes");
    TEST_ASSERT(report.find("Полином Жегалкина") != std::string::npos, "Should contain Zhegalkin");
    TEST_ASSERT(report.find("Минимизация") != std::string::npos, "Should contain minimization");
    
    return true;
}

bool test_report_builder_complex() {
    Analyzer analyzer;
    ReportBuilder builder;
    
    auto result = analyzer.analyze("!(!a->!b)|c");
    std::string report = builder.build(result);
    
    TEST_ASSERT(report.find("117") != std::string::npos, "Should contain index 117");
    TEST_ASSERT(report.find("c|(!a&b)") != std::string::npos, "Should contain minimized expression");
    
    return true;
}

void register_integration_tests(TestRunner& runner) {
    std::cout << "\n[Интеграционные тесты]" << std::endl;
    runner.run_test("Analyzer: базовый", test_analyzer_basic);
    runner.run_test("Analyzer: сложная формула", test_analyzer_complex);
    runner.run_test("Analyzer: классы Поста", test_analyzer_post_classes);
    runner.run_test("Analyzer: полином Жегалкина", test_analyzer_zhegalkin);
    runner.run_test("Analyzer: производные", test_analyzer_derivatives);
    runner.run_test("Analyzer: минимизация", test_analyzer_minimization);
    runner.run_test("Analyzer: фиктивные переменные", test_analyzer_fictitious);
    runner.run_test("ReportBuilder: базовый", test_report_builder);
    runner.run_test("ReportBuilder: сложный", test_report_builder_complex);
}
