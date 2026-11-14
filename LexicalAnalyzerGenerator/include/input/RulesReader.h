#pragma once

#include <string>
#include <vector>

namespace compiler {

/**
 * @brief Reads lexical rules from an input text file.
 */
class RulesReader {
public:
    RulesReader() = default;

    /**
     * @brief Reads non-empty, trimmed lines from the given file path.
     * @param filepath Path to the lexical rules file.
     * @return Vector of raw rule lines.
     */
    std::vector<std::string> readLines(const std::string& filepath);

private:
    static std::string trim(const std::string& str);
};

} // namespace compiler
