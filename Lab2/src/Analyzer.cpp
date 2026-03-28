
#include "Analyzer.h"

AnalysisResult Analyzer::analyze(const std::string& source) {
    ExpressionParser parser;
    ParsedExpression parsed_expression = parser.parse(source);
    
    TruthTable truth_table_service;
    TruthTableResult truth_table = truth_table_service.build(parsed_expression);
    
    CanonicalForms canonical_forms_service;
    CanonicalFormsResult canonical_forms = canonical_forms_service.build(truth_table);
    
    PostClasses post_classes_service;
    PostClassesResult post_classes = post_classes_service.determine(truth_table);
    
    ZhegalkinPolynomial zhegalkin_service;
    ZhegalkinPolynomialResult zhegalkin_polynomial = zhegalkin_service.build(truth_table);
    
    BooleanDerivative derivative_service;
    std::vector<std::string> fictitious_variables = derivative_service.find_fictitious_variables(truth_table);
    std::vector<BooleanDerivativeResult> derivatives = derivative_service.build_all(truth_table);
    
    Minimization minimization_service;
    KarnaughMap karnaugh_service;
    
    MinimizationResult sdnf_calc_min = minimization_service.minimize_by_calculation(truth_table, "sdnf");
    MinimizationResult sdnf_calc_table_min = minimization_service.minimize_by_calculation_table(truth_table, "sdnf");
    KarnaughMapResult sdnf_karnaugh_min = karnaugh_service.minimize(truth_table, "sdnf");
    
    MinimizationResult sknf_calc_min = minimization_service.minimize_by_calculation(truth_table, "sknf");
    MinimizationResult sknf_calc_table_min = minimization_service.minimize_by_calculation_table(truth_table, "sknf");
    KarnaughMapResult sknf_karnaugh_min = karnaugh_service.minimize(truth_table, "sknf");
    
    return AnalysisResult{
        parsed_expression, truth_table, canonical_forms, post_classes, zhegalkin_polynomial,
        fictitious_variables, derivatives, sdnf_calc_min, sdnf_calc_table_min, sdnf_karnaugh_min,
        sknf_calc_min, sknf_calc_table_min, sknf_karnaugh_min
    };
}
