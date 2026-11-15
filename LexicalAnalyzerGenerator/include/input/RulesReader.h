#pragma once

#include <string>
#include <vector>

namespace compiler {

class RulesReader {
public:
    RulesReader() = default;

    std::vector<std::string> readLines(const std::string& filepath);

private:
    static std::string trim(const std::string& str);
};

}
