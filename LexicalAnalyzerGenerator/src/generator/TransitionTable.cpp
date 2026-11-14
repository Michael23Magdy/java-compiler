#include "generator/TransitionTable.h"

namespace compiler {

TransitionTable TransitionTable::fromDFA(const DFA& dfa) {
    TransitionTable table;

    // TODO: Convert DFA into 2D transition table representation:
    // 1. Populate alphabet and symbolToIndex mapping
    // 2. Allocate table_ of size [stateCount][alphabetSize]
    // 3. Fill in transitions (DEAD_STATE / INVALID_STATE for missing transitions)
    // 4. Fill in stateInfos_ with acceptance and token class data

    table.startState_ = dfa.getStartState();
    table.stateCount_ = dfa.size();

    return table;
}

StateID TransitionTable::getNextState(StateID current, Symbol symbol) const {
    // TODO: Look up next state from table_ using current state and symbol
    auto it = symbolToIndex_.find(symbol);
    if (it != symbolToIndex_.end() && current >= 0 && static_cast<size_t>(current) < table_.size()) {
        return table_[current][it->second];
    }
    return INVALID_STATE;
}

bool TransitionTable::isAccepting(StateID state) const {
    if (state >= 0 && static_cast<size_t>(state) < stateInfos_.size()) {
        return stateInfos_[state].isAccepting;
    }
    return false;
}

std::string TransitionTable::getTokenClass(StateID state) const {
    if (state >= 0 && static_cast<size_t>(state) < stateInfos_.size()) {
        return stateInfos_[state].tokenClass;
    }
    return "";
}

TokenPriority TransitionTable::getPriority(StateID state) const {
    if (state >= 0 && static_cast<size_t>(state) < stateInfos_.size()) {
        return stateInfos_[state].priority;
    }
    return -1;
}

} // namespace compiler
