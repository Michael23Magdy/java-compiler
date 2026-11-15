#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace compiler {

struct LexicalError {
    std::string message;
    char invalidChar;
    int line;
    int column;
};

class ErrorHandler {
public:
    ErrorHandler() = default;

    void reportError(const std::string& message, int line, int col, char invalidChar = '\0');

    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<LexicalError>& getErrors() const { return errors_; }

    void printErrors(std::ostream& out = std::cerr) const;

    void clear() { errors_.clear(); }

private:
    std::vector<LexicalError> errors_;
};

}
