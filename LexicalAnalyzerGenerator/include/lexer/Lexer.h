#pragma once

#include "Token.h"
#include "SymbolTable.h"
#include "ErrorHandler.h"
#include "generator/TransitionTable.h"
#include <string>
#include <vector>

namespace compiler {

/**
 * @brief Simulates the generated minimal DFA to tokenize source input.
 * Implements Maximal Munch (longest prefix matching) and integrates with Phase 2 parser via getNextToken().
 */
class Lexer {
public:
    Lexer(const TransitionTable& table, std::string sourceCode = "");

    void setSource(const std::string& sourceCode);

    /**
     * @brief Reads and returns the next matched token from source input.
     * Ready for Phase 2 integration where parser requests tokens sequentially.
     */
    Token getNextToken();

    /**
     * @brief Tokenizes the entire input source code.
     */
    std::vector<Token> tokenizeAll();

    const SymbolTable& getSymbolTable() const { return symbolTable_; }
    SymbolTable& getSymbolTable() { return symbolTable_; }

    const ErrorHandler& getErrorHandler() const { return errorHandler_; }
    ErrorHandler& getErrorHandler() { return errorHandler_; }

private:
    TransitionTable table_;
    std::string source_;
    size_t cursor_ = 0;
    int currentLine_ = 1;
    int currentColumn_ = 1;

    SymbolTable symbolTable_;
    ErrorHandler errorHandler_;

    void skipWhitespace();
};

} // namespace compiler
