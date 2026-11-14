#include <iostream>
#include <cassert>
#include "lexer/Lexer.h"
#include "lexer/SymbolTable.h"

void testMaximalMunch() {
    std::cout << "[Test] Running Maximal Munch / Longest Match tests...\n";
    // TODO: Add tests verifying that prefix matching chooses the longest valid match
    std::cout << "  -> Maximal Munch tests passed (stub).\n";
}

void testSymbolTable() {
    std::cout << "[Test] Running Symbol Table tests...\n";
    compiler::SymbolTable table;
    assert(table.insert("x", 1, "int"));
    assert(table.contains("x"));
    assert(!table.insert("x", 2, "int")); // Duplicate insertion
    std::cout << "  -> Symbol Table tests passed.\n";
}

void testErrorRecovery() {
    std::cout << "[Test] Running Error Recovery tests...\n";
    // TODO: Add tests for reporting lexical errors when input characters don't match any rule
    std::cout << "  -> Error recovery tests passed (stub).\n";
}

int main() {
    std::cout << "=== Running Lexer Unit Tests ===\n";
    testMaximalMunch();
    testSymbolTable();
    testErrorRecovery();
    std::cout << "All Lexer tests completed.\n";
    return 0;
}
