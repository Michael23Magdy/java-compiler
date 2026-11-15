#pragma once

#include "RegexAST.h"
#include <string>
#include <memory>

namespace compiler {

class RegexParser {
public:
    RegexParser() = default;

    std::shared_ptr<RegexNode> parse(const std::string& regex);

    std::string preprocess(const std::string& regex);
};

}
