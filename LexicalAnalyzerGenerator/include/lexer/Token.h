#pragma once

#include <string>

namespace compiler {

/**
 * @brief Represents a token produced by the lexical analyzer.
 */
struct Token {
    std::string tokenClass; // Token type/class (e.g., "id", "num", "int", "relop")
    std::string value;      // Lexeme attribute value (e.g., "sum", "10", "==")
    int line = 1;           // 1-based line number in source
    int column = 1;         // 1-based column number in source

    bool isEOF() const { return tokenClass == "$"; }

    std::string toString() const {
        return tokenClass + " ('" + value + "') at " + std::to_string(line) + ":" + std::to_string(column);
    }
};

} // namespace compiler
