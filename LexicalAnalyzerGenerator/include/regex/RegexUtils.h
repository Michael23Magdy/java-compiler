#pragma once

#include "common/Constants.h"
#include <string>
#include <vector>

namespace compiler {

/**
 * @brief Utilities for regular expression preprocessing and conversion.
 */
class RegexUtils {
public:
    /**
     * @brief Expands ranges in regular expression strings (e.g., 'a-z' -> '(a|b|...|z)').
     */
    static std::string expandRanges(const std::string& regex);

    /**
     * @brief Inserts explicit concatenation operators into an infix regular expression.
     */
    static std::string insertExplicitConcat(const std::string& regex);

    /**
     * @brief Converts infix regular expression to postfix (Reverse Polish) notation using Shunting-yard algorithm.
     */
    static std::string infixToPostfix(const std::string& infix);

    /**
     * @brief Checks if a character is an operator in regex syntax ('|', '*', '+', EXPLICIT_CONCAT, '(', ')').
     */
    static bool isOperator(char ch);

    /**
     * @brief Gets operator precedence for Shunting-yard algorithm.
     */
    static int getPrecedence(char op);
};

} // namespace compiler
