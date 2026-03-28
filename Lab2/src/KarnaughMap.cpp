
#include "KarnaughMap.h"
#include "TruthTable.h"
#include "Minimization.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

KarnaughMapResult KarnaughMap::minimize(const TruthTableResult& table, const std::string& normal_form) {
    Minimization minimization;
    MinimizationResult result = minimization.minimize_by_calculation_table(table, normal_form);
    
    std::vector<KarnaughLayer> layers = build_layers(table);
    std::string rendered_map = render(layers);
    std::vector<std::vector<int>> groups = groups_from_implicants(table, result);
    
    return KarnaughMapResult(result.normal_form, result.expression, result.selected_implicants,
        layers, groups, rendered_map);
}

std::vector<KarnaughLayer> KarnaughMap::build_layers(const TruthTableResult& table) {
    if (table.dimension == 0) {
        return {KarnaughLayer("const", {"-"}, {"-"}, {{table.vector[0]}}, {{0}})};
    }
    
    auto [layer_bits, row_bits, col_bits] = split_dimension(table.dimension);
    std::vector<int> layer_codes = gray_codes(layer_bits);
    std::vector<int> row_codes = gray_codes(row_bits);
    std::vector<int> col_codes = gray_codes(col_bits);
    
    std::vector<KarnaughLayer> layers;
    
    for (int layer_value : layer_codes) {
        std::string title = build_title(table.variables, layer_value, layer_bits);
        std::vector<std::vector<int>> value_rows, index_rows;
        
        for (int row_value : row_codes) {
            std::vector<int> row_values, row_indexes;
            for (int col_value : col_codes) {
                std::vector<int> assignment = join_bits(layer_value, row_value, col_value,
                    layer_bits, row_bits, col_bits, table.dimension);
                TruthTable tt;
                int index = tt.assignment_to_index(assignment);
                row_values.push_back(table.vector[index]);
                row_indexes.push_back(index);
            }
            value_rows.push_back(row_values);
            index_rows.push_back(row_indexes);
        }
        
        std::vector<std::string> row_labels, col_labels;
        for (int value : row_codes) row_labels.push_back(format_label(value, row_bits));
        for (int value : col_codes) col_labels.push_back(format_label(value, col_bits));
        
        layers.emplace_back(title, row_labels, col_labels, value_rows, index_rows);
    }
    return layers;
}

std::string KarnaughMap::render(const std::vector<KarnaughLayer>& layers) {
    std::ostringstream oss;
    for (size_t i = 0; i < layers.size(); i++) {
        const auto& layer = layers[i];
        oss << "[" << layer.title << "]\n ";
        for (const auto& label : layer.col_labels) {
            oss << std::setw(4) << label;
        }
        oss << "\n";
        for (size_t row = 0; row < layer.row_labels.size(); row++) {
            oss << std::setw(3) << layer.row_labels[row] << " ";
            for (int value : layer.values[row]) {
                oss << std::setw(4) << value;
            }
            oss << "\n";
        }
        if (i < layers.size() - 1) oss << "\n";
    }
    std::string result = oss.str();
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

std::vector<std::vector<int>> KarnaughMap::groups_from_implicants(const TruthTableResult& table, const MinimizationResult& result) {
    int target_value = (result.normal_form == "sdnf") ? 1 : 0;
    std::vector<std::vector<int>> groups;
    Minimization minimization;
    
    for (const auto& implicant : result.selected_implicants) {
        std::vector<int> covered;
        int table_size = 1 << table.dimension;
        for (int index = 0; index < table_size; index++) {
            if (minimization.implicant_covers_term(implicant, index, table.dimension) &&
                table.vector[index] == target_value) {
                covered.push_back(index);
            }
        }
        groups.push_back(covered);
    }
    return groups;
}

std::vector<int> KarnaughMap::gray_codes(int bits) const {
    if (bits == 0) return {0};
    std::vector<int> codes;
    int size = 1 << bits;
    codes.reserve(size);
    for (int index = 0; index < size; index++) {
        codes.push_back(index ^ (index >> 1));
    }
    return codes;
}

std::string KarnaughMap::format_label(int value, int bits) const {
    if (bits == 0) return "-";
    std::ostringstream oss;
    for (int i = bits - 1; i >= 0; i--) {
        oss << ((value >> i) & 1);
    }
    return oss.str();
}

std::tuple<int, int, int> KarnaughMap::split_dimension(int dimension) const {
    if (dimension < 5) return std::make_tuple(0, std::max(0, dimension - 2), std::min(2, dimension));
    return std::make_tuple(1, 2, 2);
}

std::string KarnaughMap::build_title(const std::vector<std::string>& variables, int layer_value, int layer_bits) const {
    if (layer_bits == 0) return "map";
    std::ostringstream oss;
    std::string layer_text = format_label(layer_value, layer_bits);
    for (int i = 0; i < layer_bits && i < static_cast<int>(variables.size()); i++) {
        if (i > 0) oss << ", ";
        oss << variables[i] << "=" << layer_text[i];
    }
    return oss.str();
}

std::vector<int> KarnaughMap::join_bits(int layer_value, int row_value, int col_value,
    int layer_bits, int row_bits, int col_bits, int dimension) const {
    std::string layer_text = (layer_bits > 0) ? format_label(layer_value, layer_bits) : "";
    std::string row_text = (row_bits > 0) ? format_label(row_value, row_bits) : "";
    std::string col_text = (col_bits > 0) ? format_label(col_value, col_bits) : "";
    
    std::string full_text = layer_text + row_text + col_text;
    while (static_cast<int>(full_text.length()) < dimension) full_text += "0";
    
    std::vector<int> assignment;
    assignment.reserve(dimension);
    for (char c : full_text) {
        assignment.push_back(c - '0');
    }
    return assignment;
}
