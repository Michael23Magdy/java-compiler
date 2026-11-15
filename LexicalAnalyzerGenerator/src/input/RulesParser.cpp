#include "input/RulesParser.h"

namespace compiler {

std::vector<Rule> RulesParser::parse(const std::vector<std::string>& lines) {
    definitions_.clear();
    tokenRules_.clear();
    nextPriority_ = 0;

    // Iterate over each line and categorize:
    // 1. Keywords: { ... } -> parseKeywords()
    // 2. Punctuations: [ ... ] -> parsePunctuations()
    // 3. Regular definitions: LHS = RHS -> parseRegularDefinition()
    // 4. Regular expressions: LHS: RHS -> parseRegularExpression()

    for (const std::string& line : lines) {
        if (line.empty()) {
            continue;
        }

        size_t equalPos = line.find('=');
        size_t colonPos = line.find(':');

        if (line.front() == '{') {
            parseKeywords(line);
        }
        else if (line.front() == '[') {
            parsePunctuations(line);
        }
        else if (colonPos != std::string::npos &&
                (equalPos == std::string::npos || colonPos < equalPos)) {
            parseRegularExpression(line);
        }
        else if (equalPos != std::string::npos) {
            parseRegularDefinition(line);
        }
        else {
            // We should report a malformed rule.
            std::cerr << "Error: Malformed lexical rule: " << line << '\n';
            continue;
        }
    }

    return tokenRules_;
}

void RulesParser::parseKeywords(const std::string& line) {
    // Extract keywords from `{ kw1 kw2 ... }` and add each as a Rule with high priority
    size_t start = line.find('{');
    size_t end = line.find('}');

    if (start == std::string::npos ||
        end == std::string::npos ||
        start >= end) {
        return;
    }

    std::string inside = line.substr(
        start + 1,
        end - start - 1
    );

    std::istringstream iss(inside);
    std::string keyword;

    while (iss >> keyword) {
        Rule rule;

        rule.name = keyword;
        rule.expression = keyword;
        rule.type = RuleType::KEYWORD;
        rule.priority = -1;

        tokenRules_.push_back(rule);
    }
   
}

void RulesParser::parsePunctuations(const std::string& line) {
    // Extract punctuation symbols from `[ p1 p2 ... ]` and add each as a Rule
    size_t start = line.find('[');
    size_t end = line.find(']');

    if (start == std::string::npos ||
        end == std::string::npos ||
        start >= end) {
        return;
    }

    std::string inside = line.substr(
        start + 1,
        end - start - 1
    );

    for (size_t i = 0; i < inside.size();) {

        // Skip whitespace.
        if (std::isspace(
                static_cast<unsigned char>(inside[i])
            )) {
            ++i;
            continue;
        }

        std::string expression;

        // Preserve escaped punctuation such as \( and \).
        if (inside[i] == '\\') {
            expression += inside[i++];

            if (i < inside.size()) {
                expression += inside[i++];
            }
        }
        else {
            expression += inside[i++];
        }

        Rule rule;

        // The token class is the literal punctuation.
        rule.name = (expression.size() == 2 &&
                     expression[0] == '\\')
                        ? expression.substr(1)
                        : expression;

        rule.expression = expression;
        rule.type = RuleType::PUNCTUATION;
        rule.priority = nextPriority_++;

        tokenRules_.push_back(rule);
    }
}

void RulesParser::parseRegularDefinition(const std::string& line) {
    // Parse `LHS = RHS`, expand any prior definitions in RHS, and store in definitions_
     size_t equalPos = line.find('=');

    if (equalPos == std::string::npos) {
        return;
    }

    std::string name = line.substr(0, equalPos);
    std::string expression = line.substr(equalPos + 1);

    name = trim(name);
    expression = trim(expression);

    expression = expandDefinitions(expression);

    definitions_[name] = expression;
}

void RulesParser::parseRegularExpression(const std::string& line) {
    // Parse `LHS: RHS`, expand definitions in RHS, and add to tokenRules_ with nextPriority_++
    size_t colonPos = line.find(':');

    if (colonPos == std::string::npos) {
        return;
    }

    std::string name = line.substr(0, colonPos);
    std::string expression = line.substr(colonPos + 1);

    name = trim(name);
    expression = trim(expression);

    expression = expandDefinitions(expression);

    Rule rule;
    rule.name = name;
    rule.expression = expression;
    rule.type = RuleType::REGULAR_EXPRESSION;
    rule.priority = nextPriority_++;

    tokenRules_.push_back(rule);
}

std::string RulesParser::expandDefinitions(const std::string& expr) {
    // Replace identifiers in expr with their expanded definitions from definitions_
    std::string result;

    for (size_t i = 0; i < expr.size();) {

        // Detect an identifier.
        if (std::isalpha(
                static_cast<unsigned char>(expr[i])
            ) || expr[i] == '_') {

            size_t j = i + 1;

            while (j < expr.size() &&
                   (std::isalnum(
                       static_cast<unsigned char>(expr[j])
                   ) || expr[j] == '_')) {
                ++j;
            }

            std::string word = expr.substr(i, j - i);

            auto it = definitions_.find(word);

            if (it != definitions_.end()) {
                result += "(" + it->second + ")";
            }
            else {
                result += word;
            }

            i = j;
        }
        else {
            result += expr[i];
            ++i;
        }
    }

    return result;
}

std::string RulesParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");

    if (first == std::string::npos) {
        return "";
    }

    size_t last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}

} // namespace compiler
