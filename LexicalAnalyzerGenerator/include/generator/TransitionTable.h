#pragma once

#include "automata/DFA.h"
#include "common/Types.h"
#include <vector>
#include <map>
#include <set>
#include <string>

namespace compiler {

class TransitionTable {
public:
    TransitionTable() = default;

    static TransitionTable fromDFA(const DFA& dfa);

    StateID getNextState(StateID current, Symbol symbol) const;
    bool isAccepting(StateID state) const;
    std::string getTokenClass(StateID state) const;
    TokenPriority getPriority(StateID state) const;

    StateID getStartState() const { return startState_; }
    size_t getStateCount() const { return stateCount_; }
    const std::vector<Symbol>& getAlphabet() const { return alphabet_; }
    const std::vector<std::vector<StateID>>& getTable() const { return table_; }
    const std::vector<StateInfo>& getStateInfos() const { return stateInfos_; }

private:
    StateID startState_ = 0;
    size_t stateCount_ = 0;
    std::vector<Symbol> alphabet_;
    std::map<Symbol, int> symbolToIndex_;

    std::vector<std::vector<StateID>> table_;
    std::vector<StateInfo> stateInfos_;
};

}
