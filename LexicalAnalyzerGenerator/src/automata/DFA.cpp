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

void DFA::addTransition(StateID from, StateID to, Symbol symbol) {
    if (from >= 0 && static_cast<size_t>(from) < states_.size()) {
        states_[from].transitions[symbol] = to;
        addToAlphabet(symbol);
    }
}

void DFA::setAccepting(StateID state, bool isAccepting, const std::string& tokenClass, TokenPriority priority) {
    if (state >= 0 && static_cast<size_t>(state) < states_.size()) {
        states_[state].isAccepting = isAccepting;
        states_[state].tokenClass = tokenClass;
        states_[state].priority = priority;
        if (isAccepting) {
            acceptStates_.push_back(state);
        }
    }
}

void DFA::addToAlphabet(Symbol symbol) {
    alphabet_.insert(symbol);
}

StateID DFA::getTransition(StateID from, Symbol symbol) const {
    if (from >= 0 && static_cast<size_t>(from) < states_.size()) {
        const auto& trans = states_[from].transitions;
        auto it = trans.find(symbol);
        if (it != trans.end()) {
            return it->second;
        }
    }
    return INVALID_STATE;
}

} // namespace compiler
