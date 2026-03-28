
#include "BooleanDerivative.h"
#include "TruthTable.h"
#include "CanonicalForms.h"
#include <stdexcept>
#include <sstream>

std::vector<std::string> BooleanDerivative::find_fictitious_variables(const TruthTableResult& table) {
    if (table.dimension == 0) {
        return {};
    }

    std::vector<std::string> fictitious;
    for (size_t index = 0; index < table.variables.size(); index++) {
        int bit_mask = 1 << (table.dimension - 1 - index);
        if (is_fictitious(table, bit_mask)) {
            fictitious.push_back(table.variables[index]);
        }
    }
    return fictitious;
}

std::vector<BooleanDerivativeResult> BooleanDerivative::build_all(const TruthTableResult& table, int max_order) {
    std::vector<BooleanDerivativeResult> derivatives;
    int supported_order = std::min(max_order, table.dimension);

    for (int order = 1; order <= supported_order; order++) {
        auto variable_groups = combinations(table.variables, order);
        for (const auto& variables_group : variable_groups) {
            derivatives.push_back(build(table, variables_group));
        }
    }
    return derivatives;
}

BooleanDerivativeResult BooleanDerivative::build(const TruthTableResult& table, const std::vector<std::string>& by_variables) {
    if (by_variables.empty()) {
        throw std::invalid_argument("Для вычисления производной нужна хотя бы одна переменная.");
    }

    std::map<std::string, int> variable_to_mask;
    for (size_t index = 0; index < table.variables.size(); index++) {
        variable_to_mask[table.variables[index]] = 1 << (table.dimension - 1 - index);
    }

    std::vector<std::string> unknown_variables;
    for (const auto& name : by_variables) {
        if (variable_to_mask.find(name) == variable_to_mask.end()) {
            unknown_variables.push_back(name);
        }
    }
    if (!unknown_variables.empty()) {
        std::ostringstream oss;
        oss << "Неизвестные переменные: ";
        for (size_t i = 0; i < unknown_variables.size(); i++) {
            if (i > 0) oss << ", ";
            oss << unknown_variables[i];
        }
        throw std::invalid_argument(oss.str());
    }

    std::vector<int> masks;
    for (const auto& name : by_variables) {
        masks.push_back(variable_to_mask[name]);
    }
    auto xor_masks = build_masks(masks);
    
    std::vector<int> derivative_vector;
    int table_size = 1 << table.dimension;
    
    for (int index = 0; index < table_size; index++) {
        int value = 0;
        for (int mask : xor_masks) {
            value ^= table.vector[index ^ mask];
        }
        derivative_vector.push_back(value);
    }

    TruthTable truth_table_service;
    TruthTableResult derivative_table = truth_table_service.from_vector(table.variables, derivative_vector);
    
    CanonicalForms canonical_service;
    CanonicalFormsResult canonical = canonical_service.build(derivative_table);

    return BooleanDerivativeResult(by_variables, static_cast<int>(by_variables.size()),
        derivative_vector, canonical.sdnf, canonical.sdnf_numeric,
        canonical.index_vector, canonical.index_number);
}

bool BooleanDerivative::is_fictitious(const TruthTableResult& table, int bit_mask) const {
    int table_size = 1 << table.dimension;
    for (int index = 0; index < table_size; index++) {
        if (index & bit_mask) continue;
        if (table.vector[index] != table.vector[index | bit_mask]) {
            return false;
        }
    }
    return true;
}

std::vector<int> BooleanDerivative::build_masks(const std::vector<int>& bits) const {
    std::vector<int> masks = {0};
    for (int bit : bits) {
        size_t current_size = masks.size();
        for (size_t i = 0; i < current_size; i++) {
            masks.push_back(masks[i] | bit);
        }
    }
    return masks;
}

std::vector<std::vector<std::string>> BooleanDerivative::combinations(const std::vector<std::string>& elements, int r) const {
    std::vector<std::vector<std::string>> result;
    int n = static_cast<int>(elements.size());
    if (r > n) return result;
    
    std::vector<int> indices(r);
    for (int i = 0; i < r; i++) indices[i] = i;
    
    while (true) {
        std::vector<std::string> combination;
        for (int i = 0; i < r; i++) {
            combination.push_back(elements[indices[i]]);
        }
        result.push_back(combination);
        
        int i = r - 1;
        while (i >= 0 && indices[i] == i + n - r) i--;
        if (i < 0) break;
        
        indices[i]++;
        for (int j = i + 1; j < r; j++) {
            indices[j] = indices[j - 1] + 1;
        }
    }
    return result;
}
