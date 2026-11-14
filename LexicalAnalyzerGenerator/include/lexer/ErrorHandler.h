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

/**
 * @brief Handles and records lexical errors and recovery actions.
 */
class ErrorHandler {
public:
    ErrorHandler() = default;

    /**
     * @brief Reports a lexical error and records it.
     */
    void reportError(const std::string& message, int line, int col, char invalidChar = '\0');

    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<LexicalError>& getErrors() const { return errors_; }

    /**
     * @brief Prints all recorded errors to the specified stream.
     */
    void printErrors(std::ostream& out = std::cerr) const;

    void clear() { errors_.clear(); }

private:
    std::vector<LexicalError> errors_;
};

} // namespace compiler
