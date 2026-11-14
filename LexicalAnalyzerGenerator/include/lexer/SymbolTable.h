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

/**
 * @brief Symbol table to store and manage identifiers encountered during lexical analysis.
 */
class SymbolTable {
public:
    SymbolTable() = default;

    /**
     * @brief Inserts an identifier into the symbol table if not already present.
     * @return true if inserted, false if already present.
     */
    bool insert(const std::string& name, int line = 1, const std::string& type = "");

    /**
     * @brief Checks if identifier exists in table.
     */
    bool contains(const std::string& name) const;

    /**
     * @brief Retrieves entry for identifier.
     */
    const SymbolEntry* lookup(const std::string& name) const;

    /**
     * @brief Returns all entries in insertion order.
     */
    const std::vector<SymbolEntry>& getEntries() const { return order_; }

    /**
     * @brief Prints symbol table contents to standard output or stream.
     */
    void print(std::ostream& out = std::cout) const;

private:
    std::unordered_map<std::string, SymbolEntry> table_;
    std::vector<SymbolEntry> order_;
};

} // namespace compiler
