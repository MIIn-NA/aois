
#include "Minimization.h"
#include "TruthTable.h"
#include "CanonicalForms.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <set>

const std::set<std::string> SUPPORTED_FORMS = {"sdnf", "sknf"};

MinimizationResult Minimization::minimize_by_calculation(const TruthTableResult& table, const std::string& normal_form) {
    std::string normalized_form = normalize_form(normal_form);
    auto [terms, initial_expression] = extract_terms(table, normalized_form);
    
    if (terms.empty()) {
        return MinimizationResult("calculation", normalized_form, initial_expression, {}, {}, {},
            (normalized_form == "sdnf") ? "0" : "1");
    }
    
    auto [stages, prime_implicants] = generate_prime_implicants(table.dimension, terms);
    
    std::vector<std::string> selected;
    for (const auto& implicant : prime_implicants) {
        selected.push_back(implicant.pattern);
    }
    
    std::vector<std::string> checks;
    std::vector<std::string> selected_copy = selected;
    
    for (const auto& pattern : selected) {
        std::vector<std::string> other_patterns;
        for (const auto& p : selected_copy) {
            if (p != pattern) other_patterns.push_back(p);
        }
        
        bool removable = can_remove_pattern(pattern, other_patterns, terms, table.dimension);
        if (removable) {
            checks.push_back(pattern + ": удалена как лишняя");
            selected_copy.erase(std::remove(selected_copy.begin(), selected_copy.end(), pattern), selected_copy.end());
        } else {
            checks.push_back(pattern + ": оставлена");
        }
    }
    
    selected = selected_copy;
    std::sort(selected.begin(), selected.end(), [](const std::string& a, const std::string& b) {
        int ca = std::count_if(a.begin(), a.end(), [](char c) { return c != '-'; });
        int cb = std::count_if(b.begin(), b.end(), [](char c) { return c != '-'; });
        if (ca != cb) return ca < cb;
        return a < b;
    });
    
    std::vector<std::string> prime_patterns;
    for (const auto& implicant : prime_implicants) {
        prime_patterns.push_back(implicant.pattern);
    }
    
    return MinimizationResult("calculation", normalized_form, initial_expression, stages,
        prime_patterns, selected, patterns_to_expression(selected, table.variables, normalized_form), checks);
}

MinimizationResult Minimization::minimize_by_calculation_table(const TruthTableResult& table, const std::string& normal_form) {
    std::string normalized_form = normalize_form(normal_form);
    auto [terms, initial_expression] = extract_terms(table, normalized_form);
    
    if (terms.empty()) {
        auto chart = std::make_shared<PrimeImplicantChart>(std::vector<int>{}, std::vector<std::string>{},
            std::vector<std::vector<int>>{}, normalized_form);
        return MinimizationResult("calculation_table", normalized_form, initial_expression, {}, {}, {},
            (normalized_form == "sdnf") ? "0" : "1", {}, chart);
    }
    
    auto [stages, prime_implicants] = generate_prime_implicants(table.dimension, terms);
    
    std::vector<std::string> prime_patterns;
    for (const auto& implicant : prime_implicants) {
        prime_patterns.push_back(implicant.pattern);
    }
    
    std::sort(prime_patterns.begin(), prime_patterns.end(), [](const std::string& a, const std::string& b) {
        int ca = std::count_if(a.begin(), a.end(), [](char c) { return c != '-'; });
        int cb = std::count_if(b.begin(), b.end(), [](char c) { return c != '-'; });
        if (ca != cb) return ca < cb;
        return a < b;
    });
    
    PrimeImplicantChart chart = build_chart(table.dimension, prime_patterns, terms, normalized_form);
    std::vector<std::string> selected = select_covering_implicants(chart, table.dimension);
    
    return MinimizationResult("calculation_table", normalized_form, initial_expression, stages,
        prime_patterns, selected, patterns_to_expression(selected, table.variables, normalized_form),
        {}, std::make_shared<PrimeImplicantChart>(chart));
}

bool Minimization::implicant_covers_term(const std::string& pattern, int index, int dimension) const {
    std::string candidate;
    for (int i = dimension - 1; i >= 0; i--) {
        candidate += ((index >> i) & 1) ? '1' : '0';
    }
    for (size_t i = 0; i < pattern.length(); i++) {
        if (pattern[i] != '-' && pattern[i] != candidate[i]) return false;
    }
    return true;
}

std::string Minimization::normalize_form(const std::string& normal_form) const {
    std::string normalized = normal_form;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    if (SUPPORTED_FORMS.find(normalized) == SUPPORTED_FORMS.end()) {
        throw std::invalid_argument("Неподдерживаемая форма: " + normal_form);
    }
    return normalized;
}

std::pair<std::vector<int>, std::string> Minimization::extract_terms(const TruthTableResult& table, const std::string& normal_form) {
    CanonicalForms canonical;
    CanonicalFormsResult result = canonical.build(table);
    if (normal_form == "sdnf") {
        return {result.sdnf_numeric, result.sdnf};
    }
    return {result.sknf_numeric, result.sknf};
}

