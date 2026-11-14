#include "automata/DFAMinimizer.h"

namespace compiler {

DFAMinimizer::Partition DFAMinimizer::initialPartition(const DFA& dfa) {
    Partition partition;

    // TODO: Create initial partition:
    // Group 1: Non-accepting states
    // Group 2..N: Accepting states partitioned by their token class / priority
    // (states with different token classes MUST NOT be merged)

    return partition;
}

int DFAMinimizer::findGroup(StateID state, const Partition& partition) const {
    // TODO: Find which group/equivalence class in partition contains 'state'
    for (size_t i = 0; i < partition.size(); ++i) {
        if (partition[i].find(state) != partition[i].end()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

DFAMinimizer::Partition DFAMinimizer::refinePartition(const Partition& current, const DFA& dfa) {
    Partition refined;

    // TODO: Implement partition refinement (Hopcroft's-style):
    // For each group in current partition, check if all states transition to the same group
    // for every symbol in the alphabet. If not, split the group.

    return refined;
}

DFA DFAMinimizer::minimize(const DFA& dfa) {
    DFA minimalDfa;

    // TODO: Implement DFA minimization:
    // 1. Compute initial partition
    // 2. Iteratively refine partition until no changes occur
    // 3. Construct minimized DFA from final partition equivalence classes
    // 4. Set start state (group containing original start state)
    // 5. Set accept states and token classes for each group

    return minimalDfa;
}

} // namespace compiler
