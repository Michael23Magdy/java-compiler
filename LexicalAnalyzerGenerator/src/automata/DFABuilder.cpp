#include "automata/DFABuilder.h"
#include <queue>

namespace compiler {

std::set<StateID> DFABuilder::epsilonClosure(StateID state, const NFA& nfa) {
    return epsilonClosure(std::set<StateID>{state}, nfa);
}

std::set<StateID> DFABuilder::epsilonClosure(const std::set<StateID>& states, const NFA& nfa) {
    std::set<StateID> closure = states;
    std::vector<StateID> stack(states.begin(), states.end());
    while (!stack.empty()) {
        StateID current = stack.back();
        stack.pop_back();
        for (const auto& nextState : nfa.getTransitions(current, EPSILON)) {
            if (closure.find(nextState) == closure.end()) {
                closure.insert(nextState);
                stack.push_back(nextState);
            }
        }
    }
    return closure;
}

std::set<StateID> DFABuilder::move(const std::set<StateID>& states, Symbol symbol, const NFA& nfa) {
    std::set<StateID> result;
    for (StateID state : states) {
        for (StateID nextState : nfa.getTransitions(state, symbol)) {
            result.insert(nextState);
        }
    }
    return result;
}

DFA DFABuilder::build(const NFA& nfa) {
    DFA dfa;
    StateID initialDFAStateID = dfa.addState();
    dfa.setStartState(initialDFAStateID);
    dfa.setNFAStates(initialDFAStateID, epsilonClosure(nfa.getStartState(), nfa));
    
    std::map<std::set<StateID>, StateID> dfaStateMap;
    dfaStateMap[dfa.getNFAStates(initialDFAStateID)] = initialDFAStateID;
    std::queue<StateID> worklist;
    worklist.push(initialDFAStateID);

    while(!worklist.empty()) {
        StateID currentDFAStateID = worklist.front();
        worklist.pop();
        const std::set<StateID> currentDFAStateNFAStates = dfa.getNFAStates(currentDFAStateID);

        for (Symbol symbol : nfa.getAlphabet()) {
            std::set<StateID> nextNFAStates = epsilonClosure(move(currentDFAStateNFAStates, symbol, nfa), nfa);
            if (nextNFAStates.empty()) continue;
            if (dfaStateMap.find(nextNFAStates) == dfaStateMap.end()) {
                StateID newDFAStateID = dfa.addState();
                dfa.setNFAStates(newDFAStateID, nextNFAStates);
                dfaStateMap[nextNFAStates] = newDFAStateID;
                worklist.push(newDFAStateID);
            }
            StateID nextDFAStateID = dfaStateMap[nextNFAStates];
            dfa.addTransition(currentDFAStateID, nextDFAStateID, symbol);
        }

        handleAccepting(currentDFAStateNFAStates, nfa, dfa, currentDFAStateID);
    }

    dfa.setAlphabet(nfa.getAlphabet());
    return dfa;
}

void DFABuilder::handleAccepting(const std::set<StateID> &NFAStates, const NFA &nfa, DFA &dfa, StateID DFAStateID)
{
    bool isAccepting = false;
    std::string tokenClass;
    TokenPriority priority = -1;
    for (StateID nfaState : NFAStates) {
        const NFAState &nfaStateObj = nfa.getState(nfaState);
        if (nfaStateObj.isAccepting) {
            if (!isAccepting || nfaStateObj.priority < priority) {
                isAccepting = true;
                tokenClass = nfaStateObj.tokenClass;
                priority = nfaStateObj.priority;
            }
        }
    }
    if (isAccepting) {
        dfa.setAccepting(DFAStateID, true, tokenClass, priority);
    }
}

}
