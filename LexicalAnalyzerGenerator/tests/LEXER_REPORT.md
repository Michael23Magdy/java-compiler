<style>
/* Readable colours on white paper (same hues as the default theme, darker) */
pre, tr, img { page-break-inside: avoid; }
body { color: #1f2328; }
:not(pre):not(.hljs) > code { color: #8a3b00; background-color: #f6f1ea; padding: 1px 4px; border-radius: 3px; }
pre.hljs, .hljs { color: #24292f; }
.hljs-comment, .hljs-quote { color: #57606a; }
.hljs-variable, .hljs-template-variable, .hljs-tag, .hljs-name, .hljs-selector-id, .hljs-selector-class, .hljs-regexp, .hljs-deletion { color: #b31d28; }
.hljs-number, .hljs-built_in, .hljs-builtin-name, .hljs-literal, .hljs-type, .hljs-params, .hljs-meta, .hljs-link { color: #a04a00; }
.hljs-attribute { color: #7a5c00; }
.hljs-string, .hljs-symbol, .hljs-bullet, .hljs-addition { color: #3f6b00; }
.hljs-title, .hljs-section { color: #2f5f9e; }
.hljs-keyword, .hljs-selector-tag { color: #6f3fa0; }
.shots td { border: none !important; vertical-align: top; padding: 0 24px 0 0 !important; }
.shots td:first-child img { width: 150px; }
.shots td:last-child img { width: 470px; }
/* Compact DFA table */
.dfa-table table { font-size: 8px; line-height: 9px; margin: 0; }
.dfa-table th, .dfa-table td { padding: 1.5px 3px !important; line-height: 9px !important; text-align: center; }
.dfa-table code { font-size: 8px; line-height: 9px; padding: 0; background: none; }
.team { text-align: center; margin: 24px 0 28px; }
.team p { font-size: 22px; margin-bottom: 12px; }
.team table { margin: 0 auto; font-size: 19px; line-height: 1.5; }
.team th, .team td { padding: 10px 28px !important; text-align: center; }
.team td:nth-child(2) { font-size: 24px; }
</style>

# Manual Lexical Analyzer Generator

## Phase 1 — Implementation and Verification Report

**Date:** 15 November 2026  
**Platform:** Ubuntu Linux 22.04  • **Language:** C++17  • **Compiler:** g++ 11.4.0

<div class="team">

**Team members**

| # | Name | ID |
|---|---|---|
| 1 | جون وليم | 22010701 |
| 2 | كارولين حنا | 22011029 |
| 3 | مايكل مجدي نجيب | 22011082 |
| 4 | مينا مبيل يوسف | 22011290 |

</div>

> **Executive summary.** We wrote the lexical analyzer generator by hand in C++, without Lex or Flex. It reads the lexical rules file, builds one NFA per rule, joins the NFAs under a new start state, converts the result to a DFA, and minimizes it. For the given Java subset, the DFA goes from **185 states to 44**. A table-driven lexer then scans the test program with longest-match rules and produces the expected **23 tokens**.

<div style="page-break-before: always;"></div>

## 1. System overview

```text
lexical_rules.txt
   ▼
RulesParser      → token rules in priority order
   ▼
RegexParser      → regular expression → syntax tree
   ▼
NFAConstructor   → one NFA per rule (Thompson)        18 NFAs
   ▼
NFAMerger        → new start state + ε-edges          733 states
   ▼
DFABuilder       → subset construction                185 states
   ▼
DFAMinimizer     → partition refinement                44 states
   ▼
TransitionTable  → output/transition_table.txt
   ▼
Lexer            → output/tokens.txt + symbol table + errors
```

Build and run:

```bash
cd LexicalAnalyzerGenerator
make          # build the generator and the tests
make run      # rules + test program from input/, results to output/
```

<div style="page-break-before: always;"></div>

## 2. Input: the lexical rules

The rules file from the assignment is used unchanged. Each line is classified as follows:

| Line | Meaning |
|---|---|
| `name = RHS` | regular definition, reused by later lines |
| `name: RHS` | token rule; the token class is `name` |
| `{ … }` | keywords; the token class is the word itself |
| `[ … ]` | punctuation; the token class is the symbol itself |

- Definitions are substituted in parentheses, so `digit+` becomes `((0-9))+` and precedence is kept.
- Each rule gets a **priority** in file order (1, 2, 3, …). A lower number wins a tie.
- Keywords get priority **0**, so they always beat `id` (see Assumption 1).

<div style="page-break-before: always;"></div>

## 3. Data structures

| Structure | Purpose |
|---|---|
| `Rule` | name, expression, type, priority |
| `RegexNode` tree | literal, ε, concat, union, `*`, `+` nodes |
| `NFA` | vector of states; each state maps a symbol to a **list** of targets (ε = `'\0'`) |
| `DFA` | vector of states; each state maps a symbol to **one** target and remembers its set of NFA states |
| `map<set<StateID>, StateID>` | finds an existing DFA state for a set of NFA states |
| `Partition` | groups of DFA states plus a state → group index |
| `TransitionTable` | 2-D array `[state][symbol]`; `-1` means the dead state |
| `Token` | class, lexeme (attribute value), line, column |
| `SymbolTable` | hash map for lookup plus a vector that keeps insertion order |
| `ErrorHandler` | list of errors (character, line, column) |

<div style="page-break-before: always;"></div>

## 4. Algorithms

**Regex parsing.** The parser runs three steps:

1. Expand ranges: `a-z` → `(a|b|…|z)`.
2. Insert an explicit concatenation operator between adjacent operands.
3. Convert to postfix with the **Shunting-yard** algorithm and build the syntax tree. Precedence is `*` `+` > concatenation > `|`.

**Thompson's construction.** Each tree node becomes a small NFA joined with ε-edges: a literal, union, concatenation, `*`, or `+` (`+` is `*` without the skip edge). The rule's token class and priority are stored on its accepting state.

**Combining the NFAs.** A new start state gets an ε-edge to the start of each of the 18 rule NFAs.

**Subset construction.** Starting from ε-closure(start), a worklist computes ε-closure(move(S, a)) for every symbol `a`. Each new set becomes a DFA state, and empty sets are skipped. If a DFA state contains several accepting NFA states, it takes the class with the **lowest priority number**. That is how ties go to the rule listed first.

**Minimization (partition refinement).**

1. Start with one group for the non-accepting states and one group per token class.
2. Split any group whose states go to different groups on some symbol.
3. Repeat until nothing splits, then make each group one state.

**Lexer (maximal munch).** The lexer runs the DFA one character at a time until it reaches the dead state, remembering the **last accepting position**. It then backs up to that position and emits the token. For example, `3.` gives `num 3` and an error on `.`. Identifiers are added to the symbol table. `getNextToken()` returns `$` at end of input, ready for the Phase 2 parser.

**Error recovery.** If no prefix matches, the lexer reports the character with its line and column, skips it, and continues:

```text
Lexical error at line 2, column 23: Unrecognized character (character: '@')
```

<div style="page-break-before: always;"></div>

## 5. Minimal DFA transition table

The full table is in `output/transition_table.txt`: **44 states × 77 symbols**, with start state **3**. Below, symbols whose columns are identical are merged into one column. This loses no information. A `·` means the dead state.

**Column key:** `dig` = 0–9 • `*/` = `*` or `/` • `+-` = `+` or `-` • `<>` = `<` or `>` • `oth` = letters not used in any keyword (`A–D F–Z c d g j k m p q r u v x y z`).

<div class="dfa-table">

| St | `!` | `(` | `)` | `*/` | `+-` | `,` | `.` | dig | `;` | `<>` | `=` | oth | `E` | a | b | e | f | h | i | l | n | o | s | t | w | `{` | `}` | Accepts |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | · | · | · | · | · | · | · | · | · | · | 39 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | — |
| 1 | · | · | · | · | · | · | · | 36 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | — |
| 2 | · | · | · | · | · | · | · | 37 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | — |
| **3** | 0 | 4 | 5 | 35 | 8 | 6 | · | 38 | 7 | 40 | 9 | 26 | 26 | 26 | 25 | 20 | 23 | 26 | 31 | 26 | 26 | 26 | 26 | 26 | 27 | 42 | 43 | — (start) |
| 4 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `(` |
| 5 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `)` |
| 6 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `,` |
| 7 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `;` |
| 8 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `addop` |
| 9 | · | · | · | · | · | · | · | · | · | · | 39 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `assign` |
| 10 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `boolean` |
| 11 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `else` |
| 12 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `float` |
| 13 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 11 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 14 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 10 | 26 | 26 | 26 | 26 | · | · | `id` |
| 15 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 12 | 26 | · | · | `id` |
| 16 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 14 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 17 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 15 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 18 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 13 | 26 | 26 | · | · | `id` |
| 19 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 16 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 20 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 18 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 21 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 17 | 26 | 26 | 26 | · | · | `id` |
| 22 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 19 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 23 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 21 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 24 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 22 | 26 | 26 | 26 | · | · | `id` |
| 25 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 24 | 26 | 26 | 26 | · | · | `id` |
| 26 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 27 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 28 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 28 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 29 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 29 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 32 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 30 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 34 | 26 | · | · | `id` |
| 31 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 33 | 26 | 26 | 26 | 30 | 26 | 26 | 26 | 26 | · | · | `id` |
| 32 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 41 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `id` |
| 33 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `if` |
| 34 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `int` |
| 35 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `mulop` |
| 36 | · | · | · | · | · | · | · | 36 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `num` |
| 37 | · | · | · | · | · | · | · | 37 | · | · | · | · | 1 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `num` |
| 38 | · | · | · | · | · | · | 2 | 38 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `num` |
| 39 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `relop` |
| 40 | · | · | · | · | · | · | · | · | · | · | 39 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `relop` |
| 41 | · | · | · | · | · | · | · | 26 | · | · | · | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | 26 | · | · | `while` |
| 42 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `{` |
| 43 | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | · | `}` |

</div>

**Example paths from state 3:**

- **`while`:** 3 → 27 → 28 → 29 → 32 → 41. One more letter or digit goes to 26 (`id`), so `whilex` is an identifier.
- **Numbers:** digit → 38. Then `.` → 2 → digit → 37, and `E` → 1 → digit → 36.
- **Operators:** `=` → 9 (`assign`), and a second `=` goes to 39 (`relop`). `!` alone stops in state 0, which is not accepting, so it is an error.

<div style="page-break-before: always;"></div>

## 6. Token stream for the test program

<table><tr><td style="vertical-align: top; border: none; padding-right: 30px;">

**Input**

```java
int sum , count , pass , mnt; while (pass !=10)
{
pass = pass + 1 ;
}
```

**Symbol table**

| Identifier | First line |
|---|---|
| `sum` | 1 |
| `count` | 1 |
| `pass` | 1 |
| `mnt` | 1 |

</td><td style="vertical-align: top; border: none;">

**`output/tokens.txt`**

```text
int
id
,
id
,
id
,
id
;
while
(
id
relop
num
)
{
id
assign
id
addop
num
;
}
```

</td></tr></table>

The output matches the example in the assignment. `mnt;` and `!=10` are split correctly without blanks. **Result: PASS.**

<div style="page-break-before: always;"></div>

## 7. Testing

| Test | Result |
|---|---|
| Unit tests: regex, NFA, DFA, lexer (`make test_*`) | PASS |
| Lab tests (`make lab_test`): 53 hand-written cases, CLI checks, bad rule files, 400 fuzz cases against Python `re`, stress tests | **73 / 73 passed** |
| Exported table checked for minimality by an independent algorithm | PASS |

### Test 1 — the assignment's test program

Input:

```java
int sum , count , pass , mnt; while (pass !=10)
{
pass = pass + 1 ;
}
```

The lexer outputs the 23 tokens the assignment expects. It finds the identifiers `sum`, `count`, `pass` and `mnt`, and reports no errors. **Result: PASS.**

<table class="shots"><tr><td>

![Test 1 tokens](report%20assets/test%201%20tokens.jpg)

</td><td>

![Test 1 symbol table and errors](report%20assets/test%201%20symbol%20table.jpg)

</td></tr></table>

<div style="page-break-before: always;"></div>

### Test 2 — numbers, operators, keyword-like identifiers and errors

Input:

```java
int x1 = 3.14E2 + @ y;
if (x1 >= 10) { z = 5 $ ; }
intx float2 == != <=
```

- `3.14E2` is a single `num`.
- `@` (line 1, column 19) and `$` (line 2, column 23) are reported as errors and skipped. Scanning continues after each one.
- `intx` and `float2` are identifiers, not keywords, because the longest match wins.
- `==`, `!=` and `<=` are each one `relop`.
- The symbol table holds `x1`, `y`, `z`, `intx` and `float2`.

**Result: PASS.**

<table class="shots"><tr><td>

![Test 2 tokens](report%20assets/test%202%20tokens.jpg)

</td><td>

![Test 2 symbol table and errors](report%20assets/test%202%20symbol%20table.jpg)

</td></tr></table>

<div style="page-break-before: always;"></div>

## 8. Assumptions

1. **Keywords always have the highest priority, regardless of their order in the rules file.** All keywords get priority 0, so they beat every regular expression on a match of the same length. In the given file `id` comes before the keywords, but the sheet says keywords are reserved and expects `int` and `while` as tokens.
2. Lexical Analyzer generator outputs the transition table and passes it directly to the lexer (without a separate lexer program parsing the Java Code)
3. **Token class names** are the rule name (`id`, `num`, …) or the keyword or symbol itself (`int`, `;`), as in the sheet's sample output.
4. **Whitespace** separates tokens and is skipped. Spaces inside a rule are ignored, so `0 - 9` and `0-9` mean the same.
5. **`-`** is a range only between two letters or digits. **`.`** is a literal decimal point.
6. **`\L`** is ε, and **`\x`** is the literal character `x`.
7. **Error recovery** skips one character, so valid tokens next to the error are kept.
8. **The dead state** is not stored as a row. A missing transition (`·`) is the dead state.

<div style="page-break-before: always;"></div>

## Conclusion

The generator implements the full Phase 1 pipeline in C++. It builds the NFAs, merges them, converts them to a DFA, and minimizes it to 44 states. The lexer scans with longest-match rules and first-listed tie-breaking, fills the symbol table, and recovers from errors. It produces exactly the token stream the assignment expects, and all tests pass.
