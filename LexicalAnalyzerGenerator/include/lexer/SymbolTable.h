#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace compiler {

struct SymbolEntry {
    std::string name;
    std::string type = "";
    int lineDeclared = 1;
};

class SymbolTable {
public:
    SymbolTable() = default;

    bool insert(const std::string& name, int line = 1, const std::string& type = "");

    bool contains(const std::string& name) const;

    const SymbolEntry* lookup(const std::string& name) const;

    const std::vector<SymbolEntry>& getEntries() const { return order_; }

    void print(std::ostream& out = std::cout) const;

private:
    std::unordered_map<std::string, SymbolEntry> table_;
    std::vector<SymbolEntry> order_;
};

}
