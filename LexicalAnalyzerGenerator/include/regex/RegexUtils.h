#pragma once

#include "common/Constants.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace compiler {

class RegexUtils {
public:

    static std::string expandRanges(const std::string& regex);

    static std::string insertExplicitConcat(const std::string& regex);

    static std::string infixToPostfix(const std::string& infix);

    static bool isOperator(char ch);

    static int getPrecedence(char op);
};

}
