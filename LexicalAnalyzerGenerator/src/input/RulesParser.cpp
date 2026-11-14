#include "input/RulesParser.h"

namespace compiler {

std::vector<Rule> RulesParser::parse(const std::vector<std::string>& lines) {
    definitions_.clear();
    tokenRules_.clear();
    nextPriority_ = 0;

    // TODO: Iterate over each line and categorize:
    // 1. Keywords: { ... } -> parseKeywords()
    // 2. Punctuations: [ ... ] -> parsePunctuations()
    // 3. Regular definitions: LHS = RHS -> parseRegularDefinition()
    // 4. Regular expressions: LHS: RHS -> parseRegularExpression()

    return tokenRules_;
}

void RulesParser::parseKeywords(const std::string& line) {
    // TODO: Extract keywords from `{ kw1 kw2 ... }` and add each as a Rule with high priority
}

void RulesParser::parsePunctuations(const std::string& line) {
    // TODO: Extract punctuation symbols from `[ p1 p2 ... ]` and add each as a Rule
}

void RulesParser::parseRegularDefinition(const std::string& line) {
    // TODO: Parse `LHS = RHS`, expand any prior definitions in RHS, and store in definitions_
}

void RulesParser::parseRegularExpression(const std::string& line) {
    // TODO: Parse `LHS: RHS`, expand definitions in RHS, and add to tokenRules_ with nextPriority_++
}

std::string RulesParser::expandDefinitions(const std::string& expr) {
    // TODO: Replace identifiers in expr with their expanded definitions from definitions_
    return expr;
}

} // namespace compiler
