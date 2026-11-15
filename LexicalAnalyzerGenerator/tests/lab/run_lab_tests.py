#!/usr/bin/env python3
"""
Spec-driven black-box tests for the Phase 1 lexical analyzer generator.

Everything here is derived from "Phase 1.pdf" (maximal munch, first-listed-rule
tie-break, reserved keywords, one-char-at-a-time error recovery, symbol table,
minimal DFA, token-class-per-line output), not from the implementation.

  1. cases/*          hand-written (rules, input, expected) triples
  2. CLI              bin/lexical_analyzer_generator on the sheet's own files
  3. DFA properties   exported table is deterministic, minimal, has a start state
  4. robustness       bad rule files / missing files must fail cleanly (no abort)
  5. differential     random inputs for the sheet's rules + random regex rule
                      sets, checked against an independent Python oracle (re)
  6. stress           very long identifiers / many tokens

usage: python3 tests/lab/run_lab_tests.py [-v] [--seed N] [--fuzz N]
(run `make lab_tests` to build bin/dump_tokens first, or just `make lab_test`)
"""
import os, re, sys, glob, random, subprocess, tempfile, argparse, time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
DUMP = os.path.join(ROOT, "bin", "dump_tokens")
MAIN = os.path.join(ROOT, "bin", "lexical_analyzer_generator")
LAB_RULES = os.path.join(ROOT, "input", "lexical_rules.txt")

ap = argparse.ArgumentParser()
ap.add_argument("-v", action="store_true")
ap.add_argument("--seed", type=int, default=12345)
ap.add_argument("--fuzz", type=int, default=300)
args = ap.parse_args()

passed = failed = 0
failures = []

def ok(name, _msg=None):
    global passed
    passed += 1
    if args.v: print("  PASS", name)

def bad(name, msg):
    global failed
    failed += 1
    failures.append(name)
    print("  FAIL", name)
    for l in msg.rstrip().split("\n")[:25]: print("       " + l)

def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, timeout=kw.pop("timeout", 60), **kw)

def dump(rules, inp, table_out=None):
    cmd = [DUMP, rules, inp] + ([table_out] if table_out else [])
    return run(cmd)

# ---------------------------------------------------------------- table checks
def parse_table(path):
    lines = open(path, encoding="latin-1").read().split("\n")
    start = int(re.search(r"Start State: (\d+)", "\n".join(lines)).group(1))
    hdr = next(i for i, l in enumerate(lines) if l.lstrip().startswith("State"))
    dashes = len(lines[hdr + 1])
    n = (dashes - 8 - 26) // 6
    header = lines[hdr]
    syms = []
    for i in range(n):
        s = header[8 + 6 * i: 14 + 6 * i].strip()
        syms.append(" " if s == "' '" else {"\\t": "\t", "\\n": "\n", "\\r": "\r"}.get(s, s))
    trans, acc = {}, {}
    for l in lines[hdr + 2:]:
        if not l.strip(): continue
        st = int(l[:8])
        row = {}
        for i, s in enumerate(syms):
            c = l[8 + 6 * i: 14 + 6 * i].strip()
            if c != "-": row[s] = int(c)
        rest = l[8 + 6 * n:]
        a = rest[:12].strip() == "yes"
        trans[st] = row
        acc[st] = rest[14:].strip() if a else None
    return start, syms, trans, acc

def check_table(path):
    """returns list of problems: unreachable states, equivalent (mergeable) states"""
    problems = []
    start, syms, trans, acc = parse_table(path)
    if start not in trans: problems.append("start state missing")
    reach, st = {start}, [start]
    while st:
        s = st.pop()
        for t in trans[s].values():
            if t not in trans: problems.append(f"transition to unknown state {t}")
            elif t not in reach: reach.add(t); st.append(t)
    if reach != set(trans): problems.append(f"unreachable states: {sorted(set(trans)-reach)}")
    # Moore partition refinement, missing transition == dead sink (-1)
    grp = {s: (acc[s] if acc[s] is not None else "\0nonacc") for s in trans}
    while True:
        ids, new = {}, {}
        for s in trans:
            sig = (grp[s],) + tuple(grp.get(trans[s].get(c, -1), -1) if trans[s].get(c, -1) != -1 else -1 for c in syms)
            new[s] = ids.setdefault(sig, len(ids))
        if len(set(new.values())) == len(set(grp.values())): break
        grp = new
    if len(set(grp.values())) != len(trans):
        problems.append(f"not minimal: {len(trans)} states but only {len(set(grp.values()))} distinct")
    # a dead (non-accepting, no way out to acceptance) state should not be stored
    live = {s for s in trans if acc[s] is not None}
    ch = True
    while ch:
        ch = False
        for s in trans:
            if s not in live and any(t in live for t in trans[s].values()):
                live.add(s); ch = True
    if live != set(trans): problems.append(f"dead states present: {sorted(set(trans)-live)}")
    return problems

