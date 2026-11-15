#include "generator/TransitionTable.h"

namespace compiler {

TransitionTable TransitionTable::fromDFA(const DFA& dfa) {
    TransitionTable table;

    const auto& alpha = dfa.getAlphabet();
    table.alphabet_.assign(alpha.begin(), alpha.end());
    for (size_t i = 0; i < table.alphabet_.size(); i++) {
        table.symbolToIndex_[table.alphabet_[i]] = static_cast<int>(i);
    }

    const size_t n = dfa.size();
    table.table_.assign(n, std::vector<StateID>(table.alphabet_.size(), INVALID_STATE));
    table.stateInfos_.resize(n);
    for (size_t s = 0; s < n; s++) {
        const DFAState& st = dfa.getState(static_cast<StateID>(s));
        for (size_t i = 0; i < table.alphabet_.size(); i++) {
            table.table_[s][i] = dfa.getTransition(static_cast<StateID>(s), table.alphabet_[i]);
        }
        table.stateInfos_[s] = StateInfo{static_cast<StateID>(s), st.isAccepting,
                                         st.isAccepting ? st.tokenClass : "",
                                         st.isAccepting ? st.priority : -1};
    }

    table.startState_ = dfa.getStartState();
    table.stateCount_ = dfa.size();

    return table;
}

StateID TransitionTable::getNextState(StateID current, Symbol symbol) const {
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
