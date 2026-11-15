#pragma once

#include "common/Types.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

namespace compiler {

class RulesParser {
public:
    RulesParser() = default;

    std::vector<Rule> parse(const std::vector<std::string>& lines);

    const std::map<std::string, std::string>& getDefinitions() const { return definitions_; }

    const std::vector<Rule>& getTokenRules() const { return tokenRules_; }

private:
    std::map<std::string, std::string> definitions_;
    std::vector<Rule> tokenRules_;
    int nextPriority_ = 1;

    void parseRegularDefinition(const std::string& line);
    void parseRegularExpression(const std::string& line);
    void parseKeywords(const std::string& line);
    void parsePunctuations(const std::string& line);

    std::string expandDefinitions(const std::string& expr);
    static std::string trim(const std::string& str);
};

}