std::pair<std::vector<GluingStage>, std::vector<Implicant>> Minimization::generate_prime_implicants(int dimension, const std::vector<int>& terms) {
    std::set<int> unique_terms(terms.begin(), terms.end());
    std::vector<Implicant> current;
    
    for (int index : unique_terms) {
        std::string pattern;
        for (int i = dimension - 1; i >= 0; i--) {
            pattern += ((index >> i) & 1) ? '1' : '0';
        }
        current.emplace_back(pattern, std::vector<int>{index});
    }
    current = merge_duplicate_implicants(current);
    
    std::vector<GluingStage> stages;
    std::map<std::string, std::set<int>> prime_map;
    
    while (!current.empty()) {
        std::map<int, std::vector<Implicant>> groups;
        for (const auto& implicant : current) {
            int ones_count = std::count(implicant.pattern.begin(), implicant.pattern.end(), '1');
            groups[ones_count].push_back(implicant);
        }
        
        std::set<std::string> used_patterns;
        std::vector<Implicant> next_implicants;
        std::vector<GluingRecord> records;
        
        for (const auto& group_pair : groups) {
            int group_index = group_pair.first;
            const auto& left_group = group_pair.second;
            auto it = groups.find(group_index + 1);
            if (it == groups.end()) continue;
            const auto& right_group = it->second;
            
            for (const auto& left_implicant : left_group) {
                for (const auto& right_implicant : right_group) {
                    std::string combined = combine_patterns(left_implicant.pattern, right_implicant.pattern);
                    if (combined.empty()) continue;
                    
                    used_patterns.insert(left_implicant.pattern);
                    used_patterns.insert(right_implicant.pattern);
                    
                    std::set<int> merged_indexes(left_implicant.indexes.begin(), left_implicant.indexes.end());
                    merged_indexes.insert(right_implicant.indexes.begin(), right_implicant.indexes.end());
                    
                    next_implicants.emplace_back(combined, std::vector<int>(merged_indexes.begin(), merged_indexes.end()));
                    
                    std::string left_pat = std::min(left_implicant.pattern, right_implicant.pattern);
                    std::string right_pat = std::max(left_implicant.pattern, right_implicant.pattern);
                    records.emplace_back(left_pat, right_pat, combined);
                }
            }
        }
        
        for (const auto& implicant : current) {
            if (used_patterns.find(implicant.pattern) == used_patterns.end()) {
                prime_map[implicant.pattern].insert(implicant.indexes.begin(), implicant.indexes.end());
            }
        }
        
        if (next_implicants.empty()) break;
        
        std::vector<Implicant> next_merged = merge_duplicate_implicants(next_implicants);
        std::sort(records.begin(), records.end(), [](const GluingRecord& a, const GluingRecord& b) {
            return a.left_pattern < b.left_pattern;
        });
        
        std::vector<std::string> input_patterns, output_patterns;
        for (const auto& imp : current) input_patterns.push_back(imp.pattern);
        for (const auto& imp : next_merged) output_patterns.push_back(imp.pattern);
        
        stages.emplace_back(input_patterns, records, output_patterns);
        current = next_merged;
    }
    
    std::vector<Implicant> prime_result;
    for (const auto& pair : prime_map) {
        prime_result.emplace_back(pair.first, std::vector<int>(pair.second.begin(), pair.second.end()));
    }
    prime_result = merge_duplicate_implicants(prime_result);
    
    return {stages, prime_result};
}

std::string Minimization::combine_patterns(const std::string& left, const std::string& right) const {
    int differences = 0;
    std::string result;
    for (size_t i = 0; i < left.length(); i++) {
        if (left[i] == right[i]) {
            result += left[i];
        } else if (left[i] == '-' || right[i] == '-') {
            return "";
        } else {
            differences++;
            result += '-';
            if (differences > 1) return "";
        }
    }
    return (differences == 1) ? result : "";
}

std::vector<Implicant> Minimization::merge_duplicate_implicants(const std::vector<Implicant>& implicants) const {
    std::map<std::string, std::set<int>> merged;
    for (const auto& implicant : implicants) {
        merged[implicant.pattern].insert(implicant.indexes.begin(), implicant.indexes.end());
    }
    std::vector<Implicant> result;
    for (const auto& pair : merged) {
        result.emplace_back(pair.first, std::vector<int>(pair.second.begin(), pair.second.end()));
    }
    std::sort(result.begin(), result.end(), [](const Implicant& a, const Implicant& b) {
        if (a.literals_count() != b.literals_count()) return a.literals_count() < b.literals_count();
        return a.pattern < b.pattern;
    });
    return result;
}

