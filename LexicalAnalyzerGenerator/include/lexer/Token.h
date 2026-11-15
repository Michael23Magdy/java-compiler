#pragma once

#include <string>

namespace compiler {

struct Token {
    std::string tokenClass;
    std::string value;
    int line = 1;
    int column = 1;

    bool isEOF() const { return tokenClass == "$"; }

    std::string toString() const {
        return tokenClass + " ('" + value + "') at " + std::to_string(line) + ":" + std::to_string(column);
    }
};

}
