#include <iostream>
#include <cassert>
#include "automata/NFA.h"
#include "automata/NFAConstructor.h"
#include "automata/NFAMerger.h"

void testThompsonConstruction() {
    std::cout << "[Test] Running Thompson Construction tests...\n";
    // TODO: Add unit tests for basic Thompson operations:
    // 1. Literal NFA
    // 2. Concat NFA
    // 3. Union NFA
    // 4. Star NFA
    std::cout << "  -> Thompson construction tests passed (stub).\n";
}

void testNFAMerging() {
    std::cout << "[Test] Running NFA Merger tests...\n";
    // TODO: Add unit tests for merging multiple token NFAs
    std::cout << "  -> NFA Merger tests passed (stub).\n";
}

int main() {
    std::cout << "=== Running NFA Unit Tests ===\n";
    testThompsonConstruction();
    testNFAMerging();
    std::cout << "All NFA tests completed.\n";
    return 0;
}
