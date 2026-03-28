
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <stdexcept>

const int MAX_VARIABLES = 5;

struct Token {
    std::string token_type;
    std::string value;
    int position;
    
    Token(const std::string& type, const std::string& val, int pos)
        : token_type(type), value(val), position(pos) {}
};

class Node {
public:
    virtual ~Node() = default;
    virtual int evaluate(const std::map<std::string, int>& values) const = 0;
    virtual std::set<std::string> collect_variables() const = 0;
};

class ConstantNode : public Node {
private:
    int value;
public:
    explicit ConstantNode(int val) : value(val) {}
    
    int evaluate(const std::map<std::string, int>& values) const override {
        (void)values;
        return value ? 1 : 0;
    }
    
    std::set<std::string> collect_variables() const override {
        return {};
    }
};

class VariableNode : public Node {
private:
    std::string name;
public:
    explicit VariableNode(const std::string& var_name) : name(var_name) {}
    
    int evaluate(const std::map<std::string, int>& values) const override {
        auto it = values.find(name);
        if (it == values.end()) {
            throw std::invalid_argument(
                "Для переменной '" + name + "' не задано значение."
            );
        }
        return it->second ? 1 : 0;
    }
    
    std::set<std::string> collect_variables() const override {
        return {name};
    }
};

class UnaryNode : public Node {
private:
    std::string op;
    std::shared_ptr<Node> operand;
public:
    UnaryNode(const std::string& oper, std::shared_ptr<Node> opnd)
        : op(oper), operand(opnd) {}
    
    int evaluate(const std::map<std::string, int>& values) const override {
        int operand_value = operand->evaluate(values);
        if (op == "!") {
            return 1 - operand_value;
        }
        throw std::invalid_argument("Неизвестный унарный оператор: " + op);
    }
    
    std::set<std::string> collect_variables() const override {
        return operand->collect_variables();
    }
};

class BinaryNode : public Node {
private:
    std::string op;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
public:
    BinaryNode(const std::string& oper,
               std::shared_ptr<Node> l,
               std::shared_ptr<Node> r)
        : op(oper), left(l), right(r) {}
    
    int evaluate(const std::map<std::string, int>& values) const override {
        int left_value = left->evaluate(values);
        int right_value = right->evaluate(values);
        
        if (op == "&") {
            return left_value & right_value;
        }
        if (op == "|") {
            return left_value | right_value;
        }
        if (op == "->") {
            return (!left_value || right_value) ? 1 : 0;
        }
        if (op == "~") {
            return (left_value == right_value) ? 1 : 0;
        }
        
        throw std::invalid_argument("Неизвестный бинарный оператор: " + op);
    }
    
    std::set<std::string> collect_variables() const override {
        std::set<std::string> vars = left->collect_variables();
        auto right_vars = right->collect_variables();
        vars.insert(right_vars.begin(), right_vars.end());
        return vars;
    }
};

struct ParsedExpression {
    std::string source;
    std::shared_ptr<Node> root;
    std::vector<std::string> variables;
    
    ParsedExpression(
        const std::string& src,
        std::shared_ptr<Node> r,
        const std::vector<std::string>& vars
    ) : source(src), root(r), variables(vars) {}
};

class ExpressionParser {
public:
    ParsedExpression parse(const std::string& source);
    
private:
    void validate_source(const std::string& source) const;
    std::string normalize(const std::string& source) const;
    std::vector<Token> tokenize(const std::string& source) const;
};

class _Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    
public:
    explicit _Parser(const std::vector<Token>& toks);
    
    std::shared_ptr<Node> parse();
    
private:
    const Token& current() const;
    std::shared_ptr<Node> parse_equivalence();
    std::shared_ptr<Node> parse_implication();
    std::shared_ptr<Node> parse_or();
    std::shared_ptr<Node> parse_and();
    std::shared_ptr<Node> parse_unary();
    std::shared_ptr<Node> parse_primary();
    bool match(const std::string& token_type);
    Token consume(const std::string& token_type);
};
