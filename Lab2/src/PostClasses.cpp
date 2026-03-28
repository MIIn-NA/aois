
#include "PostClasses.h"
#include "TruthTable.h"
#include "ZhegalkinPolynomial.h"
#include <stdexcept>

PostClassesResult PostClasses::determine(const TruthTableResult& table) {
    if (table.rows.empty()) {
        throw std::invalid_argument("Таблица истинности пуста.");
    }
    
    return PostClassesResult(
        table.rows[0].result == 0,
        table.rows.back().result == 1,
        is_self_dual(table),
        is_monotone(table),
        is_linear(table)
    );
}

bool PostClasses::is_self_dual(const TruthTableResult& table) const {
    int max_index = (1 << table.dimension) - 1;
    
    for (const auto& row : table.rows) {
        int opposite_index = max_index ^ row.index;
        int opposite_result = table.rows[opposite_index].result;
        if (row.result == opposite_result) {
            return false;
        }
    }
    return true;
}

bool PostClasses::is_monotone(const TruthTableResult& table) const {
    for (const auto& left_row : table.rows) {
        for (const auto& right_row : table.rows) {
            bool dominates = true;
            for (size_t i = 0; i < left_row.assignment.size(); i++) {
                if (left_row.assignment[i] > right_row.assignment[i]) {
                    dominates = false;
                    break;
                }
            }
            if (dominates && left_row.result > right_row.result) {
                return false;
            }
        }
    }
    return true;
}

bool PostClasses::is_linear(const TruthTableResult& table) const {
    ZhegalkinPolynomial zhegalkin;
    std::vector<int> coeffs = zhegalkin.coefficients(table);
    
    for (size_t mask = 0; mask < coeffs.size(); mask++) {
        if (coeffs[mask] == 1 && count_bits(static_cast<int>(mask)) > 1) {
            return false;
        }
    }
    return true;
}

int PostClasses::count_bits(int value) const {
    int count = 0;
    while (value > 0) {
        count += value & 1;
        value >>= 1;
    }
    return count;
}
