
#include "ReportBuilder.h"
#include <sstream>
#include <iomanip>

std::string ReportBuilder::build(const AnalysisResult& result) {
    std::vector<std::string> lines;
    
    lines.push_back("Формула: " + result.parsed_expression.source);
    lines.push_back("");
    lines.push_back("Таблица истинности:");
    lines.push_back(format_truth_table(result.truth_table));
    lines.push_back("");
    lines.push_back("СДНФ и СКНФ:");
    lines.push_back("СДНФ: " + result.canonical_forms.sdnf);
    lines.push_back("СКНФ: " + result.canonical_forms.sknf);
    
    std::ostringstream oss;
    oss << "Числовая форма СДНФ: (";
    for (size_t i = 0; i < result.canonical_forms.sdnf_numeric.size(); i++) {
        if (i > 0) oss << ", ";
        oss << result.canonical_forms.sdnf_numeric[i];
    }
    oss << ")";
    lines.push_back(oss.str());
    
    oss.str(""); oss.clear();
    oss << "Числовая форма СКНФ: (";
    for (size_t i = 0; i < result.canonical_forms.sknf_numeric.size(); i++) {
        if (i > 0) oss << ", ";
        oss << result.canonical_forms.sknf_numeric[i];
    }
    oss << ")";
    lines.push_back(oss.str());
    
    lines.push_back("");
    lines.push_back("Индексная форма:");
    lines.push_back("Вектор: " + result.canonical_forms.index_vector);
    lines.push_back("Индекс: " + std::to_string(result.canonical_forms.index_number));
    lines.push_back("");
    lines.push_back("Классы Поста:");
    lines.push_back(format_post_classes(result));
    lines.push_back("");
    lines.push_back("Полином Жегалкина:");
    lines.push_back(result.zhegalkin_polynomial.expression);
    lines.push_back("");
    lines.push_back("Фиктивные переменные:");
    lines.push_back(result.fictitious_variables.empty() ? "нет" : format_implicants(result.fictitious_variables));
    lines.push_back("");
    lines.push_back("Булевы производные:");
    lines.push_back(format_derivatives(result));
    lines.push_back("");
    
    append_calculation_section(lines, "Минимизация СДНФ (расчетный метод):", result.sdnf_calculation_minimization);
    append_calculation_section(lines, "Минимизация СКНФ (расчетный метод):", result.sknf_calculation_minimization);
    append_calculation_table_section(lines, "Минимизация СДНФ (расчетно-табличный метод):", result.sdnf_calculation_table_minimization);
    append_calculation_table_section(lines, "Минимизация СКНФ (расчетно-табличный метод):", result.sknf_calculation_table_minimization);
    append_karnaugh_section(lines, "Минимизация СДНФ (карта Карно):", result.sdnf_karnaugh_minimization);
    append_karnaugh_section(lines, "Минимизация СКНФ (карта Карно):", result.sknf_karnaugh_minimization);
    
    return join_lines(lines);
}

std::string ReportBuilder::format_truth_table(const TruthTableResult& table) const {
    std::vector<std::string> lines;
    std::string header;
    if (table.variables.empty()) {
        header = "f";
    } else {
        for (size_t i = 0; i < table.variables.size(); i++) {
            if (i > 0) header += " ";
            header += table.variables[i];
        }
        header += " | f";
    }
    lines.push_back(header);
    lines.push_back(std::string(header.length(), '-'));
    
    for (const auto& row : table.rows) {
        std::string assignment;
        for (size_t i = 0; i < row.assignment.size(); i++) {
            if (i > 0) assignment += " ";
            assignment += std::to_string(row.assignment[i]);
        }
        if (assignment.empty()) {
            lines.push_back(std::to_string(row.result));
        } else {
            lines.push_back(assignment + " | " + std::to_string(row.result));
        }
    }
    return join_lines(lines);
}

std::string ReportBuilder::format_post_classes(const AnalysisResult& result) const {
    const auto& c = result.post_classes;
    std::vector<std::string> lines;
    lines.push_back("T0: " + std::string(c.t0 ? "да" : "нет"));
    lines.push_back("T1: " + std::string(c.t1 ? "да" : "нет"));
    lines.push_back("S: " + std::string(c.s ? "да" : "нет"));
    lines.push_back("M: " + std::string(c.m ? "да" : "нет"));
    lines.push_back("L: " + std::string(c.l ? "да" : "нет"));
    return join_lines(lines);
}

