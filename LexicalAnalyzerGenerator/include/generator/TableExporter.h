#pragma once

#include "TransitionTable.h"
#include <string>
#include <ostream>

namespace compiler {

class TableExporter {
public:
    TableExporter() = default;

    static bool exportToFile(const TransitionTable& table, const std::string& filepath);

    static void exportToStream(const TransitionTable& table, std::ostream& out);

private:
    static std::string formatSymbol(Symbol s);
};

}
