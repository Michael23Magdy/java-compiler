#include "automata/DFABuilder.h"

namespace compiler {

std::set<StateID> DFABuilder::epsilonClosure(StateID state, const NFA& nfa) {
    // TODO: Compute epsilon-closure of a single state (all states reachable via epsilon transitions)
    return epsilonClosure(std::set<StateID>{state}, nfa);
}

std::set<StateID> DFABuilder::epsilonClosure(const std::set<StateID>& states, const NFA& nfa) {
    // TODO: Compute epsilon-closure for a set of NFA states using BFS or DFS
    std::set<StateID> closure = states;
    return closure;
}

std::set<StateID> DFABuilder::move(const std::set<StateID>& states, Symbol symbol, const NFA& nfa) {
    // TODO: Return set of NFA states reachable from 'states' on input 'symbol'
    std::set<StateID> result;
    return result;
}

DFA DFABuilder::build(const NFA& nfa) {
    DFA dfa;

    // TODO: Implement the Subset Construction Algorithm:
    // 1. Initial DFA state = epsilonClosure(nfa.getStartState())
    // 2. Maintain a queue/worklist of unmarked DFA states
    // 3. For each unmarked DFA state and each symbol in NFA alphabet:
    //    U = epsilonClosure(move(currentState, symbol))
    //    If U is not empty, add transition. If U is a new state, add to worklist.
    // 4. Resolve accepting states: if any NFA state in the subset is accepting,
    //    the DFA state is accepting.
    //    Tie-breaking rule: break tie in favor of the rule with highest priority (lowest priority number).

    return dfa;
}

} // namespace compiler
