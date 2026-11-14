#include "regex/RegexUtils.h"

namespace compiler {

bool RegexUtils::isOperator(char ch) {
    // TODO: Return true if character is a regex operator ('|', '*', '+', '(', ')', EXPLICIT_CONCAT)
    return ch == '|' || ch == '*' || ch == '+' || ch == EXPLICIT_CONCAT || ch == '(' || ch == ')';
}

int RegexUtils::getPrecedence(char op) {
    // TODO: Define operator precedence for Shunting-yard algorithm
    switch (op) {
        case '*':
        case '+':
            return 3;
        case EXPLICIT_CONCAT:
            return 2;
        case '|':
            return 1;
        case '(':
            return 0;
        default:
            return -1;
    }
}

std::string RegexUtils::expandRanges(const std::string& regex) {
    // TODO: Expand character ranges like 'a-z', 'A-Z', '0-9' into '(a|b|...|z)'
    return regex;
}

std::string RegexUtils::insertExplicitConcat(const std::string& regex) {
    // TODO: Insert explicit concatenation operator (EXPLICIT_CONCAT) between adjacent tokens
    // e.g. "ab" -> "a·b", "a(b)" -> "a·(b)", "(a)b" -> "(a)·b", "a*b" -> "a*·b"
    return regex;
}

std::string RegexUtils::infixToPostfix(const std::string& infix) {
    // TODO: Convert infix regex to postfix using the Shunting-yard algorithm
    return infix;
}

} // namespace compiler
