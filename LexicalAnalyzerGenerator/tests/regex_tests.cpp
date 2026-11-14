#include <iostream>
#include <cassert>
#include "regex/RegexParser.h"
#include "regex/RegexUtils.h"

void testRegexUtils() {
    std::cout << "[Test] Running RegexUtils tests...\n";
    // TODO: Add unit tests for RegexUtils (range expansion, explicit concat, infix to postfix)
    std::cout << "  -> RegexUtils tests passed (stub).\n";
}

void testRegexParser() {
    std::cout << "[Test] Running RegexParser tests...\n";
    // TODO: Add unit tests for RegexParser AST construction
    compiler::RegexParser parser;
    auto ast = parser.parse("a|b");
    assert(ast != nullptr);
    std::cout << "  -> RegexParser tests passed (stub).\n";
}

int main() {
    std::cout << "=== Running Regex Unit Tests ===\n";
    testRegexUtils();
    testRegexParser();
    std::cout << "All Regex tests completed.\n";
    return 0;
}
