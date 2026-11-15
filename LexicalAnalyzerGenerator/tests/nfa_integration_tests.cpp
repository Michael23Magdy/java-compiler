#include <iostream>
#include <cassert>
#include <vector>

#include "input/RulesReader.h"
#include "input/RulesParser.h"
#include "regex/RegexParser.h"
#include "automata/NFAConstructor.h"
#include "automata/NFAMerger.h"

using namespace compiler;

void testNFAIntegration() {
    std::cout << "=== Running NFA Integration Test ===\n";

    std::cout << "[Test] Reading lexical rules...\n";

    RulesReader reader;
    auto lines = reader.readLines("input/lexical_rules.txt");

    assert(!lines.empty());

    std::cout << "[Test] Parsing lexical rules...\n";

    RulesParser rulesParser;
    auto rules = rulesParser.parse(lines);

    assert(!rules.empty());

    std::cout << "  -> Parsed " << rules.size()
              << " token specifications.\n";

    std::cout << "[Test] Constructing individual NFAs...\n";

    RegexParser regexParser;
    NFAConstructor nfaConstructor;

    std::vector<NFA> tokenNfas;

    for (const auto& rule : rules) {
        auto ast = regexParser.parse(rule.expression);

        assert(ast != nullptr);

        NFA nfa = nfaConstructor.buildFromAST(
            ast,
            rule.name,
            rule.priority
        );

        assert(!nfa.getAcceptStates().empty());

        for (StateID accept : nfa.getAcceptStates()) {
            const auto& state = nfa.getState(accept);

            assert(state.isAccepting);
            assert(state.tokenClass == rule.name);
            assert(state.priority == rule.priority);
        }

        tokenNfas.push_back(nfa);
    }

    assert(tokenNfas.size() == rules.size());

    std::cout << "  -> Built " << tokenNfas.size()
              << " individual NFAs.\n";

    std::cout << "[Test] Merging individual NFAs...\n";

    NFAMerger merger;
    NFA combinedNFA = merger.merge(tokenNfas);

    std::cout << "[Test] Validating combined NFA...\n";

    size_t expectedStates = 1;
    size_t expectedAcceptStates = 0;

    for (const auto& nfa : tokenNfas) {
        expectedStates += nfa.size();
        expectedAcceptStates += nfa.getAcceptStates().size();
    }

    assert(combinedNFA.size() == expectedStates);
    assert(combinedNFA.getAcceptStates().size() == expectedAcceptStates);

    auto startTransitions = combinedNFA.getTransitions(
        combinedNFA.getStartState(),
        EPSILON
    );

    assert(startTransitions.size() == tokenNfas.size());

    std::vector<bool> matched(combinedNFA.getAcceptStates().size(), false);

    for (const auto& nfa : tokenNfas) {
        for (StateID originalAccept : nfa.getAcceptStates()) {
            const auto& originalState = nfa.getState(originalAccept);

            bool found = false;

            for (size_t i = 0;
                 i < combinedNFA.getAcceptStates().size(); ++i) {

                if (matched[i]) {
                    continue;
                }

                StateID combinedAccept =
                    combinedNFA.getAcceptStates()[i];

                const auto& combinedState =
                    combinedNFA.getState(combinedAccept);

                if (combinedState.isAccepting &&
                    combinedState.tokenClass == originalState.tokenClass &&
                    combinedState.priority == originalState.priority) {

                    matched[i] = true;
                    found = true;
                    break;
                }
            }

            assert(found);
        }
    }

    std::cout << "  -> State count passed.\n";
    std::cout << "  -> Accepting state count passed.\n";
    std::cout << "  -> Start transitions passed.\n";
    std::cout << "  -> Token metadata preservation passed.\n";

    std::cout << "\nAll NFA integration tests passed!\n";
}

int main() {
    testNFAIntegration();
    return 0;
}
