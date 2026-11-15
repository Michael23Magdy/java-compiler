#include "regex/RegexParser.h"
#include "regex/RegexUtils.h"

#include <stdexcept>
#include <vector>

namespace compiler {

std::string RegexParser::preprocess(const std::string& regex) {

    std::string expanded = RegexUtils::expandRanges(regex);

    return RegexUtils::insertExplicitConcat(expanded);
}

std::shared_ptr<RegexNode> RegexParser::parse(const std::string& regex) {

    std::string infix = preprocess(regex);

    std::string postfix = RegexUtils::infixToPostfix(infix);

    std::vector<std::shared_ptr<RegexNode>> stack;

    for (size_t i = 0; i < postfix.size(); ++i) {
        char current = postfix[i];

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

        stack.push_back(std::make_shared<LiteralNode>(current));
    }

    if (stack.size() != 1) {
        throw std::invalid_argument("Invalid regular expression");
    }

    return stack.back();
}

}
