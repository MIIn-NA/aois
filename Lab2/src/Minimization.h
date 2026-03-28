
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

struct TruthTableResult;

struct Implicant {
    std::string pattern;
    std::vector<int> indexes;
    Implicant(const std::string& pat, const std::vector<int>& idx) : pattern(pat), indexes(idx) {}
    int literals_count() const {
        int count = 0;
        for (char c : pattern) if (c != '-') count++;
        return count;
    }
};

struct GluingRecord {
    std::string left_pattern, right_pattern, result_pattern;
    GluingRecord(const std::string& l, const std::string& r, const std::string& res)
        : left_pattern(l), right_pattern(r), result_pattern(res) {}
};

struct GluingStage {
    std::vector<std::string> input_patterns, output_patterns;
    std::vector<GluingRecord> records;
    GluingStage(const std::vector<std::string>& in, const std::vector<GluingRecord>& rec, const std::vector<std::string>& out)
        : input_patterns(in), output_patterns(out), records(rec) {}
};

struct PrimeImplicantChart {
    std::vector<int> terms;
    std::vector<std::string> implicants;
    std::vector<std::vector<int>> matrix;
    std::string normal_form;
    PrimeImplicantChart(const std::vector<int>& t, const std::vector<std::string>& i,
        const std::vector<std::vector<int>>& m, const std::string& nf)
        : terms(t), implicants(i), matrix(m), normal_form(nf) {}
};

struct MinimizationResult {
    std::string method_name, normal_form, initial_expression, expression;
    std::vector<GluingStage> stages;
    std::vector<std::string> prime_implicants, selected_implicants, redundancy_checks;
    std::shared_ptr<PrimeImplicantChart> chart;
    
    MinimizationResult(const std::string& method, const std::string& nf, const std::string& init_expr,
        const std::vector<GluingStage>& st, const std::vector<std::string>& prime,
        const std::vector<std::string>& selected, const std::string& expr,
        const std::vector<std::string>& checks = {}, std::shared_ptr<PrimeImplicantChart> ch = nullptr)
        : method_name(method), normal_form(nf), initial_expression(init_expr), expression(expr),
          stages(st), prime_implicants(prime), selected_implicants(selected),
          redundancy_checks(checks), chart(ch) {}
    
    std::string form_label() const { return (normal_form == "sdnf") ? "СДНФ" : "СКНФ"; }
};

class Minimization {
public:
    MinimizationResult minimize_by_calculation(const TruthTableResult& table, const std::string& normal_form = "sdnf");
    MinimizationResult minimize_by_calculation_table(const TruthTableResult& table, const std::string& normal_form = "sdnf");
    bool implicant_covers_term(const std::string& pattern, int index, int dimension) const;
    
private:
    std::string normalize_form(const std::string& normal_form) const;
    std::pair<std::vector<int>, std::string> extract_terms(const TruthTableResult& table, const std::string& normal_form);
    std::pair<std::vector<GluingStage>, std::vector<Implicant>> generate_prime_implicants(int dimension, const std::vector<int>& terms);
    std::string combine_patterns(const std::string& left, const std::string& right) const;
    std::vector<Implicant> merge_duplicate_implicants(const std::vector<Implicant>& implicants) const;
    bool can_remove_pattern(const std::string& pattern, const std::vector<std::string>& other_patterns,
        const std::vector<int>& terms, int dimension) const;
    std::string pattern_to_term(const std::string& pattern, const std::vector<std::string>& variables, const std::string& normal_form) const;
    std::string patterns_to_expression(const std::vector<std::string>& patterns, const std::vector<std::string>& variables, const std::string& normal_form) const;
    PrimeImplicantChart build_chart(int dimension, const std::vector<std::string>& prime_patterns,
        const std::vector<int>& terms, const std::string& normal_form) const;
    std::vector<std::string> select_covering_implicants(const PrimeImplicantChart& chart, int dimension) const;
};
