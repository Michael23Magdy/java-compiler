#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

#include "input/RulesReader.h"
#include "input/RulesParser.h"
#include "regex/RegexParser.h"
#include "automata/NFAConstructor.h"
#include "automata/NFAMerger.h"
#include "automata/DFABuilder.h"
#include "automata/DFAMinimizer.h"
#include "generator/TransitionTable.h"
#include "generator/TableExporter.h"
#include "lexer/Lexer.h"

static std::string findIdentifierClass(const std::vector<compiler::Rule>& rules,
                                       const std::vector<compiler::NFA>& nfas) {
    for (const auto& r : rules) {
        if (r.type == compiler::RuleType::REGULAR_EXPRESSION && r.name == "id") return "id";
    }
    for (const auto& kw : rules) {
        if (kw.type != compiler::RuleType::KEYWORD) continue;
        for (size_t i = 0; i < rules.size(); ++i) {
            if (rules[i].type != compiler::RuleType::REGULAR_EXPRESSION) continue;
            compiler::DFA d = compiler::DFABuilder().build(nfas[i]);
            compiler::StateID st = d.getStartState();
            for (char c : kw.name) {
                st = (st == compiler::INVALID_STATE) ? st : d.getTransition(st, c);
            }
            if (st != compiler::INVALID_STATE && d.getState(st).isAccepting) return rules[i].name;
        }
        break;
    }
    return "id";
}

static int run(int argc, char* argv[]) {
    std::string rulesFile = "input/lexical_rules.txt";
    std::string testProgramFile = "input/test_program.java";
    std::string tableOutputFile = "output/transition_table.txt";
    std::string tokensOutputFile = "output/tokens.txt";

    if (argc >= 2) rulesFile = argv[1];
    if (argc >= 3) testProgramFile = argv[2];
    if (argc >= 4) tableOutputFile = argv[3];
    if (argc >= 5) tokensOutputFile = argv[4];

    std::cout << "========================================\n";
    std::cout << "    Lexical Analyzer Generator (Phase 1)\n";
    std::cout << "========================================\n\n";

    std::cout << "[Step 1] Reading rules file: " << rulesFile << "\n";
    compiler::RulesReader reader;
    auto lines = reader.readLines(rulesFile);
    std::cout << "         Read " << lines.size() << " non-empty rule lines.\n\n";

    std::cout << "[Step 2] Parsing lexical rules...\n";
    compiler::RulesParser rulesParser;
    auto rules = rulesParser.parse(lines);
    if (rules.empty()) {
        std::cerr << "Error: no token specifications found in rules file: " << rulesFile << "\n";
        return 1;
    }
    std::cout << "         Parsed " << rules.size() << " token specifications.\n\n";

    std::cout << "[Step 3] Constructing NFAs for each regular expression...\n";
    compiler::RegexParser regexParser;
    compiler::NFAConstructor nfaConstructor;
    std::vector<compiler::NFA> tokenNfas;

    for (const auto& rule : rules) {
        auto ast = regexParser.parse(rule.expression);
        auto nfa = nfaConstructor.buildFromAST(ast, rule.name, rule.priority);
        tokenNfas.push_back(nfa);
    }
    std::cout << "         Built " << tokenNfas.size() << " individual NFAs.\n\n";

    std::cout << "[Step 4] Merging NFAs into combined NFA...\n";
    compiler::NFAMerger merger;
    compiler::NFA combinedNFA = merger.merge(tokenNfas);
    std::cout << "         Combined NFA created.\n\n";

    std::cout << "[Step 5] Converting NFA to DFA (Subset Construction)...\n";
    compiler::DFABuilder dfaBuilder;
    compiler::DFA dfa = dfaBuilder.build(combinedNFA);
    std::cout << "         DFA created with " << dfa.size() << " states.\n\n";

    std::cout << "[Step 6] Minimizing DFA...\n";
    compiler::DFAMinimizer minimizer;
    compiler::DFA minDfa = minimizer.minimize(dfa);
    std::cout << "         Minimized DFA has " << minDfa.size() << " states.\n\n";

    std::cout << "[Step 7] Exporting transition table to: " << tableOutputFile << "\n";
    compiler::TransitionTable table = compiler::TransitionTable::fromDFA(minDfa);
    compiler::TableExporter::exportToFile(table, tableOutputFile);
    std::cout << "         Transition table exported.\n\n";

    std::cout << "[Step 8] Running Lexer on: " << testProgramFile << "\n";
    std::ifstream testFile(testProgramFile);
    if (!testFile.is_open()) {
        std::cerr << "         Error: Could not open test program: " << testProgramFile << "\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << testFile.rdbuf();
    std::string testCode = buffer.str();

    compiler::Lexer lexer(table, testCode);
    lexer.setIdentifierClass(findIdentifierClass(rules, tokenNfas));
    auto tokens = lexer.tokenizeAll();

    std::ofstream tokenOut(tokensOutputFile);
    if (tokenOut.is_open()) {
        for (const auto& token : tokens) {
            tokenOut << token.tokenClass << "\n";
        }
        std::cout << "         Recognized " << tokens.size() << " tokens. Written to: " << tokensOutputFile << "\n\n";
    }

    std::cout << "[Step 9] Symbol Table:\n";
    lexer.getSymbolTable().print(std::cout);

    if (lexer.getErrorHandler().hasErrors()) {
        std::cout << "\n[Step 10] Lexical Errors:\n";
        lexer.getErrorHandler().printErrors(std::cerr);
    } else {
        std::cout << "\nNo lexical errors encountered.\n";
    }

    std::cout << "\n========================================\n";
    std::cout << "    Lexical Analysis Pipeline Complete\n";
    std::cout << "========================================\n";

    return 0;
}

int main(int argc, char* argv[]) {
    try {
        return run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
