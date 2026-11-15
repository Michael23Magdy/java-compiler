#pragma once

#include "NFA.h"
#include <vector>

namespace compiler {

class NFAMerger {
public:
    NFAMerger() = default;

    NFA merge(const std::vector<NFA>& nfas);
};

}
