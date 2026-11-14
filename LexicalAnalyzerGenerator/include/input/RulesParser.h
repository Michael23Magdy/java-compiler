#pragma once

#include "common/Types.h"
#include <string>
#include <vector>
#include <map>

namespace compiler {

/**
 * @brief Parses lexical rules file content into structured rules.
 * Handles:
 * - Regular definitions: LHS = RHS
 * - Regular expressions: LHS: RHS
 * - Keywords: { keyword1 keyword2 ... }
 * - Punctuations: [ punct1 punct2 ... ]
 * - Definition substitution in expressions
 */
class RulesParser {
public:
    RulesParser() = default;

    /**
     * @brief Parses a list of rule lines from the input file.
     * @param lines Raw lines read from the lexical specification file.
     * @return Vector of processed token rules in precedence order.
     */
    std::vector<Rule> parse(const std::vector<std::string>& lines);

    /**
     * @brief Gets regular definitions map (name -> expanded definition).
     */
    const std::map<std::string, std::string>& getDefinitions() const { return definitions_; }

    /**
     * @brief Gets parsed token rules with definitions substituted.
     */
    const std::vector<Rule>& getTokenRules() const { return tokenRules_; }

private:
    std::map<std::string, std::string> definitions_;
    std::vector<Rule> tokenRules_;
    int nextPriority_ = 0;

    void parseRegularDefinition(const std::string& line);
    void parseRegularExpression(const std::string& line);
    void parseKeywords(const std::string& line);
    void parsePunctuations(const std::string& line);

    std::string expandDefinitions(const std::string& expr);
};

} // namespace compiler
