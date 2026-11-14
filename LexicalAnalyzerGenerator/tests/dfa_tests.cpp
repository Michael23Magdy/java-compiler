#include <iostream>
#include <cassert>
#include "automata/DFA.h"
#include "automata/DFABuilder.h"
#include "automata/DFAMinimizer.h"

void testSubsetConstruction() {
    std::cout << "[Test] Running Subset Construction tests...\n";
    // TODO: Add unit tests for epsilonClosure, move, and NFA-to-DFA conversion
    std::cout << "  -> Subset construction tests passed (stub).\n";
}

void testDFAMinimization() {
    std::cout << "[Test] Running DFA Minimization tests...\n";
    // TODO: Add unit tests for DFA minimization and state equivalence partition
    std::cout << "  -> DFA minimization tests passed (stub).\n";
}

int main() {
    std::cout << "=== Running DFA Unit Tests ===\n";
    testSubsetConstruction();
    testDFAMinimization();
    std::cout << "All DFA tests completed.\n";
    return 0;
}