bool Minimization::can_remove_pattern(const std::string& pattern, const std::vector<std::string>& other_patterns,
    const std::vector<int>& terms, int dimension) const {
    for (int term : terms) {
        if (implicant_covers_term(pattern, term, dimension)) {
            bool covered_by_others = false;
            for (const auto& other : other_patterns) {
                if (implicant_covers_term(other, term, dimension)) {
                    covered_by_others = true;
                    break;
                }
            }
            if (!covered_by_others) return false;
        }
    }
    return true;
}

std::string Minimization::pattern_to_term(const std::string& pattern, const std::vector<std::string>& variables, const std::string& normal_form) const {
    std::vector<std::string> literals;
    for (size_t i = 0; i < pattern.length() && i < variables.size(); i++) {
        if (normal_form == "sdnf") {
            if (pattern[i] == '1') literals.push_back(variables[i]);
            else if (pattern[i] == '0') literals.push_back("!" + variables[i]);
        } else {
            if (pattern[i] == '0') literals.push_back(variables[i]);
            else if (pattern[i] == '1') literals.push_back("!" + variables[i]);
        }
    }
    if (literals.empty()) return (normal_form == "sdnf") ? "1" : "0";
    
    std::string joiner = (normal_form == "sdnf") ? "&" : "|";
    if (literals.size() == 1) return literals[0];
    
    std::ostringstream oss;
    oss << "(";
    for (size_t i = 0; i < literals.size(); i++) {
        if (i > 0) oss << joiner;
        oss << literals[i];
    }
    oss << ")";
    return oss.str();
}

std::string Minimization::patterns_to_expression(const std::vector<std::string>& patterns, const std::vector<std::string>& variables, const std::string& normal_form) const {
    if (patterns.empty()) return (normal_form == "sdnf") ? "0" : "1";
    
    std::vector<std::string> terms;
    for (const auto& pattern : patterns) {
        terms.push_back(pattern_to_term(pattern, variables, normal_form));
    }
    
    std::string joiner = (normal_form == "sdnf") ? "|" : "&";
    std::ostringstream oss;
    for (size_t i = 0; i < terms.size(); i++) {
        if (i > 0) oss << joiner;
        oss << terms[i];
    }
    return oss.str();
}

PrimeImplicantChart Minimization::build_chart(int dimension, const std::vector<std::string>& prime_patterns,
    const std::vector<int>& terms, const std::string& normal_form) const {
    std::vector<std::vector<int>> matrix;
    for (const auto& pattern : prime_patterns) {
        std::vector<int> row;
        for (int term : terms) {
            row.push_back(implicant_covers_term(pattern, term, dimension) ? 1 : 0);
        }
        matrix.push_back(row);
    }
    return PrimeImplicantChart(terms, prime_patterns, matrix, normal_form);
}

std::vector<std::string> Minimization::select_covering_implicants(const PrimeImplicantChart& chart, int dimension) const {
    std::set<std::string> selected;
    
    for (size_t col = 0; col < chart.terms.size(); col++) {
        std::vector<std::string> covering;
        for (size_t row = 0; row < chart.matrix.size(); row++) {
            if (chart.matrix[row][col] == 1) {
                covering.push_back(chart.implicants[row]);
            }
        }
        if (covering.size() == 1) {
            selected.insert(covering[0]);
        }
    }
    
    std::set<int> uncovered(chart.terms.begin(), chart.terms.end());
    for (const auto& pattern : selected) {
        for (int term : chart.terms) {
            if (implicant_covers_term(pattern, term, dimension)) {
                uncovered.erase(term);
            }
        }
    }
    
    while (!uncovered.empty()) {
        std::vector<std::string> candidates;
        for (const auto& pattern : chart.implicants) {
            if (selected.find(pattern) == selected.end()) {
                candidates.push_back(pattern);
            }
        }
        if (candidates.empty()) break;
        
        std::vector<std::tuple<int, int, std::string, std::set<int>>> scores;
        for (const auto& pattern : candidates) {
            std::set<int> covered_terms;
            for (int term : uncovered) {
                if (implicant_covers_term(pattern, term, dimension)) {
                    covered_terms.insert(term);
                }
            }
            int literal_count = std::count_if(pattern.begin(), pattern.end(), [](char c) { return c != '-'; });
            scores.emplace_back(covered_terms.size(), -literal_count, pattern, covered_terms);
        }
        
        std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) { return a > b; });
        
        auto& [cover_count, _, best_pattern, covered_terms] = scores[0];
        if (cover_count == 0) break;
        
        selected.insert(best_pattern);
        for (int term : covered_terms) {
            uncovered.erase(term);
        }
    }
    
    std::vector<std::string> result(selected.begin(), selected.end());
    std::sort(result.begin(), result.end(), [](const std::string& a, const std::string& b) {
        int ca = std::count_if(a.begin(), a.end(), [](char c) { return c != '-'; });
        int cb = std::count_if(b.begin(), b.end(), [](char c) { return c != '-'; });
        if (ca != cb) return ca < cb;
        return a < b;
    });
    return result;
}
