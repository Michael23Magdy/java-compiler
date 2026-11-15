CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -pedantic -Iinclude
BIN_DIR = bin
BUILD_DIR = build

CORE_SRCS = src/input/RulesReader.cpp \
            src/input/RulesParser.cpp \
            src/regex/RegexParser.cpp \
            src/regex/RegexUtils.cpp \
            src/automata/NFA.cpp \
            src/automata/NFAConstructor.cpp \
            src/automata/NFAMerger.cpp \
            src/automata/DFA.cpp \
            src/automata/DFABuilder.cpp \
            src/automata/DFAMinimizer.cpp \
            src/generator/TransitionTable.cpp \
            src/generator/TableExporter.cpp \
            src/lexer/Lexer.cpp \
            src/lexer/SymbolTable.cpp \
            src/lexer/ErrorHandler.cpp

MAIN_SRC = src/main.cpp
MAIN_BIN = $(BIN_DIR)/lexical_analyzer_generator

TEST_SRCS = tests/regex_tests.cpp tests/nfa_tests.cpp tests/dfa_tests.cpp tests/lexer_tests.cpp
TEST_BINS = $(BIN_DIR)/regex_tests $(BIN_DIR)/nfa_tests $(BIN_DIR)/dfa_tests $(BIN_DIR)/lexer_tests

.PHONY: all clean run tests test_regex test_nfa test_dfa test_lexer lab_tests lab_test

all: $(MAIN_BIN) tests

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(MAIN_BIN): $(CORE_SRCS) $(MAIN_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRCS) $(MAIN_SRC) -o $@

tests: $(TEST_BINS)

$(BIN_DIR)/regex_tests: tests/regex_tests.cpp $(CORE_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) tests/regex_tests.cpp $(CORE_SRCS) -o $@

$(BIN_DIR)/nfa_tests: tests/nfa_tests.cpp $(CORE_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) tests/nfa_tests.cpp $(CORE_SRCS) -o $@

$(BIN_DIR)/dfa_tests: tests/dfa_tests.cpp $(CORE_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) tests/dfa_tests.cpp $(CORE_SRCS) -o $@

$(BIN_DIR)/lexer_tests: tests/lexer_tests.cpp $(CORE_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) tests/lexer_tests.cpp $(CORE_SRCS) -o $@

run: $(MAIN_BIN)
	./$(MAIN_BIN) input/lexical_rules.txt input/test_program.java output/transition_table.txt output/tokens.txt

test_regex: $(BIN_DIR)/regex_tests
	./$(BIN_DIR)/regex_tests

test_nfa: $(BIN_DIR)/nfa_tests
	./$(BIN_DIR)/nfa_tests

test_dfa: $(BIN_DIR)/dfa_tests
	./$(BIN_DIR)/dfa_tests

test_lexer: $(BIN_DIR)/lexer_tests
	./$(BIN_DIR)/lexer_tests

# Spec-driven lab tests (see tests/lab/run_lab_tests.py)
lab_tests: $(BIN_DIR)/dump_tokens

$(BIN_DIR)/dump_tokens: tests/lab/dump_tokens.cpp $(CORE_SRCS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) tests/lab/dump_tokens.cpp $(CORE_SRCS) -o $@

lab_test: $(BIN_DIR)/dump_tokens $(MAIN_BIN)
	python3 tests/lab/run_lab_tests.py

clean:
	rm -rf $(BIN_DIR)
