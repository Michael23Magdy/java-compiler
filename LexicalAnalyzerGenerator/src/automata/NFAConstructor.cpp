#include "automata/NFAConstructor.h"

#include <functional>
#include <stdexcept>

namespace compiler {

NFA NFAConstructor::createLiteralNFA(char ch) {

    NFA nfa;

    StateID start = nfa.addState();
    nfa.setStartState(start);

    StateID accept = nfa.addState();
    nfa.setAccepting(accept, true);

    nfa.addTransition(start, accept, ch);

    return nfa;
}

NFA NFAConstructor::createEpsilonNFA() {

    NFA nfa;

    StateID start = nfa.addState();
    nfa.setStartState(start);

    StateID accept = nfa.addState();
    nfa.setAccepting(accept, true);

    nfa.addTransition(start, accept, EPSILON);

    return nfa;
}

NFA NFAConstructor::createConcatNFA(const NFA& first, const NFA& second) {

    NFA result;
    std::vector<StateID> firstMap;
    std::vector<StateID> secondMap;

    StateID firstStart = result.append(first, firstMap);
    StateID secondStart = result.append(second, secondMap);

    result.setStartState(firstStart);

    for (StateID accept : first.getAcceptStates()) {
        result.addTransition(firstMap[accept], secondStart, EPSILON);

        result.setAccepting(firstMap[accept], false);
    }

    std::vector<StateID> acceptingStates;

    for (StateID accept : second.getAcceptStates()) {
        acceptingStates.push_back(secondMap[accept]);
    }

    result.setAcceptStates(acceptingStates);

    return result;
}

NFA NFAConstructor::createUnionNFA(const NFA& first, const NFA& second) {

    NFA result;

    StateID start = result.addState();
    result.setStartState(start);

    std::vector<StateID> firstMap;
    std::vector<StateID> secondMap;

    StateID firstStart = result.append(first, firstMap);
    StateID secondStart = result.append(second, secondMap);

    result.addTransition(start, firstStart, EPSILON);
    result.addTransition(start, secondStart, EPSILON);

    StateID accept = result.addState();
    result.setAccepting(accept, true);

    for (StateID state : first.getAcceptStates()) {
        result.addTransition(firstMap[state], accept, EPSILON);
        result.setAccepting(firstMap[state], false);
    }

    for (StateID state : second.getAcceptStates()) {
        result.addTransition(secondMap[state], accept, EPSILON);
        result.setAccepting(secondMap[state], false);
    }

    result.setAcceptStates({accept});

    return result;
}

NFA NFAConstructor::createStarNFA(const NFA& base) {

    NFA result;

    StateID start = result.addState();
    result.setStartState(start);

    std::vector<StateID> stateMap;
    StateID baseStart = result.append(base, stateMap);

    StateID accept = result.addState();
    result.setAccepting(accept, true);

    result.addTransition(start, baseStart, EPSILON);

    result.addTransition(start, accept, EPSILON);

    for (StateID state : base.getAcceptStates()) {
        result.addTransition(stateMap[state], baseStart, EPSILON);
        result.addTransition(stateMap[state], accept, EPSILON);
        result.setAccepting(stateMap[state], false);
    }

    result.setAcceptStates({accept});

    return result;
}

NFA NFAConstructor::createPlusNFA(const NFA& base) {

    NFA result;

    StateID start = result.addState();
    result.setStartState(start);

    std::vector<StateID> stateMap;
    StateID baseStart = result.append(base, stateMap);

    StateID accept = result.addState();
    result.setAccepting(accept, true);

    result.addTransition(start, baseStart, EPSILON);

    for (StateID state : base.getAcceptStates()) {
        result.addTransition(stateMap[state], baseStart, EPSILON);
        result.addTransition(stateMap[state], accept, EPSILON);
        result.setAccepting(stateMap[state], false);
    }

    result.setAcceptStates({accept});

    return result;
}

NFA NFAConstructor::buildFromAST(const std::shared_ptr<RegexNode>& root,
                                 const std::string& tokenClass,
                                 TokenPriority priority) {
    if (!root) {
        throw std::invalid_argument("Cannot build NFA from a null AST");
    }

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

    NFA result = build(root);

    for (StateID state : result.getAcceptStates()) {
        result.getState(state).tokenClass = tokenClass;
        result.getState(state).priority = priority;
    }

    return result;
}

}
