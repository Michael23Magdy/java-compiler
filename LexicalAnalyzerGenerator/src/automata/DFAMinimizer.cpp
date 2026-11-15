#include "automata/DFAMinimizer.h"
#include <map>

namespace compiler {

DFAMinimizer::Partition DFAMinimizer::initialPartition(const DFA& dfa) {
    Partition partition;
    partition.stateToGroup.assign(dfa.size(), -1);

    std::set<StateID> nonAcceptingStates;
    std::map<std::pair<std::string, TokenPriority>, std::set<StateID>> acceptingGroups;
    const std::vector<DFAState>& allStates = dfa.getStates();

    for (const DFAState& state : allStates) {
        if (state.isAccepting) {
            acceptingGroups[std::make_pair(state.tokenClass, state.priority)].insert(state.id);
        } else {
            nonAcceptingStates.insert(state.id);
        }
    }

    if (!nonAcceptingStates.empty()) {
        partition.addGroup(nonAcceptingStates);
    }

    for (const auto& [key, states] : acceptingGroups) {
        partition.addGroup(states);
    }

    return partition;
}

int DFAMinimizer::getGroup(StateID state, const Partition& partition) const {
    if (state == INVALID_STATE || state < 0 || static_cast<size_t>(state) >= partition.stateToGroup.size()) {
        return -1;
    }
    return partition.stateToGroup[state];
}

DFAMinimizer::Partition DFAMinimizer::refinePartition(const Partition& current, const DFA& dfa) {
    Partition refined;

    for (auto& group : current.groups) {
        std::map<std::vector<int>, std::set<StateID>> transitionMap;
        for (StateID state : group) {
            std::vector<int> transitions;
            for (Symbol symbol : dfa.getAlphabet()) {
                StateID nextState = dfa.getTransition(state, symbol);
                transitions.push_back(getGroup(nextState, current));
            }
            transitionMap[transitions].insert(state);
        }

        for (const auto& [transitions, states] : transitionMap) {
            refined.addGroup(states);
        }
    }

    return refined;
}

DFA DFAMinimizer::minimize(const DFA& dfa) {
    DFA minimalDfa;
    Partition partition = initialPartition(dfa);
    Partition refinedPartition = refinePartition(partition, dfa);
    while (refinedPartition.groups.size() != partition.groups.size()) {
        partition = refinedPartition;
        refinedPartition = refinePartition(partition, dfa);
    }

    minimalDfa = constructMinimizedDFA(dfa, partition);
    minimalDfa.setAlphabet(dfa.getAlphabet());
    return minimalDfa;
}

DFA DFAMinimizer::constructMinimizedDFA(const DFA& dfa, const Partition& partition) {
    DFA minimizedDfa;
    std::map<int, StateID> groupToStateID;

    for (size_t groupIdx = 0; groupIdx < partition.groups.size(); groupIdx++) {
        StateID newStateID = minimizedDfa.addState();
        groupToStateID[groupIdx] = newStateID;

        for (StateID oldState : partition.groups[groupIdx]) {
            const DFAState& oldDFAState = dfa.getState(oldState);
            if (oldDFAState.isAccepting) {
                minimizedDfa.setAccepting(newStateID, true, oldDFAState.tokenClass, oldDFAState.priority);
                break;
            }
        }
    }

    for (size_t groupIdx = 0; groupIdx < partition.groups.size(); groupIdx++) {
        StateID newFromState = groupToStateID[groupIdx];
        for (Symbol symbol : dfa.getAlphabet()) {
            StateID representativeOldState = *partition.groups[groupIdx].begin();
            StateID oldToState = dfa.getTransition(representativeOldState, symbol);
            int toGroupIdx = getGroup(oldToState, partition);
            if (toGroupIdx != -1) {
                StateID newToState = groupToStateID[toGroupIdx];
                minimizedDfa.addTransition(newFromState, newToState, symbol);
            }
        }
    }

    int startGroupIdx = getGroup(dfa.getStartState(), partition);
    if (startGroupIdx != -1) {
        minimizedDfa.setStartState(groupToStateID[startGroupIdx]);
    }

    return minimizedDfa;
}

} // namespace compiler
