#include "automata/NFAMerger.h"
#include <stdexcept>

namespace compiler {

NFA NFAMerger::merge(const std::vector<NFA>& nfas) {
    if (nfas.empty()) {
        throw std::invalid_argument("Cannot merge an empty list of NFAs");
    }

    NFA combined;

    // Create a new common start state.
    StateID start = combined.addState();
    combined.setStartState(start);

    std::vector<StateID> acceptingStates;

    // Append each NFA and connect it to the common start.
    for (const NFA& nfa : nfas) {
        std::vector<StateID> stateMap;

        StateID nfaStart = combined.append(nfa, stateMap);

        // Connect the common start to this NFA.
        combined.addTransition(start, nfaStart, EPSILON);

        // Preserve all accepting states.
        for (StateID accept : nfa.getAcceptStates()) {
            acceptingStates.push_back(stateMap[accept]);
        }
    }

    // Set all original accepting states as final accepting states.
    combined.setAcceptStates(acceptingStates);

    return combined;
}

} // namespace compiler
