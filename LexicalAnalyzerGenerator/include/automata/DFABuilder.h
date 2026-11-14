#pragma once

#include "NFA.h"
#include "DFA.h"
#include <set>
#include <vector>

namespace compiler {

/**
 * @brief Converts an NFA to a DFA using the Subset Construction algorithm.
 */
class DFABuilder {
public:
    DFABuilder() = default;

    /**
     * @brief Converts an NFA into an equivalent DFA.
     * @param nfa Input NFA (usually combined token NFA).
     * @return Equivalent deterministic finite automaton.
     */
    DFA build(const NFA& nfa);

    /**
     * @brief Computes the epsilon-closure of a single state.
     */
    static std::set<StateID> epsilonClosure(StateID state, const NFA& nfa);

    /**
     * @brief Computes the epsilon-closure of a set of states.
     */
    static std::set<StateID> epsilonClosure(const std::set<StateID>& states, const NFA& nfa);

    /**
     * @brief Computes the set of states reachable on symbol from a given set of states.
     */
    static std::set<StateID> move(const std::set<StateID>& states, Symbol symbol, const NFA& nfa);
};

} // namespace compiler
