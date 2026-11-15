#include "automata/DFA.h"

namespace compiler {

DFA::DFA() : startState_(0) {}

StateID DFA::addState() {
    StateID id = static_cast<StateID>(states_.size());
    DFAState state;
    state.id = id;
    states_.push_back(state);
    return id;
}

bool DFA::validState(StateID state) const {
    return state >= 0 && static_cast<size_t>(state) < states_.size();
}

void DFA::addTransition(StateID from, StateID to, Symbol symbol) {
    if (!validState(from) || !validState(to)) return;
    states_[from].transitions[symbol] = to;
    addToAlphabet(symbol);
}

void DFA::setNFAStates(StateID state, const std::set<StateID>& nfaStates) {
    if (!validState(state)) return;
    states_[state].nfaStates = nfaStates;
}

std::set<StateID> DFA::getNFAStates(StateID state) const {
    if (!validState(state)) return {};
    return states_[state].nfaStates;
}

void DFA::setAccepting(StateID state, bool isAccepting, const std::string& tokenClass, TokenPriority priority) {
    if (!validState(state)) return;
    states_[state].isAccepting = isAccepting;
    states_[state].tokenClass = tokenClass;
    states_[state].priority = priority;
    if (isAccepting) {
        acceptStates_.push_back(state);
    }
}

void DFA::addToAlphabet(Symbol symbol) {
    alphabet_.insert(symbol);
}

StateID DFA::getTransition(StateID from, Symbol symbol) const {
    if (!validState(from)) return INVALID_STATE;
    const auto& trans = states_[from].transitions;
    auto it = trans.find(symbol);
    if (it == trans.end()) return INVALID_STATE;
    return it->second;
}

} // namespace compiler
