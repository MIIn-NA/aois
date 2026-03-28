
#pragma once

#include <string>
#include <vector>
#include <map>

struct ParsedExpression;

struct TruthRow {
    std::vector<int> assignment;
    int result;
    int index;
    
    TruthRow(const std::vector<int>& assign, int res, int idx)
        : assignment(assign), result(res), index(idx) {}
};

struct TruthTableResult {
    std::vector<std::string> variables;
    std::vector<TruthRow> rows;
    std::vector<int> vector;
    int dimension;
    
    TruthTableResult(
        const std::vector<std::string>& vars,
        const std::vector<TruthRow>& r
    ) : variables(vars), rows(r), dimension(static_cast<int>(vars.size())) {
        vector.reserve(rows.size());
        for (const auto& row : rows) {
            vector.push_back(row.result);
        }
    }
};

class TruthTable {
public:
    TruthTableResult build(const ParsedExpression& parsed_expression);
    TruthTableResult from_vector(
        const std::vector<std::string>& variables,
        const std::vector<int>& vec
    );
    int assignment_to_index(const std::vector<int>& assignment) const;
    std::vector<int> index_to_assignment(int index, int dimension) const;
};
