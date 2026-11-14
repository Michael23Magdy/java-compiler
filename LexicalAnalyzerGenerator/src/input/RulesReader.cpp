#include "input/RulesReader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace compiler {

std::string RulesReader::trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();
    return (start < end) ? std::string(start, end) : std::string();
}

std::vector<std::string> RulesReader::readLines(const std::string& filepath) {
    std::vector<std::string> lines;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[RulesReader] Error: Could not open file: " << filepath << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }
        lines.push_back(trimmed);
    }

    return lines;
}

} // namespace compiler
