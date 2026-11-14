#pragma once

#include "DFA.h"
#include <vector>
#include <set>

namespace compiler {

/**
 * @brief Minimizes a DFA using partition refinement (Hopcroft's-style algorithm).
 * Distinguishes accepting states by their token classes and priorities.
 */
class DFAMinimizer {
public:
    DFAMinimizer() = default;

    /**
     * @brief Produces a minimal DFA equivalent to the input DFA.
     * @param dfa Input deterministic finite automaton.
     * @return Minimized DFA with redundant states merged and dead states handled.
     */
    DFA minimize(const DFA& dfa);

private:
    using Partition = std::vector<std::set<StateID>>;

    Partition initialPartition(const DFA& dfa);
    Partition refinePartition(const Partition& current, const DFA& dfa);
    int findGroup(StateID state, const Partition& partition) const;
};

} // namespace compiler
