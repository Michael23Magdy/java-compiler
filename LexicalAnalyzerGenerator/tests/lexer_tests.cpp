#include <iostream>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include "input/RulesParser.h"
#include "regex/RegexParser.h"
#include "automata/NFAConstructor.h"
#include "automata/NFAMerger.h"
#include "automata/DFABuilder.h"
#include "automata/DFAMinimizer.h"
#include "generator/TransitionTable.h"
#include "lexer/Lexer.h"
#include "lexer/SymbolTable.h"

using namespace compiler;

static TransitionTable buildTable(const std::vector<std::string>& ruleLines) {
    RulesParser parser;
    auto rules = parser.parse(ruleLines);
    RegexParser rp;
    NFAConstructor nc;
    std::vector<NFA> nfas;
    for (const auto& r : rules) nfas.push_back(nc.buildFromAST(rp.parse(r.expression), r.name, r.priority));
    NFA nfa = NFAMerger().merge(nfas);
    DFA min = DFAMinimizer().minimize(DFABuilder().build(nfa));
    return TransitionTable::fromDFA(min);
}

static const std::vector<std::string> LAB_RULES = {
    "letter = a-z | A-Z", "digit = 0 - 9", "id: letter (letter|digit)*", "digits = digit+",
    "{boolean int float}", "num: digit+ | digit+ . digits ( \\L | E digits)",
    "relop: \\=\\= | !\\= | > | >\\= | < | <\\=", "assign: =", "{ if else while }",
    "[; , \\( \\) { }]", "addop: \\+ | \\-", "mulop: \\* | /"};

static std::string join(const std::vector<Token>& tokens) {
    std::string out;
    for (const auto& t : tokens) out += t.tokenClass + ":" + t.value + " ";
    return out;
}

void testMaximalMunch() {
    std::cout << "[Test] Running Maximal Munch / Longest Match tests...\n";
    TransitionTable table = buildTable(LAB_RULES);

    Lexer a(table, "a<=b");
    assert(join(a.tokenizeAll()) == "id:a relop:<= id:b ");

    Lexer b(table, "a===b");
    assert(join(b.tokenizeAll()) == "id:a relop:== assign:= id:b ");

    Lexer c(table, "1.5E3 1.5E 12abc");
    assert(join(c.tokenizeAll()) == "num:1.5E3 num:1.5 id:E num:12 id:abc ");

    Lexer d(table, "int integer iff if");
    assert(join(d.tokenizeAll()) == "int:int id:integer id:iff if:if ");

    TransitionTable t2 = buildTable({"x: a | ab | abcd"});
    Lexer e(t2, "abcd abc");
    auto toks = e.tokenizeAll();
    assert(join(toks) == "x:abcd x:ab ");
    assert(e.getErrorHandler().getErrors().size() == 1);

    TransitionTable t3 = buildTable({"r1: ab", "r2: a*b"});
    Lexer f(t3, "ab aab");
    assert(join(f.tokenizeAll()) == "r1:ab r2:aab ");
    std::cout << "  -> Maximal Munch tests passed.\n";
}

void testGetNextToken() {
    std::cout << "[Test] Running getNextToken tests...\n";
    TransitionTable table = buildTable(LAB_RULES);
    Lexer lx(table, "x\n  = 10");
    Token t1 = lx.getNextToken();
    assert(t1.tokenClass == "id" && t1.value == "x" && t1.line == 1 && t1.column == 1);
    Token t2 = lx.getNextToken();
    assert(t2.tokenClass == "assign" && t2.line == 2 && t2.column == 3);
    Token t3 = lx.getNextToken();
    assert(t3.tokenClass == "num" && t3.value == "10" && t3.column == 5);
    assert(lx.getNextToken().isEOF());
    assert(lx.getNextToken().isEOF());

    Lexer empty(table, "  \n\t ");
    assert(empty.getNextToken().isEOF());
    assert(empty.tokenizeAll().empty());

    lx.setSource("int y");
    assert(lx.getNextToken().tokenClass == "int");
    std::cout << "  -> getNextToken tests passed.\n";
}

void testSymbolTable() {
    std::cout << "[Test] Running Symbol Table tests...\n";
    SymbolTable table;
    assert(table.insert("x", 1, "int"));
    assert(table.contains("x"));
    assert(!table.insert("x", 2, "int"));
    assert(table.lookup("x") != nullptr && table.lookup("x")->lineDeclared == 1);
    assert(table.lookup("y") == nullptr);

    Lexer lx(buildTable(LAB_RULES), "int a ; a = b ;\nif ( c ) a = 1 ;");
    lx.tokenizeAll();
    const auto& entries = lx.getSymbolTable().getEntries();
    assert(entries.size() == 3);
    assert(entries[0].name == "a" && entries[0].lineDeclared == 1);
    assert(entries[1].name == "b" && entries[2].name == "c" && entries[2].lineDeclared == 2);
    assert(!lx.getSymbolTable().contains("int") && !lx.getSymbolTable().contains("if"));

    TransitionTable t = buildTable({"letter = a-z", "name: letter+", "{if}"});
    Lexer l2(t, "if foo bar");
    l2.setIdentifierClass("name");
    l2.tokenizeAll();
    assert(l2.getSymbolTable().getEntries().size() == 2);
    Lexer l3(t, "foo");
    l3.setIdentifierClass("");
    l3.tokenizeAll();
    assert(l3.getSymbolTable().getEntries().empty());
    std::cout << "  -> Symbol Table tests passed.\n";
}

void testErrorRecovery() {
    std::cout << "[Test] Running Error Recovery tests...\n";
    TransitionTable table = buildTable(LAB_RULES);
    Lexer lx(table, "int a @ b;\n  ? !c");
    auto toks = lx.tokenizeAll();
    assert(join(toks) == "int:int id:a id:b ;:; id:c ");
    const auto& errs = lx.getErrorHandler().getErrors();
    assert(errs.size() == 3);
    assert(errs[0].invalidChar == '@' && errs[0].line == 1 && errs[0].column == 7);
    assert(errs[1].invalidChar == '?' && errs[1].line == 2 && errs[1].column == 3);
    assert(errs[2].invalidChar == '!' && errs[2].line == 2 && errs[2].column == 5);

    std::ostringstream os;
    lx.getErrorHandler().printErrors(os);
    assert(os.str().find("line 1, column 7") != std::string::npos);

    Lexer bin(table, std::string("a\xc3\xa9\x01 b", 6));
    assert(join(bin.tokenizeAll()) == "id:a id:b ");
    assert(bin.getErrorHandler().getErrors().size() == 3);

    TransitionTable t = buildTable({"t: a*"});
    Lexer nl(t, "aab");
    assert(join(nl.tokenizeAll()) == "t:aa ");
    assert(nl.getErrorHandler().getErrors().size() == 1);
    std::cout << "  -> Error recovery tests passed.\n";
}

int main() {
    std::cout << "=== Running Lexer Unit Tests ===\n";
    testMaximalMunch();
    testGetNextToken();
    testSymbolTable();
    testErrorRecovery();
    std::cout << "All Lexer tests completed.\n";
    return 0;
}
