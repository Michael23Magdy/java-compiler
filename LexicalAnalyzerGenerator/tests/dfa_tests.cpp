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
    using namespace compiler;
    DFAMinimizer minimizer;

    // Equivalent accepting states 1 and 2 (same token) must merge: 4 -> 3 states.
    DFA d;
    for (int i = 0; i < 4; i++) d.addState();
    d.setStartState(0);
    d.setAlphabet({'a', 'b'});
    d.addTransition(0, 1, 'a');
    d.addTransition(0, 2, 'b');
    for (StateID s : {1, 2}) { d.addTransition(s, 3, 'a'); d.addTransition(s, 3, 'b'); }
    d.addTransition(3, 3, 'a');
    d.addTransition(3, 3, 'b');
    d.setAccepting(1, true, "X", 0);
    d.setAccepting(2, true, "X", 0);
    DFA m = minimizer.minimize(d);
    assert(m.size() == 3);

    // Same structure but different token classes: must NOT merge.
    d.setAccepting(2, true, "Y", 0);
    assert(minimizer.minimize(d).size() == 4);
    std::cout << "  -> DFA minimization tests passed.\n";
}

int main() {
    std::cout << "=== Running DFA Unit Tests ===\n";
    testSubsetConstruction();
    testDFAMinimization();
    std::cout << "All DFA tests completed.\n";
    return 0;
}
