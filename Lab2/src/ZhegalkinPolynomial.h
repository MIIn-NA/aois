
#pragma once

#include <string>
#include <vector>

struct TruthTableResult;

struct ZhegalkinPolynomialResult {
    std::vector<int> coefficients;
    std::vector<std::string> monomials;
    std::string expression;
    
    ZhegalkinPolynomialResult(
        const std::vector<int>& coeffs,
        const std::vector<std::string>& monoms,
        const std::string& expr
    ) : coefficients(coeffs), monomials(monoms), expression(expr) {}
};

class ZhegalkinPolynomial {
public:
    ZhegalkinPolynomialResult build(const TruthTableResult& table);
    std::vector<int> coefficients(const TruthTableResult& table);
    
private:
    std::string mask_to_monomial(int mask, const std::vector<std::string>& variables) const;
};
