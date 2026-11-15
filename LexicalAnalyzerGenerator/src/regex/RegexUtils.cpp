#include "regex/RegexUtils.h"

namespace compiler {

bool RegexUtils::isOperator(char ch) {
    // Return true if character is a regex operator ('|', '*', '+', '(', ')', EXPLICIT_CONCAT)
    return ch == '|' || ch == '*' || ch == '+' || ch == EXPLICIT_CONCAT || ch == '(' || ch == ')';
}

int RegexUtils::getPrecedence(char op) {
    // Define operator precedence for Shunting-yard algorithm
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
    // Expand character ranges like 'a-z', 'A-Z', '0-9' into '(a|b|...|z)'
    std::string result;

    for (size_t i = 0; i < regex.size();) {
        char current = regex[i];

        // Preserve escaped characters.
        if (current == '\\') {
            if (i + 1 >= regex.size()) {
                throw std::invalid_argument("Incomplete escape sequence");
            }

            result += regex[i];
            result += regex[i + 1];
            i += 2;
            continue;
        }

        // Ignore whitespace in regular expressions.
        if (std::isspace(static_cast<unsigned char>(current))) {
            ++i;
            continue;
        }

        // Check whether the current character starts a range.
        if (std::isalnum(static_cast<unsigned char>(current))) {
            size_t dashPos = i + 1;

            while (dashPos < regex.size() &&
                   std::isspace(static_cast<unsigned char>(regex[dashPos]))) {
                ++dashPos;
            }

            if (dashPos < regex.size() && regex[dashPos] == '-') {
                size_t endPos = dashPos + 1;

                while (endPos < regex.size() &&
                       std::isspace(static_cast<unsigned char>(regex[endPos]))) {
                    ++endPos;
                }

                if (endPos < regex.size() &&
                    std::isalnum(static_cast<unsigned char>(regex[endPos]))) {

                    char end = regex[endPos];

                    if (current > end) {
                        throw std::invalid_argument("Invalid descending character range");
                    }

                    result += '(';

                    for (char ch = current; ch <= end; ++ch) {
                        if (ch != current) {
                            result += '|';
                        }

                        result += ch;
                    }

                    result += ')';
                    i = endPos + 1;
                    continue;
                }
            }
        }

        result += current;
        ++i;
    }

    return result;
}

std::string RegexUtils::insertExplicitConcat(const std::string& regex) {
    // Insert explicit concatenation operator (EXPLICIT_CONCAT) between adjacent tokens
    // e.g. "ab" -> "a·b", "a(b)" -> "a·(b)", "(a)b" -> "(a)·b", "a*b" -> "a*·b"
    std::string result;
    bool previousCanEndAtom = false;

    for (size_t i = 0; i < regex.size();) {
        char current = regex[i];
        std::string token;
        bool escaped = false;

        // Treat an escaped character as one literal operand.
        if (current == '\\') {
            if (i + 1 >= regex.size()) {
                throw std::invalid_argument("Incomplete escape sequence");
            }

            token += regex[i];
            token += regex[i + 1];
            escaped = true;
            i += 2;
        }
        else {
            if (std::isspace(static_cast<unsigned char>(current))) {
                ++i;
                continue;
            }

            token += current;
            ++i;
        }

        bool currentCanStartAtom =
            escaped || current == '(' || !isOperator(current);

        if (previousCanEndAtom && currentCanStartAtom) {
            result += EXPLICIT_CONCAT;
        }

        result += token;

        // These tokens can be followed by an implicit concatenation.
        previousCanEndAtom =
            escaped ||
            current == ')' ||
            current == '*' ||
            current == '+' ||
            !isOperator(current);
    }

    return result;
}

std::string RegexUtils::infixToPostfix(const std::string& infix) {
    // Convert infix regex to postfix using the Shunting-yard algorithm
    std::string result;
    std::vector<char> operators;

    for (size_t i = 0; i < infix.size();) {
        char current = infix[i];

        // Preserve escaped characters as literal operands.
        if (current == '\\') {
            if (i + 1 >= infix.size()) {
                throw std::invalid_argument("Incomplete escape sequence");
            }

            result += infix[i];
            result += infix[i + 1];
            i += 2;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(current))) {
            ++i;
            continue;
        }

        // Opening parenthesis.
        if (current == '(') {
            operators.push_back(current);
        }
        // Closing parenthesis: pop until the matching opening parenthesis.
        else if (current == ')') {
            while (!operators.empty() && operators.back() != '(') {
                result += operators.back();
                operators.pop_back();
            }

            if (operators.empty()) {
                throw std::invalid_argument("Mismatched closing parenthesis");
            }

            operators.pop_back();
        }
        // Unary postfix operators are emitted immediately.
        else if (current == '*' || current == '+') {
            result += current;
        }
        // Binary operators: union and explicit concatenation.
        else if (current == '|' || current == EXPLICIT_CONCAT) {
            while (!operators.empty() &&
                   operators.back() != '(' &&
                   getPrecedence(operators.back()) >= getPrecedence(current)) {

                result += operators.back();
                operators.pop_back();
            }

            operators.push_back(current);
        }
        // Any other character is an operand.
        else {
            result += current;
        }

        ++i;
    }

    // Pop the remaining operators.
    while (!operators.empty()) {
        if (operators.back() == '(') {
            throw std::invalid_argument("Mismatched opening parenthesis");
        }

        result += operators.back();
        operators.pop_back();
    }

    return result;
}

} // namespace compiler
