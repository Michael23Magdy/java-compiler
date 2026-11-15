#include "lexer/Lexer.h"
#include <cctype>

namespace compiler {

Lexer::Lexer(const TransitionTable& table, std::string sourceCode)
    : table_(table), source_(std::move(sourceCode)) {}

void Lexer::setSource(const std::string& sourceCode) {
    source_ = sourceCode;
    cursor_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
}

void Lexer::skipWhitespace() {
    while (cursor_ < source_.length() && std::isspace(source_[cursor_])) {
        if (source_[cursor_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        cursor_++;
    }
}

Token Lexer::getNextToken() {
    while(true){
        skipWhitespace();

        if (cursor_ >= source_.length()) {
            return Token{"$", "$", currentLine_, currentColumn_};
        }

        int startLine = currentLine_;
        int startCol = currentColumn_;

        StateID state = table_.getStartState();
        StateID lastAcceptingState = INVALID_STATE;
        size_t lastAcceptingPos = cursor_;
        size_t pos = cursor_;
        while (pos < source_.length())
        {
            state = table_.getNextState(state, source_[pos]);
            if (state == DEAD_STATE) break;
            pos++;
            if (table_.isAccepting(state)) {
                lastAcceptingState = state;
                lastAcceptingPos = pos;
            }
        }

        if (lastAcceptingState != INVALID_STATE) {
            std::string lexeme = source_.substr(cursor_, lastAcceptingPos - cursor_);
            std::string tokenClass = table_.getTokenClass(lastAcceptingState);
            for (char c : lexeme) {
                if (c == '\n') {
                    currentLine_++;
                    currentColumn_ = 1;
                } else {
                    currentColumn_++;
                }
            }
            cursor_ = lastAcceptingPos;

            if (tokenClass == "id") {
                symbolTable_.insert(lexeme, startLine);
            }
            return Token{tokenClass, lexeme, startLine, startCol};
        }
        char errorChar = source_[cursor_];
        errorHandler_.reportError("Unrecognized character", startLine, startCol, errorChar);
        cursor_++;
        currentColumn_++;
    }
}

std::vector<Token> Lexer::tokenizeAll() {
    std::vector<Token> tokens;
    while (cursor_ < source_.length()) {
        Token token = getNextToken();
        if (token.isEOF()) {
            break;
        }
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace compiler
