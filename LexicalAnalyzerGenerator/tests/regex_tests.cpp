#include <iostream>
#include <cassert>
#include "regex/RegexParser.h"
#include "regex/RegexUtils.h"

void testRegexUtils() {
    using namespace compiler;

    std::cout << "[Test] Running RegexUtils tests...\n";

    assert(RegexUtils::expandRanges("a-z") ==
           "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)");

    assert(RegexUtils::expandRanges("0 - 2") == "(0|1|2)");

    assert(RegexUtils::expandRanges("\\+ | \\-") == "\\+|\\-");

    std::cout << "  -> Range expansion passed.\n";

    assert(RegexUtils::insertExplicitConcat("ab") ==
           std::string("a") + EXPLICIT_CONCAT + "b");

    assert(RegexUtils::insertExplicitConcat("a(b|c)*") ==
           std::string("a") + EXPLICIT_CONCAT + "(b|c)*");

    assert(RegexUtils::insertExplicitConcat("a*b") ==
           std::string("a*") + EXPLICIT_CONCAT + "b");

    assert(RegexUtils::insertExplicitConcat("a\\+b") ==
           std::string("a") + EXPLICIT_CONCAT + "\\+" +
           EXPLICIT_CONCAT + "b");

    std::cout << "  -> Explicit concatenation passed.\n";

    std::string concat = std::string(1, EXPLICIT_CONCAT);

    assert(RegexUtils::infixToPostfix(
               std::string("a") + EXPLICIT_CONCAT + "b|c") ==
           "ab" + concat + "c|");

    assert(RegexUtils::infixToPostfix(
               std::string("a") + EXPLICIT_CONCAT + "(b|c)*") ==
           "abc|*" + concat);

    std::cout << "  -> Infix to postfix passed.\n";
}

void testRegexParser() {
    using namespace compiler;

    std::cout << "[Test] Running RegexParser tests...\n";

    RegexParser parser;

    auto ast = parser.parse("a|b");
    assert(ast->getType() == RegexNodeType::UNION);

    ast = parser.parse("ab");
    assert(ast->getType() == RegexNodeType::CONCAT);

    ast = parser.parse("a*");
    assert(ast->getType() == RegexNodeType::STAR);

    ast = parser.parse("a+");
    assert(ast->getType() == RegexNodeType::PLUS);

    ast = parser.parse("\\L");
    assert(ast->getType() == RegexNodeType::EPSILON);

    ast = parser.parse("\\+");
    assert(ast->getType() == RegexNodeType::LITERAL);

    auto literal = std::dynamic_pointer_cast<LiteralNode>(ast);
    assert(literal != nullptr);
    assert(literal->getCharacter() == '+');

    ast = parser.parse("a(b|c)*");
    assert(ast->getType() == RegexNodeType::CONCAT);

    std::cout << "  -> RegexParser tests passed.\n";
}

int main() {
    std::cout << "=== Running Regex Unit Tests ===\n";
    testRegexUtils();
    testRegexParser();
    std::cout << "All Regex tests completed.\n";
    return 0;
}
