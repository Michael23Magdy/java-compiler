#pragma once

#include "TransitionTable.h"
#include <string>
#include <ostream>

namespace compiler {

/**
 * @brief Exporter to serialize transition tables into human-readable text and structured formats.
 */
class TableExporter {
public:
    TableExporter() = default;

    /**
     * @brief Exports the transition table to a specified output file.
     */
    static bool exportToFile(const TransitionTable& table, const std::string& filepath);

    /**
     * @brief Writes formatted transition table to an output stream.
     */
    static void exportToStream(const TransitionTable& table, std::ostream& out);

private:
    static std::string formatSymbol(Symbol s);
};

} // namespace compiler
