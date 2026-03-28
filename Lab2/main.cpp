
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "src/Analyzer.h"
#include "src/ReportBuilder.h"

class Main {
public:
    int run(int argc, char* argv[]) {
        std::string expression = readExpression(argc, argv);
        
        if (expression.empty()) {
            std::cout << "Пустая формула не поддерживается." << std::endl;
            return 1;
        }

        try {
            Analyzer analyzer;
            AnalysisResult result = analyzer.analyze(expression);
            
            ReportBuilder reportBuilder;
            std::cout << reportBuilder.build(result) << std::endl;
            
            return 0;
        } 
        catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << std::endl;
            return 1;
        }
    }

private:
    std::string readExpression(int argc, char* argv[]) {
        if (argc > 1) {
            std::stringstream ss;
            for (int i = 1; i < argc; i++) {
                ss << argv[i] << " ";
            }
            std::string expr = ss.str();
            size_t start = expr.find_first_not_of(" \t\n\r");
            size_t end = expr.find_last_not_of(" \t\n\r");
            if (start == std::string::npos) return "";
            return expr.substr(start, end - start + 1);
        }
        
        std::cout << "Введите логическую формулу: ";
        std::string expression;
        std::getline(std::cin, expression);
        
        size_t start = expression.find_first_not_of(" \t\n\r");
        size_t end = expression.find_last_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        return expression.substr(start, end - start + 1);
    }
};

int main(int argc, char* argv[]) {
    Main app;
    return app.run(argc, argv);
}
