#include "lexer/SymbolTable.h"
#include <iomanip>

namespace compiler {

bool SymbolTable::insert(const std::string& name, int line, const std::string& type) {

    if (contains(name)) {
        return false;
    }
    SymbolEntry entry{name, type, line};
    table_[name] = entry;
    order_.push_back(entry);
    return true;
}

bool SymbolTable::contains(const std::string& name) const {
    return table_.find(name) != table_.end();
}

const SymbolEntry* SymbolTable::lookup(const std::string& name) const {
    auto it = table_.find(name);
    if (it != table_.end()) {
        return &(it->second);
    }
    return nullptr;
}

void SymbolTable::print(std::ostream& out) const {
    out << "=== Symbol Table ===\n";
    out << std::left << std::setw(20) << "Identifier" << std::setw(15) << "Type" << std::setw(10) << "Line" << "\n";
    out << std::string(45, '-') << "\n";
    for (const auto& entry : order_) {
        out << std::left << std::setw(20) << entry.name
            << std::setw(15) << (entry.type.empty() ? "-" : entry.type)
            << std::setw(10) << entry.lineDeclared << "\n";
    }
}

}
