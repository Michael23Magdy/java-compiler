#pragma once

#include "automata/DFA.h"
#include "common/Types.h"
#include "common/Constants.h"
#include <vector>
#include <set>
#include <map>

namespace compiler {

/**
 * @brief Represents a partition of DFA states into equivalence groups.
 * - groups: list of state sets, where index is groupID.
 * - stateToGroup: maps stateID -> groupID for O(1) group queries.
 */
struct Partition {
    std::vector<std::set<StateID>> groups;
    std::vector<int> stateToGroup;

    /**
     * @brief Adds a new group of states to the partition and updates stateToGroup mapping.
     * @return The newly assigned group index.
     */
    int addGroup(const std::set<StateID>& states) {
        int groupIdx = static_cast<int>(groups.size());
        groups.push_back(states);
        for (StateID s : states) {
            if (s >= static_cast<int>(stateToGroup.size())) {
                stateToGroup.resize(s + 1, -1);
            }
            if (s >= 0) {
                stateToGroup[s] = groupIdx;
            }
        }
        return groupIdx;
    }
};

/**
 * @brief Minimizes a DFA using partition refinement (Hopcroft's-style algorithm).
 * Distinguishes accepting states by their token classes and priorities.
 */
class DFAMinimizer {
public:
    // Alias Partition so both `Partition` and `DFAMinimizer::Partition` are fully valid
    using Partition = compiler::Partition;

    DFAMinimizer() = default;

    /**
     * @brief Produces a minimal DFA equivalent to the input DFA.
     * @param dfa Input deterministic finite automaton.
     * @return Minimized DFA with redundant states merged and dead states handled.
     */
    DFA minimize(const DFA& dfa);

    /**
     * @brief Looks up which group a state belongs to in O(1).
     * Returns -1 if the state is INVALID_STATE or not in the partition.
     */
    int getGroup(StateID state, const Partition& partition) const;

    /**
     * @brief Helper to add a group of states into a partition.
     */
    int addGroup(Partition& partition, const std::set<StateID>& states) const {
        return partition.addGroup(states);
    }

    DFA constructMinimizedDFA(const DFA& dfa, const Partition& partition) ;

private:
    Partition initialPartition(const DFA& dfa);
    Partition refinePartition(const Partition& current, const DFA& dfa);
};

} // namespace compiler
