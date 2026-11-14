# Lexical Analyzer Generator (Phase 1)

Automatic Lexical Analyzer Generator tool implemented in C++ (C++17).

This project automatically constructs a lexical analyzer from a regular expression description of token rules. It transforms regular expressions into Non-Deterministic Finite Automata (NFAs), merges them, converts the combined NFA into a Deterministic Finite Automaton (DFA) using subset construction, minimizes the DFA using partition refinement (Hopcroft's-style), emits the minimal DFA transition table, and simulates the DFA on input programs using **Maximal Munch (longest prefix matching)**.

---

## Directory Structure

```
LexicalAnalyzerGenerator/
├── CMakeLists.txt              # CMake build configuration
├── Makefile                    # Alternative direct build with GNU Make / g++
├── README.md                   # Project documentation & implementation roadmap
│
├── input/
│   ├── lexical_rules.txt       # Lexical specification (regular definitions, expressions, keywords, punctuations)
│   └── test_program.java       # Sample input program to tokenize
│
├── output/
│   ├── transition_table.txt    # Exported minimized DFA transition table
│   └── tokens.txt              # Output stream of recognized token classes (one per line)
│
├── include/
│   ├── common/
│   │   ├── Types.h             # Type aliases (StateID, Symbol, TokenPriority) and Rule structs
│   │   └── Constants.h         # Constants (EPSILON/LAMBDA, INVALID_STATE, EXPLICIT_CONCAT)
│   │
│   ├── input/
│   │   ├── RulesReader.h       # File reader for lexical specification
│   │   └── RulesParser.h       # Parser for definitions, regexes, keywords, and punctuations
│   │
│   ├── regex/
│   │   ├── RegexAST.h          # Abstract Syntax Tree nodes for regular expressions
│   │   ├── RegexParser.h       # Infix-to-AST regular expression parser
│   │   └── RegexUtils.h        # Range expansion, explicit concat insertion, Shunting-yard postfix
│   │
│   ├── automata/
│   │   ├── NFA.h               # NFA state and graph data structure
│   │   ├── NFAConstructor.h    # Thompson's construction building blocks
│   │   ├── NFAMerger.h         # Merges individual token NFAs into a combined NFA
│   │   ├── DFA.h               # DFA state and deterministic transition table
│   │   ├── DFABuilder.h        # Subset construction (epsilon-closure & move)
│   │   └── DFAMinimizer.h      # Hopcroft's / partition refinement DFA minimization
│   │
│   ├── generator/
│   │   ├── TransitionTable.h   # 2D table representation of minimized DFA
│   │   └── TableExporter.h     # Table formatting and file exporter
│   │
│   └── lexer/
│       ├── Lexer.h             # DFA simulator with Maximal Munch (getNextToken for Phase 2)
│       ├── Token.h             # Token data structure (class, lexeme, line, col)
│       ├── SymbolTable.h       # Identifier symbol table (insert, lookup, print)
│       └── ErrorHandler.h      # Lexical error reporting and recovery
│
├── src/
│   ├── main.cpp                # Pipeline driver orchestrating steps 1 to 10
│   ├── input/
│   │   ├── RulesReader.cpp
│   │   └── RulesParser.cpp
│   ├── regex/
│   │   ├── RegexParser.cpp
│   │   └── RegexUtils.cpp
│   ├── automata/
│   │   ├── NFA.cpp
│   │   ├── NFAConstructor.cpp
│   │   ├── NFAMerger.cpp
│   │   ├── DFA.cpp
│   │   ├── DFABuilder.cpp
│   │   └── DFAMinimizer.cpp
│   ├── generator/
│   │   ├── TransitionTable.cpp
│   │   └── TableExporter.cpp
│   └── lexer/
│       ├── Lexer.cpp
│       ├── SymbolTable.cpp
│       └── ErrorHandler.cpp
│
└── tests/
    ├── regex_tests.cpp         # Unit tests for regex preprocessing & AST
    ├── nfa_tests.cpp           # Unit tests for Thompson construction & merging
    ├── dfa_tests.cpp           # Unit tests for subset construction & minimization
    └── lexer_tests.cpp         # Unit tests for tokenizer & symbol table
```

---

## How to Build and Run

### Option 1: Using Make (Immediate / Ready Out of the Box)

```bash
# Build the generator and all test suites
make

# Run the full pipeline on default input files
make run

# Run individual test suites
make test_regex
make test_nfa
make test_dfa
make test_lexer

# Clean build artifacts
make clean
```

### Option 2: Using CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .

# Run main program
./lexical_analyzer_generator ../input/lexical_rules.txt ../input/test_program.java ../output/transition_table.txt ../output/tokens.txt

# Run tests
ctest --output-on-failure
```

---

## Implementation Roadmap (Tasks to Implement)

Each file has been prepared with full function signatures, data types, and `// TODO:` guidance comments:

1. **Input Rules Parsing (`src/input/RulesParser.cpp`)**
   - Extract keywords from `{ ... }` lines.
   - Extract punctuation characters from `[ ... ]` lines.
   - Parse regular definitions (`LHS = RHS`) and substitute prior definitions.
   - Parse regular expressions (`LHS: RHS`) and maintain rule order for priority.

2. **Regex Preprocessing & AST (`src/regex/RegexUtils.cpp`, `src/regex/RegexParser.cpp`)**
   - Expand ranges: `a-z` $\to$ `(a|b|...|z)`.
   - Insert explicit concatenation operator `EXPLICIT_CONCAT`.
   - Shunting-yard algorithm to convert infix to postfix notation.
   - Construct AST nodes or evaluate postfix directly.

3. **Thompson's Construction (`src/automata/NFAConstructor.cpp`)**
   - `createLiteralNFA`: Basic 2-state NFA on a symbol.
   - `createEpsilonNFA`: 2-state NFA on $\epsilon$ ($\lambda$).
   - `createConcatNFA`: Connect accept state of $N_1$ to start state of $N_2$.
   - `createUnionNFA`: New start and accept states with $\epsilon$-transitions.
   - `createStarNFA`: Kleene star with forward and loopback $\epsilon$-transitions.
   - `createPlusNFA`: Positive closure ($N \cdot N^*$).

4. **NFA Merging (`src/automata/NFAMerger.cpp`)**
   - Create a single global start state with $\epsilon$-transitions to each token's NFA start state.
   - Retain token class and priority on each accept state.

5. **Subset Construction (`src/automata/DFABuilder.cpp`)**
   - Implement `epsilonClosure(states)`.
   - Implement `move(states, symbol)`.
   - Build unminimized DFA.
   - Break acceptance ties in favor of the rule with lowest priority number (first listed in rules).

6. **DFA Minimization (`src/automata/DFAMinimizer.cpp`)**
   - Initial partition: Group non-accepting states together; group accepting states by `tokenClass`.
   - Iteratively refine partitions until no group can be split.
   - Construct reduced DFA from equivalence classes.

7. **Transition Table Generation & Export (`src/generator/TransitionTable.cpp`, `src/generator/TableExporter.cpp`)**
   - Map states $\times$ alphabet to next states.
   - Export formatted transition table to `output/transition_table.txt`.

8. **Lexer Simulation (`src/lexer/Lexer.cpp`)**
   - Implement **Maximal Munch**: scan input character-by-character tracking the last visited accept state.
   - On mismatch/dead state: emit the token for the longest prefix match, or trigger error recovery if no accept state was reached.
   - Insert identifiers into `SymbolTable`.
   - Produce tokens stream to `output/tokens.txt`.
   - Note: `getNextToken()` is ready for seamless integration with Phase 2 (Parser Generator)!
