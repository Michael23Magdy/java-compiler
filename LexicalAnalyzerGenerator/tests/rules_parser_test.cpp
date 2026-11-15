
#include "input/RulesParser.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace compiler;

int main() {
    std::vector<std::string> lines = {
        "letter = a-z | A-Z",
        "digit = 0 - 9",
        "id: letter (letter|digit)*",
        "digits = digit+",
        "{boolean int float}",
        "num: digit+ | digit+ . digits ( \\L | E digits)",
        "relop: \\=\\= | !\\= | > | >\\= | < | <\\=",
        "assign: =",
        "{ if else while }",
        "[; , \\( \\) { }]",
        "addop: \\+ | \\-",
        "mulop: \\* | /"
    };

    RulesParser parser;
    std::vector<Rule> rules = parser.parse(lines);

    // 1. Check regular definitions.
    const auto& definitions = parser.getDefinitions();

    assert(definitions.size() == 3);
    assert(definitions.count("letter") == 1);
    assert(definitions.count("digit") == 1);
    assert(definitions.count("digits") == 1);

    std::cout << "Regular definitions: PASSED\n";

    // Helper to find a rule by its name.
    auto findRule = [&rules](const std::string& name) -> const Rule* {
        for (const Rule& rule : rules) {
            if (rule.name == name) {
                return &rule;
            }
        }
        return nullptr;
    };

    // 2. Check regular expressions.
    assert(findRule("id") != nullptr);
    assert(findRule("num") != nullptr);
    assert(findRule("relop") != nullptr);
    assert(findRule("assign") != nullptr);
    assert(findRule("addop") != nullptr);
    assert(findRule("mulop") != nullptr);

    assert(findRule("id")->type == RuleType::REGULAR_EXPRESSION);
    assert(findRule("relop")->type == RuleType::REGULAR_EXPRESSION);
    assert(findRule("assign")->type == RuleType::REGULAR_EXPRESSION);

    // Make sure assign was not mistaken for a definition.
    assert(findRule("assign")->expression == "=");

    std::cout << "Regular expressions: PASSED\n";

    // 3. Check keywords.
    std::vector<std::string> keywords = {
        "boolean", "int", "float",
        "if", "else", "while"
    };

    for (const std::string& keyword : keywords) {
        const Rule* rule = findRule(keyword);

        assert(rule != nullptr);
        assert(rule->type == RuleType::KEYWORD);
        assert(rule->expression == keyword);
    }

    std::cout << "Keywords: PASSED\n";

    // 4. Check punctuations.
    std::vector<std::string> punctuations = {
        ";", ",", "(", ")", "{", "}"
    };

    for (const std::string& punctuation : punctuations) {
        const Rule* rule = findRule(punctuation);

        assert(rule != nullptr);
        assert(rule->type == RuleType::PUNCTUATION);
    }

    // Escaped parentheses must retain their backslashes.
    assert(findRule("(")->expression == "\\(");
    assert(findRule(")")->expression == "\\)");

    std::cout << "Punctuations: PASSED\n";

    // 5. Check total counts.
    assert(definitions.size() == 3);
    assert(rules.size() == 18);

    std::cout << "Total definitions: " << definitions.size() << '\n';
    std::cout << "Total token rules: " << rules.size() << '\n';
    assert(findRule("id")->expression.find("(a-z | A-Z)") != std::string::npos);
    assert(findRule("id")->expression.find("(0 - 9)") != std::string::npos);
    std::cout << "\nAll RulesParser tests passed!\n";
    return 0;
}
