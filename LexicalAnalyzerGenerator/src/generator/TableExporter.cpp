#include "generator/TableExporter.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace compiler {

std::string TableExporter::formatSymbol(Symbol s) {
    if (s == ' ') return "' '";
    if (s == '\t') return "\\t";
    if (s == '\n') return "\\n";
    if (s == '\r') return "\\r";
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
    out << "=== DFA Transition Table ===" << "\n";
    out << "Start State: " << table.getStartState() << "\n";
    out << "Total States: " << table.getStateCount() << "\n\n";

    out << std::setw(8) << "State";
    for (Symbol sym : table.getAlphabet()) {
        out << std::setw(6) << formatSymbol(sym);
    }
    out << std::setw(12) << "Accepting" << "  " << "TokenClass\n";
    out << std::string(8 + 6 * table.getAlphabet().size() + 26, '-') << "\n";

    for (size_t s = 0; s < table.getStateCount(); s++) {
        StateID id = static_cast<StateID>(s);
        out << std::setw(8) << id;
        for (Symbol sym : table.getAlphabet()) {
            StateID next = table.getNextState(id, sym);
            if (next == INVALID_STATE) out << std::setw(6) << "-";
            else out << std::setw(6) << next;
        }
        out << std::setw(12) << (table.isAccepting(id) ? "yes" : "no") << "  "
            << (table.isAccepting(id) ? table.getTokenClass(id) : "") << "\n";
    }
}

}
