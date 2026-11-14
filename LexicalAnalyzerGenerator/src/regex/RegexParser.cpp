#include "regex/RegexParser.h"
#include "regex/RegexUtils.h"

namespace compiler {

std::string RegexParser::preprocess(const std::string& regex) {
    // TODO: Preprocess regular expression:
    // 1. Expand character ranges (e.g., a-z -> (a|b|...|z))
    // 2. Insert explicit concatenation characters where appropriate
    // 3. Handle escaped symbols and lambda (\L)
    return regex;
}

std::shared_ptr<RegexNode> RegexParser::parse(const std::string& regex) {
    // TODO: Parse the regular expression string into an AST
    // Hint: You can preprocess -> convert to postfix -> construct AST from postfix stack
    return std::make_shared<EpsilonNode>();
}

} // namespace compiler