# ---------------------------------------------------------------- 1. cases
def read_expected(path):
    toks, errs, syms, sec = [], [], None, "t"
    for l in open(path, encoding="latin-1").read().split("\n"):
        if l.startswith("#") or l == "": continue
        if l == "--symbols--": syms = []; sec = "s"; continue
        if sec == "s": syms.append(l)
        elif l.startswith("!ERR "): errs.append(l)
        else: toks.append(l)
    return toks, errs, syms

def split_output(out):
    toks, errs, syms, sec = [], [], [], "t"
    for l in out.split("\n"):
        if l == "": continue
        if l == "--symbols--": sec = "s"; continue
        if sec == "s": syms.append(l)
        elif l.startswith("!ERR "): errs.append(l)
        else: toks.append(l)
    return toks, errs, syms

def diff(exp, got):
    n = max(len(exp), len(got))
    for i in range(n):
        e = exp[i] if i < len(exp) else "<none>"
        g = got[i] if i < len(got) else "<none>"
        if e != g: return f"first difference at #{i+1}: expected {e!r}, got {g!r}  (expected {len(exp)} items, got {len(got)})"
    return ""

print("== 1. hand-written spec cases")
tmp = tempfile.mkdtemp()
for d in sorted(glob.glob(os.path.join(HERE, "cases", "*"))):
    name = os.path.basename(d)
    r = dump(os.path.join(d, "rules.txt"), os.path.join(d, "input.txt"), os.path.join(tmp, name + ".table"))
    if r.returncode != 0:
        bad(name, f"harness exit {r.returncode}: {r.stderr.decode(errors='replace')}"); continue
    et, ee, es = read_expected(os.path.join(d, "expected.txt"))
    gt, ge, gs = split_output(r.stdout.decode("latin-1"))
    msg = diff(et, gt) or diff(ee, ge) or (diff(es, gs) if es is not None else "")
    if msg: bad(name, msg + "\n" + open(os.path.join(d, "expected.txt")).readline().strip()); continue
    probs = check_table(os.path.join(tmp, name + ".table"))
    if probs: bad(name + " [DFA table]", "\n".join(probs)); continue
    ok(name)

# ---------------------------------------------------------------- 2. CLI
print("== 2. command line tool on the sheet's files")
cli_tmp = tempfile.mkdtemp()
tt, tk = os.path.join(cli_tmp, "table.txt"), os.path.join(cli_tmp, "tokens.txt")
r = run([MAIN, LAB_RULES, os.path.join(ROOT, "input", "test_program.java"), tt, tk])
exp_tokens = "int id , id , id , id ; while ( id relop num ) { id assign id addop num ; }".split()
if r.returncode != 0: bad("cli/exit-code", f"exit {r.returncode}\n{r.stderr.decode()}")
else: ok("cli/exit-code")
got = open(tk).read().split("\n") if os.path.exists(tk) else None
if got is None: bad("cli/tokens.txt", "file not created")
elif got[-1] == "" and got[:-1] == exp_tokens: ok("cli/tokens.txt (exactly the sheet's expected stream, one class per line)")
else: bad("cli/tokens.txt", f"expected {exp_tokens}\ngot      {got}")
if not os.path.exists(tt) or os.path.getsize(tt) == 0: bad("cli/transition table", "not written")
else:
    p = check_table(tt)
    if p: bad("cli/transition table is a minimal DFA", "\n".join(p))
    else: ok("cli/transition table is a minimal DFA")
out = r.stdout.decode()
for w in ("sum", "count", "pass", "mnt"):
    if not re.search(r"^%s\s" % w, out, re.M): bad("cli/symbol table prints " + w, out[-400:]); break
else: ok("cli/symbol table prints all identifiers")

# error program through the CLI: message must mention line/column and char, tokens continue
ep = os.path.join(cli_tmp, "err.java")
open(ep, "w").write("int a;\nint b @ c;\n")
r = run([MAIN, LAB_RULES, ep, tt, tk])
msg = (r.stdout + r.stderr).decode()
if r.returncode == 0 and re.search(r"line 2", msg) and "column 7" in msg and "@" in msg: ok("cli/error message has line, column and character")
else: bad("cli/error message", f"rc={r.returncode}\n{msg[-500:]}")
if open(tk).read().split() == "int id ; int id id ;".split(): ok("cli/error recovery continues scanning")
else: bad("cli/error recovery", open(tk).read())

