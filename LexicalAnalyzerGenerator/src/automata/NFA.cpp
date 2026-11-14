#include "automata/NFA.h"

namespace compiler {

NFA::NFA() : startState_(0) {}

StateID NFA::addState() {
    StateID id = static_cast<StateID>(states_.size());
    NFAState state;
    state.id = id;
    states_.push_back(state);
    return id;
}

void NFA::addTransition(StateID from, StateID to, Symbol symbol) {
    if (from >= 0 && static_cast<size_t>(from) < states_.size()) {
        states_[from].transitions[symbol].push_back(to);
        if (symbol != EPSILON) {
            addToAlphabet(symbol);
        }
    }
}

void NFA::setAccepting(StateID state, bool isAccepting, const std::string& tokenClass, TokenPriority priority) {
    if (state >= 0 && static_cast<size_t>(state) < states_.size()) {
        states_[state].isAccepting = isAccepting;
        states_[state].tokenClass = tokenClass;
        states_[state].priority = priority;
        if (isAccepting) {
            acceptStates_.push_back(state);
        }
    }
}

void NFA::addToAlphabet(Symbol symbol) {
    if (symbol != EPSILON) {
        alphabet_.insert(symbol);
    }
}

std::vector<StateID> NFA::getTransitions(StateID from, Symbol symbol) const {
    if (from >= 0 && static_cast<size_t>(from) < states_.size()) {
        const auto& trans = states_[from].transitions;
        auto it = trans.find(symbol);
        if (it != trans.end()) {
            return it->second;
        }
    }
    return {};
}

} // namespace compiler
