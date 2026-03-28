
#include "CanonicalForms.h"
#include "TruthTable.h"
#include <sstream>

CanonicalFormsResult CanonicalForms::build(const TruthTableResult& table) {
    std::vector<TruthRow> sdnf_rows;
    std::vector<TruthRow> sknf_rows;
    
    for (const auto& row : table.rows) {
        if (row.result == 1) {
            sdnf_rows.push_back(row);
        } else {
            sknf_rows.push_back(row);
        }
    }
    
    std::vector<std::string> sdnf_terms;
    std::vector<std::string> sknf_terms;
    
    for (const auto& row : sdnf_rows) {
        sdnf_terms.push_back(build_sdnf_term(table.variables, row));
    }
    for (const auto& row : sknf_rows) {
        sknf_terms.push_back(build_sknf_term(table.variables, row));
    }
    
    std::string sdnf;
    if (sdnf_terms.empty()) {
        sdnf = "0";
    } else {
        std::ostringstream oss;
        for (size_t i = 0; i < sdnf_terms.size(); i++) {
            if (i > 0) oss << "|";
            oss << sdnf_terms[i];
        }
        sdnf = oss.str();
    }
    
    std::string sknf;
    if (sknf_terms.empty()) {
        sknf = "1";
    } else {
        std::ostringstream oss;
        for (size_t i = 0; i < sknf_terms.size(); i++) {
            if (i > 0) oss << "&";
            oss << sknf_terms[i];
        }
        sknf = oss.str();
    }
    
    std::string index_vector;
    for (const auto& row : table.rows) {
        index_vector += std::to_string(row.result);
    }
    
    int index_number = 0;
    if (!index_vector.empty()) {
        index_number = std::stoi(index_vector, nullptr, 2);
    }
    
    std::vector<int> sdnf_numeric;
    std::vector<int> sknf_numeric;
    for (const auto& row : sdnf_rows) {
        sdnf_numeric.push_back(row.index);
    }
    for (const auto& row : sknf_rows) {
        sknf_numeric.push_back(row.index);
    }
    
    return CanonicalFormsResult(sdnf, sknf, sdnf_numeric, sknf_numeric, index_vector, index_number);
}

std::string CanonicalForms::build_sdnf_term(
    const std::vector<std::string>& variables,
    const TruthRow& row
) {
    if (variables.empty()) {
        return "1";
    }
    
    std::vector<std::string> literals;
    for (size_t i = 0; i < variables.size(); i++) {
        if (row.assignment[i] == 1) {
            literals.push_back(variables[i]);
        } else {
            literals.push_back("!" + variables[i]);
        }
    }
    
    if (literals.size() == 1) {
        return literals[0];
    } else {
        std::ostringstream oss;
        oss << "(";
        for (size_t i = 0; i < literals.size(); i++) {
            if (i > 0) oss << "&";
            oss << literals[i];
        }
        oss << ")";
        return oss.str();
    }
}

std::string CanonicalForms::build_sknf_term(
    const std::vector<std::string>& variables,
    const TruthRow& row
) {
    if (variables.empty()) {
        return "0";
    }
    
    std::vector<std::string> literals;
    for (size_t i = 0; i < variables.size(); i++) {
        if (row.assignment[i] == 0) {
            literals.push_back(variables[i]);
        } else {
            literals.push_back("!" + variables[i]);
        }
    }
    
    if (literals.size() == 1) {
        return literals[0];
    } else {
        std::ostringstream oss;
        oss << "(";
        for (size_t i = 0; i < literals.size(); i++) {
            if (i > 0) oss << "|";
            oss << literals[i];
        }
        oss << ")";
        return oss.str();
    }
}
