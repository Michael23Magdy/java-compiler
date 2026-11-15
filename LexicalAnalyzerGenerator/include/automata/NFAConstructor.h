#pragma once

#include "NFA.h"
#include "regex/RegexAST.h"
#include <string>
#include <memory>

namespace compiler {

class NFAConstructor {
public:
    NFAConstructor() = default;

    NFA buildFromAST(const std::shared_ptr<RegexNode>& root, const std::string& tokenClass = "", TokenPriority priority = -1);

    static NFA createLiteralNFA(char ch);
    static NFA createEpsilonNFA();
    static NFA createConcatNFA(const NFA& first, const NFA& second);
    static NFA createUnionNFA(const NFA& first, const NFA& second);
    static NFA createStarNFA(const NFA& base);
    static NFA createPlusNFA(const NFA& base);
};

}
