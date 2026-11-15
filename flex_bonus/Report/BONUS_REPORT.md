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
.team { text-align: center; margin: 24px 0 28px; }
.team p { font-size: 22px; margin-bottom: 12px; }
.team table { margin: 0 auto; font-size: 19px; line-height: 1.5; }
.team th, .team td { padding: 10px 28px !important; text-align: center; }
.team td:nth-child(2) { font-size: 24px; }
</style>

# Flex/C++ Lexical Analyzer

## BONUS Part — Implementation and Verification Report

**Date:** 15 November 2026  
**Platform:** Ubuntu Linux 22.04  • **Flex:** 2.6.4  • **Compiler:** g++ 11.4.0

<div class="team">

**Team members**

| # | Name | ID |
|---|---|---|
| 1 | جون وليم | 22010701 |
| 2 | كارولين حنا | 22011029 |
| 3 | مايكل مجدي نجيب | 22011082 |
| 4 | مينا مبيل يوسف | 22011290 |

</div>

![Verified Flex and g++ versions](report%20assets/Version.png)

> **Executive summary.** This project implements a small lexical analyzer with Flex. The human-written specification is `lexer.l`. Flex translates that specification into the generated C++ source `lex.yy.cpp`; g++ then compiles and links that source into the executable `lexer`. When `lexer` is run with redirected input, `main()` calls `yylex()`, which reads characters, groups them into the longest matching token, and prints the corresponding token category.

<div style="page-break-before: always;"></div>

## 1. System overview

The complete build-and-run path is:

```text
lexer.l  ── flex ──▶  lex.yy.cpp  ── g++ ──▶  lexer
   │                                      │
   │                                      └── ./lexer < input_test_N.txt
   │                                                       │
   └─ regular expressions + actions                       └─ token stream / errors
```

This separation is important: `lexer.l` is the source of truth, `lex.yy.cpp` is a generated intermediate artifact, and `lexer` is the runnable product. If the rules change, `lex.yy.cpp` and the executable must be regenerated.

<div style="page-break-before: always;"></div>

## 2. Installation and environment

On Ubuntu/Debian, install Flex and the GNU C++ compiler:

```bash
sudo apt update
sudo apt install flex g++
```

The version check confirms the tools used for verification:

```bash
flex --version
g++ --version
```

![Ubuntu package installation](report%20assets/installation.png)

![Tool version verification](report%20assets/Version.png)

<div style="page-break-before: always;"></div>

## 3. The scanner specification: `lexer.l`

Flex files have three logical regions separated by `%%`:

1. **Definitions and C++ prologue** — headers, aliases, and reusable regular expressions.
2. **Rules and actions** — patterns on the left and C++ code on the right.
3. **User C++ code** — here, `main()` starts scanning by calling `yylex()`.

The reusable definitions make the rules readable:

```lex
LETTER  [a-zA-Z]
DIGIT   [0-9]
ID      {LETTER}({LETTER}|{DIGIT})*
NUM     {DIGIT}+(\.{DIGIT}+(E{DIGIT}+)?)?
```

The rule table maps source text to the project’s output vocabulary:

| Input pattern | Output | Purpose |
|---|---|---|
| `int`, `float`, `boolean`, `if`, `else`, `while` | the keyword itself | Reserved words |
| `{NUM}` | `num` | Integers and implemented decimal/scientific form |
| `{ID}` | `id` | Letter-starting identifiers |
| `==`, `!=`, `<=`, `>=`, `<`, `>` | `relop` | Relational operators |
| `=` | `assign` | Assignment |
| `+`, `-` | `addop` | Additive operators |
| `*`, `/` | `mulop` | Multiplicative operators |
| delimiters | the character itself | `(`, `)`, `{`, `}`, `,`, `; ` |
| whitespace | no output | Spaces, tabs, CR, and LF |
| any other character | `Lexical error: <character>` | Error reporting |

Flex chooses the longest possible match. When matches have the same length, rule order matters; placing keyword rules before `{ID}` ensures that `while` is emitted as a keyword rather than as a generic identifier.

<div style="page-break-before: always;"></div>

## 4. From `lexer.l` to runnable C++

### Step 1 — Generate the C++ scanner

Run:

```bash
flex lexer.l
```

