#pragma once

#include "common/Types.h"
#include "common/Constants.h"
#include <map>
#include <set>
#include <vector>
#include <string>

namespace compiler {

struct DFAState {
    StateID id;
    bool isAccepting = false;
    std::string tokenClass = "";
    TokenPriority priority = -1;
    std::map<Symbol, StateID> transitions;
    std::set<StateID> nfaStates;
};

class DFA {
public:
    DFA();

    StateID addState();
    void addTransition(StateID from, StateID to, Symbol symbol);
    void setAccepting(StateID state, bool isAccepting, const std::string& tokenClass = "", TokenPriority priority = -1);
    
    void setNFAStates(StateID state, const std::set<StateID>& nfaStates);
    std::set<StateID> getNFAStates(StateID state) const;

    StateID getStartState() const { return startState_; }
    void setStartState(StateID start) { startState_ = start; }

    const std::vector<StateID>& getAcceptStates() const { return acceptStates_; }
    const DFAState& getState(StateID id) const { return states_.at(id); }
    DFAState& getState(StateID id) { return states_.at(id); }

    const std::vector<DFAState>& getStates() const { return states_; }
    size_t size() const { return states_.size(); }

    const std::set<Symbol>& getAlphabet() const { return alphabet_; }
    void setAlphabet(const std::set<Symbol>& alphabet) { alphabet_ = alphabet; }
    void addToAlphabet(Symbol symbol);

    StateID getTransition(StateID from, Symbol symbol) const;

private:
    StateID startState_ = 0;
    std::vector<StateID> acceptStates_;
    std::vector<DFAState> states_;
    std::set<Symbol> alphabet_;

    bool validState(StateID state) const;
};

}
