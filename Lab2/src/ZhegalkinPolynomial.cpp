
#include "ZhegalkinPolynomial.h"
#include "TruthTable.h"
#include <sstream>

ZhegalkinPolynomialResult ZhegalkinPolynomial::build(const TruthTableResult& table) {
    std::vector<int> coeffs = coefficients(table);
    
    std::vector<std::string> monomials;
    for (size_t mask = 0; mask < coeffs.size(); mask++) {
        if (coeffs[mask] == 1) {
            monomials.push_back(mask_to_monomial(static_cast<int>(mask), table.variables));
        }
    }
    
    std::string expression;
    if (monomials.empty()) {
        expression = "0";
    } else {
        std::ostringstream oss;
        for (size_t i = 0; i < monomials.size(); i++) {
            if (i > 0) oss << " ^ ";
            oss << monomials[i];
        }
        expression = oss.str();
    }
    
    return ZhegalkinPolynomialResult(coeffs, monomials, expression);
}

std::vector<int> ZhegalkinPolynomial::coefficients(const TruthTableResult& table) {
    std::vector<int> coeffs = table.vector;
    int dimension = table.dimension;
    int size = 1 << dimension;
    
    for (int bit = 0; bit < dimension; bit++) {
        int bit_mask = 1 << bit;
        for (int mask = 0; mask < size; mask++) {
            if (mask & bit_mask) {
                coeffs[mask] ^= coeffs[mask ^ bit_mask];
            }
        }
    }
    
    return coeffs;
}

std::string ZhegalkinPolynomial::mask_to_monomial(
    int mask,
    const std::vector<std::string>& variables
) const {
    if (mask == 0) {
        return "1";
    }
    
    std::vector<std::string> parts;
    int dimension = static_cast<int>(variables.size());
    
    for (int index = 0; index < dimension; index++) {
        int bit = 1 << (dimension - 1 - index);
        if (mask & bit) {
            parts.push_back(variables[index]);
        }
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i > 0) oss << "*";
        oss << parts[i];
    }
    return oss.str();
}
