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

/**
 * @brief Identifies the type of lexical specification entry.
 */
enum class RuleType {
    REGULAR_DEFINITION, // e.g., letter = a-z | A-Z
    REGULAR_EXPRESSION, // e.g., id: letter (letter|digit)*
    KEYWORD,            // e.g., {boolean int float}
    PUNCTUATION         // e.g., [; , \( \) { }]
};

/**
 * @brief Represents a parsed lexical rule.
 */
struct Rule {
    std::string name;           // Rule identifier/token class name
    std::string expression;     // Regular expression or expanded definition
    RuleType type;              // Category of rule
    TokenPriority priority;     // Priority (lower value = higher priority)
};

/**
 * @brief Information associated with an automaton state.
 */
struct StateInfo {
    StateID id = -1;
    bool isAccepting = false;
    std::string tokenClass = "";
    TokenPriority priority = -1;
};

} // namespace compiler
