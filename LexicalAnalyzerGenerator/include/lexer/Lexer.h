#pragma once

#include "Token.h"
#include "SymbolTable.h"
#include "ErrorHandler.h"
#include "generator/TransitionTable.h"
#include <string>
#include <vector>

namespace compiler {

class Lexer {
public:
    Lexer(const TransitionTable& table, std::string sourceCode = "");

    void setSource(const std::string& sourceCode);

    void setIdentifierClass(const std::string& tokenClass) { identifierClass_ = tokenClass; }

    Token getNextToken();

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

    std::string identifierClass_ = "id";
    SymbolTable symbolTable_;
    ErrorHandler errorHandler_;

    void skipWhitespace();
};

}