Flex reads the regular expressions and actions in `lexer.l` and writes the generated scanner to `lex.yy.cpp`. This file contains the buffering logic, matching engine, `yytext` handling, and action dispatch code. It is generated code, so it should not be edited by hand.

The explicit-output form is also valid:

```bash
flex -o lex.yy.cpp lexer.l
```

![Flex generation and compilation in the terminal](report%20assets/lexer%20setup.png)

### Step 2 — Compile and link the generated source

```bash
g++ lex.yy.cpp -o lexer
```

g++ compiles the generated C++ source into machine code and links it into the executable `lexer`. The `-o lexer` option names the output file.

### Step 3 — Start scanning with redirected input

The bottom of `lexer.l` defines:

```cpp
int main() {
    yylex();
    return 0;
}
```

`./lexer < input_test_1.txt` connects the file to standard input. `yylex()` repeatedly reads from that stream, applies the rule table, executes each matching action, and continues until end-of-file. The actions print one token result per line, so whitespace disappears while meaningful lexical units remain visible.

<div style="page-break-before: always;"></div>

## 5. Verification tests

All tests were run from `/home/john/flex_bonus` after rebuilding the executable.

### Test 1 — declarations, punctuation, loop, and `!=`

Input file: `input_test_1.txt`

```c
int sum, count, pass, mnt;
while (pass != 10)
{
    pass = pass + 1;
}
```

The output correctly contains the keywords `int` and `while`, identifiers, punctuation, `relop` for `!=`, `num` for `10`, `assign` for `=`, and `addop` for `+`. **Result: PASS.**

![Test 1 input and token output](report%20assets/test%201.png)

### Test 2 — decimal/scientific number, comparison, and subtraction

Input file: `input_test_2.txt`

```c
float x = 12.5E3;
if (x >= 10) {
    x = x - 1;
}
```

The scanner classifies `12.5E3` and `10` as `num`, `>=` as `relop`, and `-` as `addop`. **Result: PASS.**

![Test 2 input and token output](report%20assets/test%202.png)

### Test 3 — boolean declaration, multiplication, division, and `else`

Input file: `input_test_3.txt`

This case exercises `boolean`, both branches of an `if`/`else` structure, `==`, `*`, and `/`. The screenshot shows the source and the corresponding token stream, including `mulop` for both arithmetic operators. **Result: PASS.**

![Test 3 input and token output](report%20assets/test%203.png)

### Test 4 — loop, scientific notation, and invalid characters

Input file: `input_test_4.txt`

This test includes `@invalid #token` between valid statements. The catch-all rule reports `@` and `#` as lexical errors, then scanning continues. The later `3.14E2` is recognized as `num`, demonstrating recovery after invalid input. **Result: PASS.**

![Test 4 input, token output, and lexical errors](report%20assets/test%204.png)

<div style="page-break-before: always;"></div>

## 6. Reproducible build and run procedure

```bash
cd /home/john/flex_bonus
flex lexer.l
g++ lex.yy.cpp -o lexer

./lexer < input_test_1.txt
./lexer < input_test_2.txt
./lexer < input_test_3.txt
./lexer < input_test_4.txt
```

For a one-line interactive error check:

```bash
printf 'int @ 12.5E3\n' | ./lexer
```

To verify that a fresh generated scanner behaves identically:

```bash
flex lexer.l && g++ lex.yy.cpp -o lexer_rebuilt
./lexer_rebuilt < input_test_1.txt
```

<div style="page-break-before: always;"></div>

## 7. Findings and limitations

- The implementation performs lexical analysis only; it does not parse expressions or build an AST.
- Output reports token categories but not line or column positions.
- Identifiers must start with a letter and do not accept underscores.
- The numeric expression supports the implemented decimal/scientific form, not every floating-point notation.
- Invalid characters are reported to standard error and scanning continues.
- `lex.yy.cpp` is generated code and should be regenerated whenever `lexer.l` changes.

<div style="page-break-before: always;"></div>

## Conclusion

The evidence shows a complete, reproducible Flex-to-C++ pipeline: the rules in `lexer.l` generate `lex.yy.cpp`, g++ builds the `lexer` executable, and redirected source files produce the expected token streams. The four tests cover normal tokens, operators, delimiters, numeric forms, whitespace handling, and error recovery.

