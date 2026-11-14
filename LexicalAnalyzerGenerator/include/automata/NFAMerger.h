#pragma once

#include "NFA.h"
#include <vector>

namespace compiler {

/**
 * @brief Merges individual token NFAs into a single combined NFA.
 * Adds a new starting state with epsilon transitions to the starting states
 * of each token NFA, preserving acceptance conditions and priority.
 */
class NFAMerger {
public:
    NFAMerger() = default;

    /**
     * @brief Merges a list of token NFAs into a combined NFA.
     * @param nfas List of token NFAs in priority order.
     * @return Single combined NFA.
     */
    NFA merge(const std::vector<NFA>& nfas);
};

} // namespace compiler
