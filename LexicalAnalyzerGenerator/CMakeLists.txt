cmake_minimum_required(VERSION 3.16)
project(LexicalAnalyzerGenerator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -pedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files for core library
set(CORE_SOURCES
    src/input/RulesReader.cpp
    src/input/RulesParser.cpp
    src/regex/RegexParser.cpp
    src/regex/RegexUtils.cpp
    src/automata/NFA.cpp
    src/automata/NFAConstructor.cpp
    src/automata/NFAMerger.cpp
    src/automata/DFA.cpp
    src/automata/DFABuilder.cpp
    src/automata/DFAMinimizer.cpp
    src/generator/TransitionTable.cpp
    src/generator/TableExporter.cpp
    src/lexer/Lexer.cpp
    src/lexer/SymbolTable.cpp
    src/lexer/ErrorHandler.cpp
)

# Core library
add_library(LexerCore STATIC ${CORE_SOURCES})

# Main executable
add_executable(lexical_analyzer_generator src/main.cpp)
target_link_libraries(lexical_analyzer_generator PRIVATE LexerCore)

# Test executables
enable_testing()

add_executable(regex_tests tests/regex_tests.cpp)
target_link_libraries(regex_tests PRIVATE LexerCore)
add_test(NAME RegexTests COMMAND regex_tests)

add_executable(nfa_tests tests/nfa_tests.cpp)
target_link_libraries(nfa_tests PRIVATE LexerCore)
add_test(NAME NFATests COMMAND nfa_tests)

add_executable(dfa_tests tests/dfa_tests.cpp)
target_link_libraries(dfa_tests PRIVATE LexerCore)
add_test(NAME DFATests COMMAND dfa_tests)

add_executable(lexer_tests tests/lexer_tests.cpp)
target_link_libraries(lexer_tests PRIVATE LexerCore)
add_test(NAME LexerTests COMMAND lexer_tests)
