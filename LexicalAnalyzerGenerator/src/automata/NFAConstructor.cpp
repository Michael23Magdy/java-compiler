#include "automata/NFAConstructor.h"

#include <functional>
#include <stdexcept>

namespace compiler {

NFA NFAConstructor::createLiteralNFA(char ch) {
    // Implement Thompson's construction for a single literal character transition
    NFA nfa;

    StateID start = nfa.addState();
    nfa.setStartState(start);

    StateID accept = nfa.addState();
    nfa.setAccepting(accept, true);

    nfa.addTransition(start, accept, ch);

    return nfa;
}

NFA NFAConstructor::createEpsilonNFA() {
    // Implement Thompson's construction for an epsilon (lambda) transition
    NFA nfa;

    StateID start = nfa.addState();
    nfa.setStartState(start);

    StateID accept = nfa.addState();
    nfa.setAccepting(accept, true);

    nfa.addTransition(start, accept, EPSILON);

    return nfa;
}

NFA NFAConstructor::createConcatNFA(const NFA& first, const NFA& second) {
    // Implement Thompson's construction for concatenation (first . second)
    NFA result;
    std::vector<StateID> firstMap;
    std::vector<StateID> secondMap;

    // Append both NFAs.
    StateID firstStart = result.append(first, firstMap);
    StateID secondStart = result.append(second, secondMap);

    // The first NFA's start remains the overall start.
    result.setStartState(firstStart);

    // Connect the first NFA's accepting states to the second NFA.
    for (StateID accept : first.getAcceptStates()) {
        result.addTransition(firstMap[accept], secondStart, EPSILON);

        // These states are no longer accepting.
        result.setAccepting(firstMap[accept], false);
    }

    // Only the second NFA's accepting states remain accepting.
    std::vector<StateID> acceptingStates;

    for (StateID accept : second.getAcceptStates()) {
        acceptingStates.push_back(secondMap[accept]);
    }

    result.setAcceptStates(acceptingStates);

    return result;
}

NFA NFAConstructor::createUnionNFA(const NFA& first, const NFA& second) {
    // Implement Thompson's construction for union/alternation (first | second)
    NFA result;

    // Create a new start state.
    StateID start = result.addState();
    result.setStartState(start);

    // Append both NFAs.
    std::vector<StateID> firstMap;
    std::vector<StateID> secondMap;

    StateID firstStart = result.append(first, firstMap);
    StateID secondStart = result.append(second, secondMap);

    // Connect the new start to both NFAs.
    result.addTransition(start, firstStart, EPSILON);
    result.addTransition(start, secondStart, EPSILON);

    // Create a new accepting state.
    StateID accept = result.addState();
    result.setAccepting(accept, true);

    // Connect both original accepting states to the new accept.
    for (StateID state : first.getAcceptStates()) {
        result.addTransition(firstMap[state], accept, EPSILON);
        result.setAccepting(firstMap[state], false);
    }

    for (StateID state : second.getAcceptStates()) {
        result.addTransition(secondMap[state], accept, EPSILON);
        result.setAccepting(secondMap[state], false);
    }

    // Only the new state should be accepting.
    result.setAcceptStates({accept});

    return result;
}

NFA NFAConstructor::createStarNFA(const NFA& base) {
    // Implement Thompson's construction for Kleene star (base*)
    NFA result;

    // Create a new start state.
    StateID start = result.addState();
    result.setStartState(start);

    // Append the original NFA.
    std::vector<StateID> stateMap;
    StateID baseStart = result.append(base, stateMap);

    // Create a new accepting state.
    StateID accept = result.addState();
    result.setAccepting(accept, true);

    // Connect the new start to the original start.
    result.addTransition(start, baseStart, EPSILON);

    // Allow zero repetitions.
    result.addTransition(start, accept, EPSILON);

    for (StateID state : base.getAcceptStates()) {
        result.addTransition(stateMap[state], baseStart, EPSILON);
        result.addTransition(stateMap[state], accept, EPSILON);
        result.setAccepting(stateMap[state], false);
    }

    // Only the new state should be accepting.
    result.setAcceptStates({accept});

    return result;
}

NFA NFAConstructor::createPlusNFA(const NFA& base) {
    // Implement Thompson's construction for positive closure (base+)
    NFA result;
    
    // Create a new start state.
    StateID start = result.addState();
    result.setStartState(start);

    // Append the original NFA.
    std::vector<StateID> stateMap;
    StateID baseStart = result.append(base, stateMap);

    // Create a new accepting state.
    StateID accept = result.addState();
    result.setAccepting(accept, true);

    // Connect the new start to the original start.
    result.addTransition(start, baseStart, EPSILON);

    // Allow repetition and stopping.
    for (StateID state : base.getAcceptStates()) {
        result.addTransition(stateMap[state], baseStart, EPSILON);
        result.addTransition(stateMap[state], accept, EPSILON);
        result.setAccepting(stateMap[state], false);
    }

    // Only the new state should be accepting.
    result.setAcceptStates({accept});

    return result;
}

NFA NFAConstructor::buildFromAST(const std::shared_ptr<RegexNode>& root,
                                 const std::string& tokenClass,
                                 TokenPriority priority) {
    if (!root) {
        throw std::invalid_argument("Cannot build NFA from a null AST");
    }

    // Recursive function to construct an NFA from an AST node.
    std::function<NFA(const std::shared_ptr<RegexNode>&)> build =
        [&](const std::shared_ptr<RegexNode>& node) -> NFA {

        if (!node) {
            throw std::invalid_argument("Invalid AST: null node");
        }

        switch (node->getType()) {

            case RegexNodeType::EPSILON:
                return createEpsilonNFA();

            case RegexNodeType::LITERAL: {
                auto literal = std::static_pointer_cast<LiteralNode>(node);
                return createLiteralNFA(literal->getCharacter());
            }

            case RegexNodeType::RANGE: {
                auto range = std::static_pointer_cast<RangeNode>(node);

                NFA result = createLiteralNFA(range->getStart());

                for (int ch = static_cast<unsigned char>(range->getStart()) + 1;
                     ch <= static_cast<unsigned char>(range->getEnd());
                     ++ch) {

                    NFA next = createLiteralNFA(static_cast<char>(ch));
                    result = createUnionNFA(result, next);
                }

                return result;
            }

            case RegexNodeType::CONCAT: {
                auto concat = std::static_pointer_cast<ConcatNode>(node);

                NFA left = build(concat->getLeft());
                NFA right = build(concat->getRight());

                return createConcatNFA(left, right);
            }

            case RegexNodeType::UNION: {
                auto unionNode = std::static_pointer_cast<UnionNode>(node);

                NFA left = build(unionNode->getLeft());
                NFA right = build(unionNode->getRight());

                return createUnionNFA(left, right);
            }

            case RegexNodeType::STAR: {
                auto star = std::static_pointer_cast<StarNode>(node);

                NFA child = build(star->getChild());

                return createStarNFA(child);
            }

            case RegexNodeType::PLUS: {
                auto plus = std::static_pointer_cast<PlusNode>(node);

                NFA child = build(plus->getChild());

                return createPlusNFA(child);
            }
        }

        throw std::invalid_argument("Unknown regex node type");
    };

    // Build the complete NFA.
    NFA result = build(root);

    // Assign the token information to the final accepting states.
    for (StateID state : result.getAcceptStates()) {
        result.getState(state).tokenClass = tokenClass;
        result.getState(state).priority = priority;
    }

    return result;
}

} // namespace compiler
