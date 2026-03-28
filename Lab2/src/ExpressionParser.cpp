
#include "ExpressionParser.h"
#include <algorithm>
#include <cctype>

const std::map<std::string, std::string> SYMBOL_REPLACEMENTS = {
    {"¬", "!"}, {"∧", "&"}, {"∨", "|"},
    {"→", "->"}, {"⇒", "->"}, {"↔", "~"}, {"≡", "~"}
};

ParsedExpression ExpressionParser::parse(const std::string& source) {
    std::string normalized_source = normalize(source);
    validate_source(normalized_source);
    
    std::vector<Token> tokens = tokenize(normalized_source);
    _Parser parser(tokens);
    std::shared_ptr<Node> root = parser.parse();
    
    std::set<std::string> var_set = root->collect_variables();
    std::vector<std::string> variables(var_set.begin(), var_set.end());
    std::sort(variables.begin(), variables.end());
    
    if (variables.size() > MAX_VARIABLES) {
        throw std::invalid_argument(
            "Поддерживается не более " + std::to_string(MAX_VARIABLES) + " переменных."
        );
    }
    
    return ParsedExpression(normalized_source, root, variables);
}

void ExpressionParser::validate_source(const std::string& source) const {
    if (source.empty() || source.find_first_not_of(" \t\n\r") == std::string::npos) {
        throw std::invalid_argument("Формула не должна быть пустой.");
    }
}

std::string ExpressionParser::normalize(const std::string& source) const {
    std::string result = source;
    for (const auto& pair : SYMBOL_REPLACEMENTS) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    return result;
}

std::vector<Token> ExpressionParser::tokenize(const std::string& source) const {
    std::vector<Token> tokens;
    size_t index = 0;
    
    while (index < source.length()) {
        char current = source[index];
        
        if (std::isspace(current)) {
            index++;
            continue;
        }
        
        if (source.substr(index, 2) == "->") {
            tokens.emplace_back("IMP", "->", static_cast<int>(index));
            index += 2;
            continue;
        }
        
        if (current == '!') {
            tokens.emplace_back("NOT", "!", static_cast<int>(index));
            index++;
            continue;
        }
        if (current == '&') {
            tokens.emplace_back("AND", "&", static_cast<int>(index));
            index++;
            continue;
        }
        if (current == '|') {
            tokens.emplace_back("OR", "|", static_cast<int>(index));
            index++;
            continue;
        }
        if (current == '~') {
            tokens.emplace_back("EQ", "~", static_cast<int>(index));
            index++;
            continue;
        }
        if (current == '(') {
            tokens.emplace_back("LPAREN", "(", static_cast<int>(index));
            index++;
            continue;
        }
        if (current == ')') {
            tokens.emplace_back("RPAREN", ")", static_cast<int>(index));
            index++;
            continue;
        }
        if (current == '0' || current == '1') {
            tokens.emplace_back("CONST", std::string(1, current), static_cast<int>(index));
            index++;
            continue;
        }
        if (std::isalpha(current) || current == '_') {
            size_t start = index;
            index++;
            while (index < source.length()) {
                char next = source[index];
                if (std::isalnum(next) || next == '_') {
                    index++;
                } else {
                    break;
                }
            }
            std::string var_name = source.substr(start, index - start);
            tokens.emplace_back("VAR", var_name, static_cast<int>(start));
            continue;
        }
        
        throw std::invalid_argument(
            "Недопустимый символ '" + std::string(1, current) + 
            "' в позиции " + std::to_string(index) + "."
        );
    }
    
    tokens.emplace_back("END", "", static_cast<int>(source.length()));
    return tokens;
}

_Parser::_Parser(const std::vector<Token>& toks) : tokens(toks), position(0) {}

const Token& _Parser::current() const {
    return tokens[position];
}

std::shared_ptr<Node> _Parser::parse() {
    std::shared_ptr<Node> result = parse_equivalence();
    consume("END");
    return result;
}

std::shared_ptr<Node> _Parser::parse_equivalence() {
    std::shared_ptr<Node> node = parse_implication();
    while (match("EQ")) {
        node = std::make_shared<BinaryNode>("~", node, parse_implication());
    }
    return node;
}

std::shared_ptr<Node> _Parser::parse_implication() {
    std::shared_ptr<Node> node = parse_or();
    if (match("IMP")) {
        return std::make_shared<BinaryNode>("->", node, parse_implication());
    }
    return node;
}

std::shared_ptr<Node> _Parser::parse_or() {
    std::shared_ptr<Node> node = parse_and();
    while (match("OR")) {
        node = std::make_shared<BinaryNode>("|", node, parse_and());
    }
    return node;
}

std::shared_ptr<Node> _Parser::parse_and() {
    std::shared_ptr<Node> node = parse_unary();
    while (match("AND")) {
        node = std::make_shared<BinaryNode>("&", node, parse_unary());
    }
    return node;
}

std::shared_ptr<Node> _Parser::parse_unary() {
    if (match("NOT")) {
        return std::make_shared<UnaryNode>("!", parse_unary());
    }
    return parse_primary();
}

std::shared_ptr<Node> _Parser::parse_primary() {
    const Token& token = current();
    
    if (match("VAR")) {
        return std::make_shared<VariableNode>(token.value);
    }
    if (match("CONST")) {
        return std::make_shared<ConstantNode>(std::stoi(token.value));
    }
    if (match("LPAREN")) {
        std::shared_ptr<Node> inner = parse_equivalence();
        consume("RPAREN");
        return inner;
    }
    
    throw std::invalid_argument(
        "Ожидалась переменная, константа или '(' в позиции " + 
        std::to_string(token.position) + "."
    );
}

bool _Parser::match(const std::string& token_type) {
    if (current().token_type == token_type) {
        position++;
        return true;
    }
    return false;
}

Token _Parser::consume(const std::string& token_type) {
    const Token& token = current();
    if (token.token_type != token_type) {
        throw std::invalid_argument(
            "Ожидался токен " + token_type + " в позиции " + 
            std::to_string(token.position) + ", но получен " + token.token_type + "."
        );
    }
    position++;
    return token;
}
