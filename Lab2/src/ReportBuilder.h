
#pragma once

#include <string>
#include <vector>

#include "Analyzer.h"

class ReportBuilder {
public:
    std::string build(const AnalysisResult& result);
    
private:
    std::string format_truth_table(const TruthTableResult& table) const;
    std::string format_post_classes(const AnalysisResult& result) const;
    std::string format_derivatives(const AnalysisResult& result) const;
    std::string format_gluing_stages(const MinimizationResult& result) const;
    std::string format_redundancy_checks(const MinimizationResult& result) const;
    std::string format_chart(const std::shared_ptr<PrimeImplicantChart>& chart) const;
    std::string format_implicants(const std::vector<std::string>& patterns) const;
    void append_calculation_section(std::vector<std::string>& lines, const std::string& title, const MinimizationResult& result);
    void append_calculation_table_section(std::vector<std::string>& lines, const std::string& title, const MinimizationResult& result);
    void append_karnaugh_section(std::vector<std::string>& lines, const std::string& title, const KarnaughMapResult& result);
    std::string join_lines(const std::vector<std::string>& lines) const;
};
