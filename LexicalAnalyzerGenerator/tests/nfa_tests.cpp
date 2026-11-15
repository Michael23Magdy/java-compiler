#include <iostream>
#include <cassert>
#include "regex/RegexParser.h"
#include "automata/NFA.h"
#include "automata/NFAConstructor.h"
#include "automata/NFAMerger.h"

void testThompsonConstruction() {
    using namespace compiler;

    std::cout << "[Test] Running Thompson Construction tests...\n";

    // 1. Literal NFA: a
    NFA literal = NFAConstructor::createLiteralNFA('a');

    assert(literal.size() == 2);
    assert(literal.getStartState() == 0);

    auto literalTransitions = literal.getTransitions(0, 'a');
    assert(literalTransitions.size() == 1);
    assert(literalTransitions[0] == 1);
    assert(literal.getState(1).isAccepting);

    // 2. Epsilon NFA: lambda
    NFA epsilon = NFAConstructor::createEpsilonNFA();

    assert(epsilon.size() == 2);
    assert(epsilon.getStartState() == 0);

    auto epsilonTransitions = epsilon.getTransitions(0, EPSILON);
    assert(epsilonTransitions.size() == 1);
    assert(epsilonTransitions[0] == 1);
    assert(epsilon.getState(1).isAccepting);

    std::cout << "  -> Literal and epsilon NFAs passed.\n";

    // Concatenation: ab
    NFA a = NFAConstructor::createLiteralNFA('a');
    NFA b = NFAConstructor::createLiteralNFA('b');

    NFA ab = NFAConstructor::createConcatNFA(a, b);

    assert(ab.size() == 4);
    assert(ab.getStartState() == 0);

    assert(ab.getTransitions(0, 'a').size() == 1);
    assert(ab.getTransitions(1, EPSILON).size() == 1);
    assert(ab.getTransitions(2, 'b').size() == 1);

    assert(ab.getAcceptStates().size() == 1);
    assert(ab.getAcceptStates()[0] == 3);

    assert(!ab.getState(1).isAccepting);
    assert(ab.getState(3).isAccepting);

    std::cout << "  -> Concatenation passed.\n";

    // Union: a | b
    NFA unionNFA = NFAConstructor::createUnionNFA(a, b);

    assert(unionNFA.size() == 6);
    assert(unionNFA.getAcceptStates().size() == 1);

    StateID unionStart = unionNFA.getStartState();
    StateID unionAccept = unionNFA.getAcceptStates()[0];

    assert(unionNFA.getTransitions(unionStart, EPSILON).size() == 2);
    assert(unionNFA.getState(unionAccept).isAccepting);

    std::cout << "  -> Union passed.\n";


    // Star: a*
    NFA starNFA = NFAConstructor::createStarNFA(a);

    assert(starNFA.size() == 4);
    assert(starNFA.getAcceptStates().size() == 1);

    StateID starStart = starNFA.getStartState();
    StateID starAccept = starNFA.getAcceptStates()[0];

    assert(starNFA.getTransitions(starStart, EPSILON).size() == 2);
    assert(starNFA.getState(starAccept).isAccepting);

    std::cout << "  -> Star passed.\n";


    // Plus: a+
    NFA plusNFA = NFAConstructor::createPlusNFA(a);

    assert(plusNFA.size() == 4);
    assert(plusNFA.getAcceptStates().size() == 1);

    StateID plusStart = plusNFA.getStartState();
    StateID plusAccept = plusNFA.getAcceptStates()[0];

    assert(plusNFA.getTransitions(plusStart, EPSILON).size() == 1);
    assert(plusNFA.getState(plusAccept).isAccepting);

    std::cout << "  -> Plus passed.\n";
}


void testBuildFromAST() {
    using namespace compiler;

    std::cout << "[Test] Running AST to NFA tests...\n";

    RegexParser parser;
    NFAConstructor constructor;

    // Test: a(b|c)*
    auto ast = parser.parse("a(b|c)*");

    NFA nfa = constructor.buildFromAST(ast, "id", 5);

    assert(nfa.getStartState() == 0);
    assert(nfa.getAcceptStates().size() == 1);

    StateID accept = nfa.getAcceptStates()[0];

    assert(nfa.getState(accept).isAccepting);
    assert(nfa.getState(accept).tokenClass == "id");
    assert(nfa.getState(accept).priority == 5);

    std::cout << "  -> AST to NFA passed.\n";
}


void testNFAMerging() {
    std::cout << "[Test] Running NFA Merger tests...\n";
    // TODO: Add unit tests for merging multiple token NFAs
    std::cout << "  -> NFA Merger tests passed (stub).\n";
}

int main() {
    std::cout << "=== Running NFA Unit Tests ===\n";
    testThompsonConstruction();
    testBuildFromAST();
    testNFAMerging();
    std::cout << "All NFA tests completed.\n";
    return 0;
}
