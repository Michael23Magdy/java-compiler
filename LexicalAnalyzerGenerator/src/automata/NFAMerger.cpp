#include "automata/NFAMerger.h"
#include <stdexcept>

namespace compiler {

NFA NFAMerger::merge(const std::vector<NFA>& nfas) {
    if (nfas.empty()) {
        throw std::invalid_argument("Cannot merge an empty list of NFAs");
    }

    NFA combined;

    StateID start = combined.addState();
    combined.setStartState(start);

    std::vector<StateID> acceptingStates;

    for (const NFA& nfa : nfas) {
        std::vector<StateID> stateMap;

        StateID nfaStart = combined.append(nfa, stateMap);

        combined.addTransition(start, nfaStart, EPSILON);

        for (StateID accept : nfa.getAcceptStates()) {
            acceptingStates.push_back(stateMap[accept]);
        }
    }

    combined.setAcceptStates(acceptingStates);

    return combined;
}

}