# sheet's program as the *only* token on one line etc: empty program
ep2 = os.path.join(cli_tmp, "empty.java"); open(ep2, "w").close()
r = run([MAIN, LAB_RULES, ep2, tt, tk])
if r.returncode == 0 and open(tk).read() == "": ok("cli/empty program -> empty tokens file")
else: bad("cli/empty program -> empty tokens file", f"rc={r.returncode}")

# ---------------------------------------------------------------- 4. robustness
print("== 3. robustness of the generator itself")
def clean_fail(name, args_, want_ok=False):
    r = run(args_)
    if r.returncode < 0: bad(name, f"crashed with signal {-r.returncode} (uncaught exception/abort); stderr: {r.stderr.decode(errors='replace')[:200]}")
    elif want_ok: ok(name) if r.returncode == 0 else bad(name, f"exit {r.returncode}")
    elif r.returncode == 0: bad(name, "exit code 0 although the input is invalid")
    elif not (r.stderr or r.stdout): bad(name, "failed silently")
    else: ok(name)
rd = tempfile.mkdtemp()
def wr(n, s): p = os.path.join(rd, n); open(p, "w").write(s); return p
prog = wr("p.txt", "a b\n")
clean_fail("missing rules file", [MAIN, os.path.join(rd, "nope.txt"), prog, tt, tk])
clean_fail("empty rules file", [MAIN, wr("empty.txt", ""), prog, tt, tk])
clean_fail("unbalanced '(' in regex", [MAIN, wr("r1.txt", "x: (a|b\n"), prog, tt, tk])
clean_fail("unbalanced ')' in regex", [MAIN, wr("r2.txt", "x: a|b)\n"), prog, tt, tk])
clean_fail("dangling '|' in regex", [MAIN, wr("r3.txt", "x: a|\n"), prog, tt, tk])
clean_fail("leading '*' in regex", [MAIN, wr("r4.txt", "x: *a\n"), prog, tt, tk])
clean_fail("dangling backslash", [MAIN, wr("r5.txt", "x: a\\\n"), prog, tt, tk])
clean_fail("descending range z-a", [MAIN, wr("r6.txt", "x: z-a\n"), prog, tt, tk])
clean_fail("missing program file", [MAIN, LAB_RULES, os.path.join(rd, "nope.java"), tt, tk])

# ---------------------------------------------------------------- 5. differential
print("== 4. differential fuzzing vs. independent oracle")
LAB_TOKENS = [
    ("kw", r"boolean|int|float|if|else|while"),   # reserved words: highest priority
]
def oracle_lex(text, rules):
    """rules: ordered list of (class, compiled-regex). Longest prefix, ties -> earliest rule."""
    toks, errs, line, col, i = [], [], 1, 1, 0
    syms = []
    while i < len(text):
        ch = text[i]
        if ch in " \t\r\n":
            if ch == "\n": line += 1; col = 1
            else: col += 1
            i += 1; continue
        best, bcls = 0, None
        for cls, rx in rules:
            for L in range(min(len(text) - i, 60), 0, -1):
                if L > best or (L == best and bcls is None):
                    if rx.fullmatch(text[i:i + L]):
                        if L > best: best, bcls = L, cls
                        break
        if bcls is None:
            errs.append(f"!ERR {ch} {line}:{col}"); i += 1; col += 1; continue
        lex = text[i:i + best]
        toks.append(f"{bcls} {lex}")
        if bcls == "id" and lex not in [s.split()[0] for s in syms]: syms.append(f"{lex} {line}")
        col += best; i += best
    return toks, errs, syms

lab_oracle = [(k, re.compile(k)) for k in "boolean int float if else while".split()] + [
    ("id", re.compile(r"[A-Za-z][A-Za-z0-9]*")),
    ("num", re.compile(r"[0-9]+(\.[0-9]+(E[0-9]+)?)?")),
    ("relop", re.compile(r"==|!=|>=|<=|>|<")),
    ("assign", re.compile(r"=")),
] + [(c, re.compile(re.escape(c))) for c in ";,(){}"] + [
    ("addop", re.compile(r"[+\-]")),
    ("mulop", re.compile(r"[*/]")),
]
# note: oracle ties -> earlier in this list; keywords listed first = reserved.

rng = random.Random(args.seed)
frags = ["int", "float", "boolean", "if", "else", "while", "x", "y1", "ab", "E", "E5", "1", "23", "0", "1.5", "2.", ".5", "3.14E2",
         "1.5E", "==", "=", "!=", "!", "<", ">", "<=", ">=", "+", "-", "*", "/", "(", ")", "{", "}", ",", ";", "@", "#", "_", " ", " ", " ", "\n", "\t", "intx", "ifelse"]
