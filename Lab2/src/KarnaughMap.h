
#pragma once

#include <string>
#include <vector>
#include <tuple>

struct TruthTableResult;
struct MinimizationResult;

struct KarnaughLayer {
    std::string title;
    std::vector<std::string> row_labels, col_labels;
    std::vector<std::vector<int>> values, indexes;
    KarnaughLayer(const std::string& t, const std::vector<std::string>& row_lbl, const std::vector<std::string>& col_lbl,
        const std::vector<std::vector<int>>& vals, const std::vector<std::vector<int>>& idx)
        : title(t), row_labels(row_lbl), col_labels(col_lbl), values(vals), indexes(idx) {}
};

struct KarnaughMapResult {
    std::string normal_form, expression, rendered_map;
    std::vector<std::string> selected_implicants;
    std::vector<KarnaughLayer> layers;
    std::vector<std::vector<int>> groups;
    
    // Исправлен порядок инициализации (теперь совпадает с объявлением полей)
    KarnaughMapResult(const std::string& nf, const std::string& expr, const std::vector<std::string>& implicants,
        const std::vector<KarnaughLayer>& lyr, const std::vector<std::vector<int>>& grp, const std::string& rendered)
        : normal_form(nf), expression(expr), rendered_map(rendered), selected_implicants(implicants), 
          layers(lyr), groups(grp) {}
};

class KarnaughMap {
public:
    KarnaughMapResult minimize(const TruthTableResult& table, const std::string& normal_form = "sdnf");
    std::vector<KarnaughLayer> build_layers(const TruthTableResult& table);
    std::string render(const std::vector<KarnaughLayer>& layers);
    
private:
    std::vector<std::vector<int>> groups_from_implicants(const TruthTableResult& table, const MinimizationResult& result);
    std::vector<int> gray_codes(int bits) const;
    std::string format_label(int value, int bits) const;
    std::tuple<int, int, int> split_dimension(int dimension) const;
    std::string build_title(const std::vector<std::string>& variables, int layer_value, int layer_bits) const;
    std::vector<int> join_bits(int layer_value, int row_value, int col_value,
        int layer_bits, int row_bits, int col_bits, int dimension) const;
};
