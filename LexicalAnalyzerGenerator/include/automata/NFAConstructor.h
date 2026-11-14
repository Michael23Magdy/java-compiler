#pragma once

#include "NFA.h"
#include "regex/RegexAST.h"
#include <string>
#include <memory>

namespace compiler {

/**
 * @brief Constructs NFAs from regular expressions using Thompson's construction.
 */
class NFAConstructor {
public:
    NFAConstructor() = default;

    /**
     * @brief Builds an NFA from a Regex AST.
     */
    NFA buildFromAST(const std::shared_ptr<RegexNode>& root, const std::string& tokenClass = "", TokenPriority priority = -1);

    /**
     * @brief Builds an NFA from a postfix regex string.
     */
    NFA buildFromPostfix(const std::string& postfix, const std::string& tokenClass = "", TokenPriority priority = -1);

    // Thompson basic construction building blocks
    static NFA createLiteralNFA(char ch);
    static NFA createEpsilonNFA();
    static NFA createConcatNFA(const NFA& first, const NFA& second);
    static NFA createUnionNFA(const NFA& first, const NFA& second);
    static NFA createStarNFA(const NFA& base);
    static NFA createPlusNFA(const NFA& base);
};

} // namespace compiler
