#include <iostream>
#include <cassert>
#include "automata/DFA.h"
#include "automata/DFABuilder.h"
#include "automata/DFAMinimizer.h"
#include "automata/NFA.h"
#include "generator/TransitionTable.h"
#include "generator/TableExporter.h"
#include <sstream>

void testSubsetConstruction() {
    std::cout << "[Test] Running Subset Construction tests...\n";
    using namespace compiler;
    // NFA for a(a|b)*: 0 -a-> 1, 1 -eps-> 2, 2 -a-> 3, 2 -b-> 3, 3 -eps-> 1, 3 accepting
    NFA nfa;
    for (int i = 0; i < 4; i++) nfa.addState();
    nfa.setStartState(0);
    nfa.addTransition(0, 1, 'a');
    nfa.addTransition(1, 2, EPSILON);
    nfa.addTransition(2, 3, 'a');
    nfa.addTransition(2, 3, 'b');
    nfa.addTransition(3, 1, EPSILON);
    nfa.setAccepting(1, true, "T", 0);
    nfa.setAccepting(3, true, "T", 0);
    DFABuilder b;
    DFA dfa = b.build(nfa);
    assert(dfa.size() >= 2);
    StateID s1 = dfa.getTransition(dfa.getStartState(), 'a');
    assert(s1 != INVALID_STATE && dfa.getState(s1).isAccepting);
    assert(dfa.getTransition(dfa.getStartState(), 'b') == INVALID_STATE);
    std::cout << "  -> Subset construction tests passed.\n";
}

void testTransitionTable() {
    std::cout << "[Test] Running Transition Table / Exporter tests...\n";
    using namespace compiler;
    DFA d;
    d.addState(); d.addState();
    d.setStartState(0);
    d.setAlphabet({'a', 'b'});
    d.addTransition(0, 1, 'a');
    d.addTransition(1, 1, 'b');
    d.setAccepting(1, true, "ID", 3);
    TransitionTable t = TransitionTable::fromDFA(d);
    assert(t.getStateCount() == 2);
    assert(t.getNextState(0, 'a') == 1);
    assert(t.getNextState(0, 'b') == INVALID_STATE);
    assert(t.getNextState(1, 'b') == 1);
    assert(t.getNextState(0, 'z') == INVALID_STATE);
    assert(!t.isAccepting(0) && t.isAccepting(1));
    assert(t.getTokenClass(1) == "ID" && t.getPriority(1) == 3);
    std::ostringstream os;
    TableExporter::exportToStream(t, os);
    assert(os.str().find("ID") != std::string::npos);
    assert(os.str().find("Total States: 2") != std::string::npos);
    std::cout << "  -> Transition table tests passed.\n";
}

void testPipeline5to7() {
    std::cout << "[Test] Running NFA -> DFA -> min DFA -> table pipeline...\n";
    using namespace compiler;
    // Two tokens: "ab" (KW, prio 0) and (a|b)+ (ID, prio 1) as one merged NFA.
    NFA nfa;
    for (int i = 0; i < 6; i++) nfa.addState();
    nfa.setStartState(0);
    nfa.addTransition(0, 1, EPSILON); nfa.addTransition(0, 4, EPSILON);
    nfa.addTransition(1, 2, 'a'); nfa.addTransition(2, 3, 'b');
    nfa.setAccepting(3, true, "KW", 0);
    nfa.addTransition(4, 5, 'a'); nfa.addTransition(4, 5, 'b');
    nfa.addTransition(5, 5, 'a'); nfa.addTransition(5, 5, 'b');
    nfa.setAccepting(5, true, "ID", 1);
    DFABuilder builder; DFAMinimizer minimizer;
    DFA dfa = builder.build(nfa);
    DFA mini = minimizer.minimize(dfa);
    assert(mini.size() <= dfa.size());
    TransitionTable t = TransitionTable::fromDFA(mini);
    StateID st = t.getStartState();
    StateID a = t.getNextState(st, 'a');
    StateID ab = t.getNextState(a, 'b');
    assert(t.getTokenClass(a) == "ID");
    assert(t.getTokenClass(ab) == "KW");      // priority resolves overlap
    assert(t.getTokenClass(t.getNextState(ab, 'a')) == "ID");
    assert(TableExporter::exportToFile(t, "output/pipeline_test_table.txt"));
    std::cout << "  -> Pipeline 5-7 tests passed.\n";
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
    testTransitionTable();
    testPipeline5to7();
    std::cout << "All DFA tests completed.\n";
    return 0;
}
