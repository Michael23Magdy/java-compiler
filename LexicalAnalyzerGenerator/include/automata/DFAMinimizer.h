#pragma once

#include "automata/DFA.h"
#include "common/Types.h"
#include "common/Constants.h"
#include <vector>
#include <set>
#include <map>

namespace compiler {

struct Partition {
    std::vector<std::set<StateID>> groups;
    std::vector<int> stateToGroup;

    int addGroup(const std::set<StateID>& states) {
        int groupIdx = static_cast<int>(groups.size());
        groups.push_back(states);
        for (StateID s : states) {
            if (s >= static_cast<int>(stateToGroup.size())) {
                stateToGroup.resize(s + 1, -1);
            }
            if (s >= 0) {
                stateToGroup[s] = groupIdx;
            }
        }
        return groupIdx;
    }
};

class DFAMinimizer {
public:
    using Partition = compiler::Partition;

    DFAMinimizer() = default;

    DFA minimize(const DFA& dfa);

    int getGroup(StateID state, const Partition& partition) const;

    int addGroup(Partition& partition, const std::set<StateID>& states) const {
        return partition.addGroup(states);
    }

private:
    DFA constructMinimizedDFA(const DFA& dfa, const Partition& partition) ;
    Partition initialPartition(const DFA& dfa);
    Partition refinePartition(const Partition& current, const DFA& dfa);
};

}
