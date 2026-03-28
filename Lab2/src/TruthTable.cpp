
#include "TruthTable.h"
#include "ExpressionParser.h"
#include <stdexcept>

TruthTableResult TruthTable::build(const ParsedExpression& parsed_expression) {
    const auto& variables = parsed_expression.variables;
    
    if (variables.empty()) {
        std::map<std::string, int> empty_assignment;
        int result = parsed_expression.root->evaluate(empty_assignment);
        std::vector<TruthRow> rows;
        rows.emplace_back(std::vector<int>(), result, 0);
        return TruthTableResult(variables, rows);
    }
    
    std::vector<TruthRow> rows;
    int num_variables = static_cast<int>(variables.size());
    int num_rows = 1 << num_variables;
    
    for (int i = 0; i < num_rows; i++) {
        std::vector<int> values = index_to_assignment(i, num_variables);
        
        std::map<std::string, int> assignment;
        for (size_t j = 0; j < variables.size(); j++) {
            assignment[variables[j]] = values[j];
        }
        
        int result = parsed_expression.root->evaluate(assignment);
        int index = assignment_to_index(values);
        
        rows.emplace_back(values, result, index);
    }
    
    return TruthTableResult(variables, rows);
}

TruthTableResult TruthTable::from_vector(
    const std::vector<std::string>& variables,
    const std::vector<int>& vec
) {
    std::vector<int> normalized_vector;
    normalized_vector.reserve(vec.size());
    
    for (int value : vec) {
        normalized_vector.push_back(value ? 1 : 0);
    }
    
    int expected_size = 1 << static_cast<int>(variables.size());
    
    if (static_cast<int>(normalized_vector.size()) != expected_size) {
        throw std::invalid_argument(
            "Размер вектора должен быть " + std::to_string(expected_size) + 
            ", получено " + std::to_string(normalized_vector.size()) + "."
        );
    }
    
    std::vector<TruthRow> rows;
    rows.reserve(normalized_vector.size());
    
    for (size_t index = 0; index < normalized_vector.size(); index++) {
        std::vector<int> assignment = index_to_assignment(
            static_cast<int>(index),
            static_cast<int>(variables.size())
        );
        rows.emplace_back(assignment, normalized_vector[index], static_cast<int>(index));
    }
    
    return TruthTableResult(variables, rows);
}

int TruthTable::assignment_to_index(const std::vector<int>& assignment) const {
    int index = 0;
    for (int value : assignment) {
        index = (index << 1) | (value ? 1 : 0);
    }
    return index;
}

std::vector<int> TruthTable::index_to_assignment(int index, int dimension) const {
    std::vector<int> assignment;
    assignment.reserve(dimension);
    
    for (int bit = dimension - 1; bit >= 0; bit--) {
        assignment.push_back((index >> bit) & 1);
    }
    
    return assignment;
}