okc = badc = 0
for it in range(max(1, args.fuzz // 3)):   # the sheet's DFA takes ~0.3s to generate
    text = "".join(rng.choice(frags) for _ in range(rng.randint(1, 60)))
    f = os.path.join(tmp, "fz.txt"); open(f, "w").write(text)
    r = dump(LAB_RULES, f)
    exp = oracle_lex(text, lab_oracle)
    got = split_output(r.stdout.decode("latin-1"))
    if r.returncode == 0 and exp[0] == got[0] and exp[1] == got[1] and exp[2] == got[2]: okc += 1
    else:
        badc += 1
        if badc <= 3: bad(f"lab-rules fuzz #{it}", f"input={text!r}\n" + (diff(exp[0], got[0]) or diff(exp[1], got[1]) or diff(exp[2], got[2])))
if badc == 0: ok(f"lab rules: {okc} random programs agree with oracle")

# random regex rule sets
def gen_re(depth, rng):
    """returns (our_syntax, python_syntax)"""
    r = rng.random()
    if depth == 0 or r < 0.3:
        c = rng.choice("abc")
        if rng.random() < 0.08: return ("\\L", "")
        return (c, c)
    if r < 0.5:
        a, b = gen_re(depth - 1, rng), gen_re(depth - 1, rng)
        return (f"{a[0]} {b[0]}" if rng.random() < .5 else a[0] + b[0], f"(?:{a[1]})(?:{b[1]})")
    if r < 0.7:
        a, b = gen_re(depth - 1, rng), gen_re(depth - 1, rng)
        return (f"({a[0]}|{b[0]})", f"(?:{a[1]}|{b[1]})")
    a = gen_re(depth - 1, rng)
    op = rng.choice("*+")
    return (f"({a[0]}){op}", f"(?:{a[1]}){op}")

okc = badc = 0
for it in range(args.fuzz):
    nr = rng.randint(1, 4)
    rules, orc = [], []
    for k in range(nr):
        o, p = gen_re(rng.randint(1, 4), rng)
        rules.append(f"r{k}: {o}")
        orc.append((f"r{k}", re.compile(p)))
    text = "".join(rng.choice("abc  \n") for _ in range(rng.randint(1, 50)))
    rf, inf = os.path.join(tmp, "fr.txt"), os.path.join(tmp, "fi.txt")
    open(rf, "w").write("\n".join(rules) + "\n"); open(inf, "w").write(text)
    r = dump(rf, inf, os.path.join(tmp, "fr.table"))
    exp = oracle_lex(text, orc)
    # empty-matching rules never produce tokens (empty prefix is no match)
    got = split_output(r.stdout.decode("latin-1"))
    good = r.returncode == 0 and exp[0] == got[0] and exp[1] == got[1]
    probs = check_table(os.path.join(tmp, "fr.table")) if r.returncode == 0 else []
    if good and not probs: okc += 1
    else:
        badc += 1
        if badc <= 3:
            bad(f"random-regex fuzz #{it}", f"rules={rules}\ninput={text!r}\nrc={r.returncode} {r.stderr.decode()[:200]}\n" +
                (diff(exp[0], got[0]) or diff(exp[1], got[1]) or "\n".join(probs)))
if badc == 0: ok(f"random regex rule sets: {okc} cases agree with oracle and give minimal DFAs")

# ---------------------------------------------------------------- 6. stress
print("== 5. stress")
big = "a" * 200000
f = os.path.join(tmp, "big.txt"); open(f, "w").write(big)
t0 = time.time(); r = dump(LAB_RULES, f); dt = time.time() - t0
if r.returncode == 0 and r.stdout.decode().startswith("id " + big) and dt < 10: ok(f"200k-char identifier ({dt:.2f}s)")
else: bad("long identifier", f"rc={r.returncode} time={dt:.1f}s")
prog = "int a = b + 1.5E3 ; if ( a >= 2 ) { a = a * 2 ; }\n" * 20000
f = os.path.join(tmp, "many.txt"); open(f, "w").write(prog)
t0 = time.time(); r = dump(LAB_RULES, f); dt = time.time() - t0
n = len([l for l in r.stdout.decode().split("\n") if l and not l.startswith("!ERR") and l != "--symbols--"])
if r.returncode == 0 and dt < 10: ok(f"400k-line program ({dt:.2f}s)")
else: bad("many tokens", f"rc={r.returncode} time={dt:.1f}s")

print(f"\n{passed} passed, {failed} failed")
if failures: print("failed:", ", ".join(failures))
sys.exit(1 if failed else 0)
