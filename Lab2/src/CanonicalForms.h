
#pragma once

#include <string>
#include <vector>

struct TruthRow;
struct TruthTableResult;

struct CanonicalFormsResult {
    std::string sdnf;
    std::string sknf;
    std::vector<int> sdnf_numeric;
    std::vector<int> sknf_numeric;
    std::string index_vector;
    int index_number;
    
    CanonicalFormsResult(
        const std::string& s, const std::string& k,
        const std::vector<int>& s_num, const std::vector<int>& k_num,
        const std::string& idx_vec, int idx_num
    ) : sdnf(s), sknf(k), sdnf_numeric(s_num), sknf_numeric(k_num),
        index_vector(idx_vec), index_number(idx_num) {}
};

class CanonicalForms {
public:
    CanonicalFormsResult build(const TruthTableResult& table);
    
private:
    std::string build_sdnf_term(
        const std::vector<std::string>& variables,
        const TruthRow& row
    );
    std::string build_sknf_term(
        const std::vector<std::string>& variables,
        const TruthRow& row
    );
};
