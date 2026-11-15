#pragma once

#include "common/Types.h"
#include "common/Constants.h"
#include <map>
#include <set>
#include <vector>
#include <string>

namespace compiler {

struct NFAState {
    StateID id;
    bool isAccepting = false;
    std::string tokenClass = "";
    TokenPriority priority = -1;
    // Maps transition symbol (including EPSILON) to destination state IDs
    std::map<Symbol, std::vector<StateID>> transitions;
};

class NFA {
public:
    NFA();

    StateID addState();
    StateID append(const NFA& other, std::vector<StateID>& stateMap);
    void addTransition(StateID from, StateID to, Symbol symbol);
    void setAccepting(StateID state, bool isAccepting, const std::string& tokenClass = "", TokenPriority priority = -1);

    StateID getStartState() const { return startState_; }
    void setStartState(StateID start) { startState_ = start; }

    const std::vector<StateID>& getAcceptStates() const { return acceptStates_; }
    void setAcceptStates(const std::vector<StateID>& accepts) { acceptStates_ = accepts; }

    const NFAState& getState(StateID id) const { return states_.at(id); }
    NFAState& getState(StateID id) { return states_.at(id); }

    const std::vector<NFAState>& getStates() const { return states_; }
    size_t size() const { return states_.size(); }

    const std::set<Symbol>& getAlphabet() const { return alphabet_; }
    void addToAlphabet(Symbol symbol);

    std::vector<StateID> getTransitions(StateID from, Symbol symbol) const;

private:
    StateID startState_ = 0;
    std::vector<StateID> acceptStates_;
    std::vector<NFAState> states_;
    std::set<Symbol> alphabet_;
};

} // namespace compiler