std::string ReportBuilder::format_derivatives(const AnalysisResult& result) const {
    if (result.derivatives.empty()) return "нет переменных для дифференциации";
    
    std::vector<std::string> lines;
    for (const auto& derivative : result.derivatives) {
        std::string suffix;
        for (const auto& var : derivative.by_variables) suffix += var;
        std::ostringstream oss;
        oss << "D_" << suffix << ": " << derivative.sdnf << " | индекс=" << derivative.index_number << " (" << derivative.index_vector << ")";
        lines.push_back(oss.str());
    }
    return join_lines(lines);
}

std::string ReportBuilder::format_gluing_stages(const MinimizationResult& result) const {
    if (result.stages.empty()) return "склеивание не выполнялось";
    
    std::vector<std::string> lines;
    for (size_t i = 0; i < result.stages.size(); i++) {
        const auto& stage = result.stages[i];
        lines.push_back("Этап " + std::to_string(i + 1) + ":");
        if (stage.records.empty()) {
            lines.push_back("  нет склеиваемых пар");
        } else {
            for (const auto& record : stage.records) {
                lines.push_back("  " + record.left_pattern + " + " + record.right_pattern + " -> " + record.result_pattern);
            }
        }
        std::string output;
        for (size_t j = 0; j < stage.output_patterns.size(); j++) {
            if (j > 0) output += ", ";
            output += stage.output_patterns[j];
        }
        lines.push_back("  Результат: " + output);
    }
    return join_lines(lines);
}

std::string ReportBuilder::format_redundancy_checks(const MinimizationResult& result) const {
    if (result.redundancy_checks.empty()) return "нет проверок";
    return join_lines(result.redundancy_checks);
}

std::string ReportBuilder::format_chart(const std::shared_ptr<PrimeImplicantChart>& chart) const {
    if (!chart) return "таблица покрытия не построена";
    if (chart->terms.empty()) return "нет строк для покрытия";
    
    std::vector<std::string> lines;
    std::ostringstream header_oss;
    header_oss << "pat\\idx ";
    for (int term : chart->terms) header_oss << std::setw(3) << term;
    std::string header = header_oss.str();
    lines.push_back(header);
    lines.push_back(std::string(header.length(), '-'));
    
    for (size_t i = 0; i < chart->implicants.size(); i++) {
        std::ostringstream row_oss;
        row_oss << std::setw(5) << chart->implicants[i] << " ";
        for (int cell : chart->matrix[i]) row_oss << std::setw(3) << cell;
        lines.push_back(row_oss.str());
    }
    return join_lines(lines);
}

std::string ReportBuilder::format_implicants(const std::vector<std::string>& patterns) const {
    if (patterns.empty()) return "нет";
    std::string result;
    for (size_t i = 0; i < patterns.size(); i++) {
        if (i > 0) result += ", ";
        result += patterns[i];
    }
    return result;
}

void ReportBuilder::append_calculation_section(std::vector<std::string>& lines, const std::string& title, const MinimizationResult& result) {
    lines.push_back(title);
    lines.push_back("Исходная " + result.form_label() + ": " + result.initial_expression);
    lines.push_back(format_gluing_stages(result));
    lines.push_back("Простые импликанты: " + format_implicants(result.prime_implicants));
    lines.push_back("Проверка лишних термов:");
    lines.push_back(format_redundancy_checks(result));
    lines.push_back("Результат: " + result.expression);
    lines.push_back("");
}

void ReportBuilder::append_calculation_table_section(std::vector<std::string>& lines, const std::string& title, const MinimizationResult& result) {
    lines.push_back(title);
    lines.push_back("Исходная " + result.form_label() + ": " + result.initial_expression);
    lines.push_back(format_gluing_stages(result));
    lines.push_back("Таблица покрытия:");
    lines.push_back(format_chart(result.chart));
    lines.push_back("Выбранные импликанты: " + format_implicants(result.selected_implicants));
    lines.push_back("Результат: " + result.expression);
    lines.push_back("");
}

void ReportBuilder::append_karnaugh_section(std::vector<std::string>& lines, const std::string& title, const KarnaughMapResult& result) {
    lines.push_back(title);
    lines.push_back(result.rendered_map);
    
    std::ostringstream groups_oss;
    groups_oss << "Группы: (";
    for (size_t i = 0; i < result.groups.size(); i++) {
        if (i > 0) groups_oss << "), (";
        for (size_t j = 0; j < result.groups[i].size(); j++) {
            if (j > 0) groups_oss << ", ";
            groups_oss << result.groups[i][j];
        }
    }
    groups_oss << ")";
    lines.push_back(groups_oss.str());
    
    lines.push_back("Результат: " + result.expression);
    lines.push_back("");
}

std::string ReportBuilder::join_lines(const std::vector<std::string>& lines) const {
    std::ostringstream oss;
    for (size_t i = 0; i < lines.size(); i++) {
        if (i > 0) oss << "\n";
        oss << lines[i];
    }
    return oss.str();
}
