#include "lexer/ErrorHandler.h"

namespace compiler {

void ErrorHandler::reportError(const std::string& message, int line, int col, char invalidChar) {
    errors_.push_back({message, invalidChar, line, col});
}

void ErrorHandler::printErrors(std::ostream& out) const {
    if (errors_.empty()) {
        return;
    }
    out << "=== Lexical Errors (" << errors_.size() << ") ===\n";
    for (const auto& err : errors_) {
        out << "Lexical error at line " << err.line << ", column " << err.column << ": "
            << err.message;
        if (err.invalidChar != '\0') {
            out << " (character: '" << err.invalidChar << "')";
        }
        out << "\n";
    }
}

} // namespace compiler
