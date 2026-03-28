
#pragma once

#include <string>
#include <vector>

#include "ExpressionParser.h"
#include "TruthTable.h"
#include "CanonicalForms.h"
#include "PostClasses.h"
#include "ZhegalkinPolynomial.h"
#include "BooleanDerivative.h"
#include "Minimization.h"
#include "KarnaughMap.h"

struct AnalysisResult {
    ParsedExpression parsed_expression;
    TruthTableResult truth_table;
    CanonicalFormsResult canonical_forms;
    PostClassesResult post_classes;
    ZhegalkinPolynomialResult zhegalkin_polynomial;
    std::vector<std::string> fictitious_variables;
    std::vector<BooleanDerivativeResult> derivatives;
    MinimizationResult sdnf_calculation_minimization;
    MinimizationResult sdnf_calculation_table_minimization;
    KarnaughMapResult sdnf_karnaugh_minimization;
    MinimizationResult sknf_calculation_minimization;
    MinimizationResult sknf_calculation_table_minimization;
    KarnaughMapResult sknf_karnaugh_minimization;
};

class Analyzer {
public:
    AnalysisResult analyze(const std::string& source);
};
