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
    skipWhitespace();

    if (cursor_ >= source_.length()) {
        return Token{"$", "$", currentLine_, currentColumn_};
    }

    // TODO: Implement DFA Simulation with Maximal Munch (Longest Match):
    // 1. Start from DFA startState_
    // 2. Read characters from source_[cursor_] one by one and transition in table_
    // 3. Keep track of the last accepting state seen and the input position at that accept state
    // 4. When DFA reaches DEAD_STATE or EOF:
    //    a) If an accepting state was reached earlier:
    //       - Roll back cursor_ to the position of that match (longest prefix)
    //       - Extract lexeme
    //       - Get token class from accepting state
    //       - If tokenClass == "id", insert into symbolTable_
    //       - Return Token{tokenClass, lexeme, startLine, startCol}
    //    b) If no accepting state was reached:
    //       - Call errorHandler_.reportError(...)
    //       - Advance cursor_ by 1 (error recovery) and retry / continue

    // Temporary placeholder token
    Token token{"UNKNOWN", std::string(1, source_[cursor_]), currentLine_, currentColumn_};
    cursor_++;
    currentColumn_++;
    return token;
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
