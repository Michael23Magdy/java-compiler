#include "generator/TableExporter.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace compiler {

std::string TableExporter::formatSymbol(Symbol s) {
    if (s == ' ') return "' '";
    if (s == '\t') return "\\t";
    if (s == '\n') return "\\n";
    return std::string(1, s);
}

bool TableExporter::exportToFile(const TransitionTable& table, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        std::cerr << "[TableExporter] Error: Unable to open file: " << filepath << std::endl;
        return false;
    }
    exportToStream(table, out);
    return true;
}

void TableExporter::exportToStream(const TransitionTable& table, std::ostream& out) {
    // TODO: Format and write the transition table nicely to the stream
    // Should include:
    // 1. Header row showing all input alphabet symbols
    // 2. Each row showing: State ID, target state for each symbol, accept status, token class
    out << "=== DFA Transition Table ===" << "\n";
    out << "Start State: " << table.getStartState() << "\n";
    out << "Total States: " << table.getStateCount() << "\n\n";

    // Starter table structure
    out << std::setw(8) << "State";
    for (Symbol s : table.getAlphabet()) {
        out << std::setw(6) << formatSymbol(s);
    }
    out << std::setw(12) << "Accepting" << std::setw(15) << "TokenClass\n";
    out << std::string(40, '-') << "\n";
}

} // namespace compiler
