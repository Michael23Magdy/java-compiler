#pragma once

#include "RegexAST.h"
#include <string>
#include <memory>

namespace compiler {

/**
 * @brief Parses regular expression strings into AST representation.
 */
class RegexParser {
public:
    RegexParser() = default;

    /**
     * @brief Parses an infix regular expression into an AST root node.
     * @param regex The regular expression string.
     * @return Shared pointer to root RegexNode.
     */
    std::shared_ptr<RegexNode> parse(const std::string& regex);

    /**
     * @brief Prepares and normalizes regex string (expands ranges, escapes, handles \L).
     */
    std::string preprocess(const std::string& regex);
};

} // namespace compiler
