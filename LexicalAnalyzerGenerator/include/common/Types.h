#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cstdint>

namespace compiler {

using StateID = int;
using Symbol = char;
using TokenPriority = int;

enum class RuleType {
    REGULAR_DEFINITION,
    REGULAR_EXPRESSION,
    KEYWORD,
    PUNCTUATION
};

struct Rule {
    std::string name;
    std::string expression;
    RuleType type;
    TokenPriority priority;
};

struct StateInfo {
    StateID id = -1;
    bool isAccepting = false;
    std::string tokenClass = "";
    TokenPriority priority = -1;
};

}
