
#pragma once

#include <string>
#include <vector>

struct TruthTableResult;
struct CanonicalFormsResult;

struct BooleanDerivativeResult {
    std::vector<std::string> by_variables;
    int order;
    std::vector<int> vector;
    std::string sdnf;
    std::vector<int> numeric_form;
    std::string index_vector;
    int index_number;
    
    BooleanDerivativeResult(
        const std::vector<std::string>& vars, int ord,
        const std::vector<int>& vec, const std::string& s,
        const std::vector<int>& num, const std::string& idx_vec, int idx_num
    ) : by_variables(vars), order(ord), vector(vec), sdnf(s),
        numeric_form(num), index_vector(idx_vec), index_number(idx_num) {}
};

class BooleanDerivative {
public:
    std::vector<std::string> find_fictitious_variables(const TruthTableResult& table);
    std::vector<BooleanDerivativeResult> build_all(const TruthTableResult& table, int max_order = 4);
    BooleanDerivativeResult build(const TruthTableResult& table, const std::vector<std::string>& by_variables);
    
private:
    bool is_fictitious(const TruthTableResult& table, int bit_mask) const;
    std::vector<int> build_masks(const std::vector<int>& bits) const;
    std::vector<std::vector<std::string>> combinations(const std::vector<std::string>& elements, int r) const;
};
