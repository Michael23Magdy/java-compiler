#pragma once

#include "NFA.h"
#include "DFA.h"
#include <set>
#include <vector>

namespace compiler {

class DFABuilder {
public:
    DFABuilder() = default;

    DFA build(const NFA &nfa);
    
    void handleAccepting(const std::set<compiler::StateID> &currentDFAStateNFAStates, const compiler::NFA &nfa, compiler::DFA &dfa, compiler::StateID currentDFAStateID);
    
    static std::set<StateID> epsilonClosure(StateID state, const NFA& nfa);

    static std::set<StateID> epsilonClosure(const std::set<StateID>& states, const NFA& nfa);

    static std::set<StateID> move(const std::set<StateID>& states, Symbol symbol, const NFA& nfa);
};

}
