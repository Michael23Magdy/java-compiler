#include "regex/RegexParser.h"
#include "regex/RegexUtils.h"

#include <stdexcept>
#include <vector>

namespace compiler {

std::string RegexParser::preprocess(const std::string& regex) {
    // 1. Expand character ranges.
    std::string expanded = RegexUtils::expandRanges(regex);

    // 2. Insert explicit concatenation operators.
    return RegexUtils::insertExplicitConcat(expanded);
}

std::shared_ptr<RegexNode> RegexParser::parse(const std::string& regex) {
    // 1. Preprocess the regular expression.
    std::string infix = preprocess(regex);

    // 2. Convert infix notation to postfix notation.
    std::string postfix = RegexUtils::infixToPostfix(infix);

    // 3. Construct the AST using a stack.
    std::vector<std::shared_ptr<RegexNode>> stack;

    for (size_t i = 0; i < postfix.size(); ++i) {
        char current = postfix[i];

        // Handle escaped characters and lambda.
        if (current == '\\') {
            if (i + 1 >= postfix.size()) {
                throw std::invalid_argument("Incomplete escape sequence");
            }

            char escaped = postfix[++i];

            if (escaped == 'L') {
                stack.push_back(std::make_shared<EpsilonNode>());
            } else {
                stack.push_back(std::make_shared<LiteralNode>(escaped));
            }

            continue;
        }

        // Handle binary operators.
        if (current == EXPLICIT_CONCAT || current == '|') {
            if (stack.size() < 2) {
                throw std::invalid_argument("Invalid regular expression: missing operands");
            }

            auto right = stack.back();
            stack.pop_back();

            auto left = stack.back();
            stack.pop_back();

            if (current == EXPLICIT_CONCAT) {
                stack.push_back(std::make_shared<ConcatNode>(left, right));
            } else {
                stack.push_back(std::make_shared<UnionNode>(left, right));
            }

            continue;
        }

        // Handle unary operators.
        if (current == '*' || current == '+') {
            if (stack.empty()) {
                throw std::invalid_argument("Invalid regular expression: missing operand");
            }

            auto child = stack.back();
            stack.pop_back();

            if (current == '*') {
                stack.push_back(std::make_shared<StarNode>(child));
            } else {
                stack.push_back(std::make_shared<PlusNode>(child));
            }

            continue;
        }

        // Handle ordinary characters.
        stack.push_back(std::make_shared<LiteralNode>(current));
    }

    // A valid expression must produce exactly one AST root.
    if (stack.size() != 1) {
        throw std::invalid_argument("Invalid regular expression");
    }

    return stack.back();
}

} // namespace compiler
