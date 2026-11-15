# Spec-driven lab tests (Phase 1)

Run: `make lab_test`   (or `python3 tests/lab/run_lab_tests.py -v [--fuzz N] [--seed N]`)

* `cases/<name>/rules.txt`    lexical rules file fed to the generator
* `cases/<name>/input.txt`    program to tokenize (byte-exact, some contain CRLF / non-ASCII / NUL)
* `cases/<name>/expected.txt` hand-derived from Phase 1.pdf: `<class> <lexeme>` per token, then
  `!ERR <char> <line>:<col>` per lexical error, then optional `--symbols--` + `<id> <first line>`
  `L*` cases use the sheet's rules; `C*` cases use small custom rule sets for a single feature.
* `dump_tokens.cpp`  harness: runs the real pipeline and prints the token/error/symbol dump
* `run_lab_tests.py` also checks: CLI output files, the exported table is a *minimal* DFA (independent
  Moore check), bad rule files fail cleanly, differential fuzzing against a Python `re` oracle
  (the sheet's rules + random regex rule sets), and large-input stress.
