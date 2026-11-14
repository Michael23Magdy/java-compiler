#include <iostream>
#include <fstream>
#include <sstream>

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

int main(int argc, char* argv[]) {
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

    // 1. Read lexical rules file
    std::cout << "[Step 1] Reading rules file: " << rulesFile << "\n";
    compiler::RulesReader reader;
    auto lines = reader.readLines(rulesFile);
    std::cout << "         Read " << lines.size() << " non-empty rule lines.\n\n";

    // 2. Parse lexical rules
    std::cout << "[Step 2] Parsing lexical rules...\n";
    compiler::RulesParser rulesParser;
    auto rules = rulesParser.parse(lines);
    std::cout << "         Parsed " << rules.size() << " token specifications.\n\n";

    // 3. Build Thompson NFAs for each token rule
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

    // 4. Merge NFAs with new start state
    std::cout << "[Step 4] Merging NFAs into combined NFA...\n";
    compiler::NFAMerger merger;
    compiler::NFA combinedNFA = merger.merge(tokenNfas);
    std::cout << "         Combined NFA created.\n\n";

    // 5. Convert NFA to DFA using Subset Construction
    std::cout << "[Step 5] Converting NFA to DFA (Subset Construction)...\n";
    compiler::DFABuilder dfaBuilder;
    compiler::DFA dfa = dfaBuilder.build(combinedNFA);
    std::cout << "         DFA created with " << dfa.size() << " states.\n\n";

    // 6. Minimize DFA
    std::cout << "[Step 6] Minimizing DFA...\n";
    compiler::DFAMinimizer minimizer;
    compiler::DFA minDfa = minimizer.minimize(dfa);
    std::cout << "         Minimized DFA has " << minDfa.size() << " states.\n\n";

    // 7. Generate and export transition table
    std::cout << "[Step 7] Exporting transition table to: " << tableOutputFile << "\n";
    compiler::TransitionTable table = compiler::TransitionTable::fromDFA(minDfa);
    compiler::TableExporter::exportToFile(table, tableOutputFile);
    std::cout << "         Transition table exported.\n\n";

    // 8. Read test program and run Lexer
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
    auto tokens = lexer.tokenizeAll();

    // 9. Write tokens to output file (one token class per line)
    std::ofstream tokenOut(tokensOutputFile);
    if (tokenOut.is_open()) {
        for (const auto& token : tokens) {
            tokenOut << token.tokenClass << "\n";
        }
        std::cout << "         Recognized " << tokens.size() << " tokens. Written to: " << tokensOutputFile << "\n\n";
    }

    // 10. Print symbol table & errors
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
