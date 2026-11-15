#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
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

using namespace compiler;

int main(int argc, char** argv) {
    if (argc < 3) { std::cerr << "usage: dump_tokens rules input [table_out]\n"; return 2; }
    try {
        RulesReader reader;
        auto lines = reader.readLines(argv[1]);
        RulesParser parser;
        auto rules = parser.parse(lines);
        RegexParser rp;
        NFAConstructor nc;
        std::vector<NFA> nfas;
        for (auto& r : rules) nfas.push_back(nc.buildFromAST(rp.parse(r.expression), r.name, r.priority));
        NFAMerger merger;
        NFA nfa = merger.merge(nfas);
        DFA dfa = DFABuilder().build(nfa);
        DFA min = DFAMinimizer().minimize(dfa);
        TransitionTable table = TransitionTable::fromDFA(min);
        if (argc >= 4) TableExporter::exportToFile(table, argv[3]);

        std::ifstream in(argv[2], std::ios::binary);
        if (!in) { std::cerr << "cannot open input\n"; return 3; }
        std::stringstream ss; ss << in.rdbuf();
        Lexer lexer(table, ss.str());
        auto tokens = lexer.tokenizeAll();
        for (auto& t : tokens) std::cout << t.tokenClass << " " << t.value << "\n";
        for (auto& e : lexer.getErrorHandler().getErrors()) {
            unsigned char c = e.invalidChar;
            std::cout << "!ERR ";
            if (c > 32 && c < 127) std::cout << (char)c; else std::cout << "0x" << std::hex << (int)c << std::dec;
            std::cout << " " << e.line << ":" << e.column << "\n";
        }
        std::cout << "--symbols--\n";
        for (auto& s : lexer.getSymbolTable().getEntries()) std::cout << s.name << " " << s.lineDeclared << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "generator error: " << ex.what() << "\n";
        return 2;
    }
    return 0;
}
