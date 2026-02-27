# HAZARD Modernization Guide

> Generated: 2026-02-27
> Based on: Deep static analysis of src/ (42 files read, ~33K LOC)
> Constraint: Backwards-compatible input/output syntax preserved throughout

---

## Overview

This guide is ordered **riskiest first**. The global struct pattern and numerical core are load-bearing walls — touch them wrong and the math silently breaks. Parser/build modernization is comparatively safe. Do not reorder phases.

---

## 1. Migration Checklist (Ordered by Risk)

### Phase 1 — Foundation: Context Struct (HIGHEST RISK)
The global `HZRstr`, `Common`, `G`, `Early`, `Late`, `Constant` structs are the most dangerous. Every function touches them implicitly. This phase creates the scaffolding everything else depends on.

- [ ] **1.1** Audit all global structs: `HZRstr` (74 fields), `Common`, `G`, `Early`, `Late`, `Constant`, `Setcoe`, `HazG1` — document every field and which modules write to it
- [ ] **1.2** Create `struct hazard_context` that wraps all globals into a single owned context (see §2 for pattern)
- [ ] **1.3** Add a `hazard_context_init()` / `hazard_context_free()` pair that replaces the scattered global initialization
- [ ] **1.4** Thread-safe accessor macros: `#define HZRstr (ctx->hzr)` so existing code compiles without changes in a transitional build
- [ ] **1.5** Validate: run full test suite — output must be bit-for-bit identical before proceeding

### Phase 2 — Numerical Core: llike + optim (HIGH RISK)
The log-likelihood and optimizer are the mathematical heart. Bugs here produce wrong numbers, not crashes — the worst kind.

- [ ] **2.1** Add parameter signatures to `dllike()`, `setcoe()`, `setobj()`: pass `hazard_context *ctx` explicitly
- [ ] **2.2** Replace `Common.obs[G.Im1p7+N]` magic offsets with `struct obs_row { double time, c1, c2, c3, ..., weight; }` and a typed accessor
- [ ] **2.3** Refactor `dnmr2.c` FORTRAN state machine (`next` variable): extract to explicit sub-functions named for each case label
- [ ] **2.4** Lift `static double lamprv, fpprv` in `linesr.c` into `struct optimizer_state` passed as parameter
- [ ] **2.5** Replace `QEXTD`/`QTOD`/`QSUM` macros with inline functions with descriptive names (`extended_from_double`, `double_from_extended`, `extended_add`)
- [ ] **2.6** Validate: numerical regression against saved test outputs (see §5)

### Phase 3 — Variable Selection: vars/ (MEDIUM RISK)
`STEPW()` has 14 parameters AND uses globals — a confused API. State flows via `HZRstr.hpsn` and `HZRstr.swadel` set as side effects.

- [ ] **3.1** Create `struct stepwise_result { int swadel; int hpsn; logical var_excluded; logical var_included; }` as return value
- [ ] **3.2** Change `void STEPW(...)` → `struct stepwise_result STEPW(struct hazard_context *ctx, ...)`
- [ ] **3.3** Replace 14-parameter flat signature with grouped parameter structs (see §2)
- [ ] **3.4** Make the `SWVARX → SWVARQ → SWVARI` control flow explicit via return codes, not via global `hpsn`
- [ ] **3.5** Validate: stepwise selection results match reference outputs

### Phase 4 — Type System (MEDIUM RISK)
Safe to do incrementally. Each change is small; aggregate effect is large.

- [ ] **4.1** Replace `typedef short int logical` with `#include <stdbool.h>` and `typedef bool logical` (C99) — keep `logical` name to avoid mass rename
- [ ] **4.2** Add `const` qualifiers to all read-only pointer parameters across the codebase
- [ ] **4.3** Replace `typedef char charVarName[8]` with bounded-string helpers using `snprintf` (retain 8-char SAS name limit as a constant `SAS_NAME_MAX`)
- [ ] **4.4** Replace magic error strings `"SETLIK930"` with an enum `HAZARD_ERR_*` and a `const char *hazard_strerror(int)` lookup
- [ ] **4.5** Replace `CLEAR(x)` / `memset(p,' ',48)` with zero-initialized structs or explicit `memset(p, 0, ...)`

### Phase 5 — Error Handling (MEDIUM RISK)
Currently: global `Common.errorno` + manual `if(Common.errorno) return` checks. Callers silently skip results on error.

- [ ] **5.1** Standardize error return: all computation functions return `int` (0 = success, nonzero = error code), output via out-parameter
- [ ] **5.2** Remove reliance on `setjmp`/`longjmp` in `structures.h` line 127 if unused; document if used
- [ ] **5.3** Replace `ErrorPassBack`, `ErrorReturn`, `ErrorReset` macros with explicit function calls
- [ ] **5.4** Add error context string to error struct: `struct hazard_error { int code; char message[256]; }`

### Phase 6 — Parser/Lexer (LOW RISK, isolated)
The Lex/Yacc parsers are already isolated. The risk here is regression in option parsing, not math.

- [ ] **6.1** Upgrade to re-entrant Flex (`%option reentrant`) and Bison (`%define api.pure full`) — eliminates yytext/yylval globals
- [ ] **6.2** Add `--` prefix check to all yacc tokens to future-proof option names without breaking existing syntax
- [ ] **6.3** Wrap scanner state in `struct parser_context { yyscan_t scanner; hazard_context *ctx; }` — no more globals in parser
- [ ] **6.4** Validate: run all example `.sas` programs through the new parser, diff output against reference

### Phase 7 — Build System (LOW RISK)
- [ ] **7.1** Update `configure.ac` minimum: remove AC_AIX, AC_ISC_POSIX, AC_MINIX (all obsolete)
- [ ] **7.2** Add `-Wall -Wextra -Wno-unused-parameter` to CFLAGS and fix or suppress all warnings
- [ ] **7.3** Add AddressSanitizer target: `make asan` with `CFLAGS=-fsanitize=address,undefined`
- [ ] **7.4** Consider parallel migration path to CMake (`CMakeLists.txt`) alongside Autotools — do not remove Autotools yet
- [ ] **7.5** Add CI configuration (GitHub Actions) that builds and runs validation suite on push

---

## 2. Code Transformations for Common Patterns

### Pattern A: Global struct → Context struct

**Before:**
```c
// structures.h
#ifdef STRUCT
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN struct hzrstr HZRstr;
EXTERN struct common  Common;

// setlik.c
void dllike(double *llike) {
    Common.errorno = 0;
    HZRstr.nfncts++;
    // ...
}
```

**After:**
```c
// hazard_context.h
typedef struct hazard_context {
    struct hzrstr  hzr;
    struct common  common;
    struct G_state g;
    struct early   early;
    struct late    late;
    // ... all former globals
} hazard_context;

hazard_context *hazard_context_create(void);
void            hazard_context_free(hazard_context *ctx);

// Transitional compatibility shim (remove after full migration):
// #define HZRstr (ctx->hzr)
// #define Common (ctx->common)

// setlik.c
int dllike(hazard_context *ctx, double *llike) {
    ctx->common.errorno = 0;
    ctx->hzr.nfncts++;
    // ...
    return ctx->common.errorno;
}
```

---

### Pattern B: Magic array offsets → Typed row struct

**Before:**
```c
// setcoe_obs_loop.c
G.Im1p7 = (G.I * HZRstr.pp7) - 1;

double time   = Common.obs[G.Im1p7 + 1];
double c1     = Common.obs[G.Im1p7 + 2];
double c2     = Common.obs[G.Im1p7 + 3];
double c3     = Common.obs[G.Im1p7 + 4];
double weight = Common.obs[G.Im1p7 + 7];
```

**After:**
```c
// obs_row.h
typedef struct obs_row {
    double time;
    double c1;
    double c2;
    double c3;
    double pad5;   // preserve original layout if binary compat needed
    double pad6;
    double weight;
} obs_row;

static inline const obs_row *obs_get_row(const hazard_context *ctx, int i) {
    return (const obs_row *)&ctx->common.obs[i * ctx->hzr.pp7];
}

// setcoe_obs_loop.c
const obs_row *row = obs_get_row(ctx, G.I);
double c1     = row->c1;
double c2     = row->c2;
double c3     = row->c3;
double weight = row->weight;
```

---

### Pattern C: FORTRAN state machine → Explicit functions

**Before (`dnmr2.c`):**
```c
for (i = 0, next = 50; i < nn; i += incx) {
    switch (next) {
        case 50:
            if (sx[i] == ZERO) continue;
            if (fabs(sx[i]) > Params.dnmrlo) { do85 = TRUE; break; }
            next = 70;
        case 70:
            sum += sx[i] * sx[i];
            continue;
        // ...
        case 110:
            // ...
    }
}
```

**After:**
```c
typedef enum { DNMR2_SMALL, DNMR2_LARGE } dnmr2_scale;

static dnmr2_scale classify_element(double val, const params *p) {
    if (fabs(val) > p->dnmrlo) return DNMR2_LARGE;
    return DNMR2_SMALL;
}

static double accumulate_small(const double *sx, int n, int incx) { ... }
static double accumulate_large(const double *sx, int n, int incx, double xmax) { ... }

double DNMR2(const hazard_context *ctx, const double *sx, int n, int incx) {
    // Determine scale, dispatch to appropriate accumulator
    ...
}
```

---

### Pattern D: Static locals → State struct

**Before (`linesr.c`):**
```c
static double lamprv = 0, fpprv = 0;

void linesr(...) {
    // lamprv, fpprv persist across calls — hidden state
}
```

**After:**
```c
// optimizer_state.h
typedef struct optimizer_state {
    double lamprv;
    double fpprv;
} optimizer_state;

// Stored in hazard_context or optimizer context:
typedef struct optim_context {
    optimizer_state line_search;
    // ... other optimizer state
} optim_context;

void linesr(optim_context *optim, ...) {
    optim->line_search.lamprv = ...;
}
```

---

### Pattern E: EXTERN/STRUCT macro → Normal declarations

**Before:**
```c
// setcoe.h
#ifdef SETCOE_DEFS
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN struct { xtended qsumcz; double sc1pc3; ... } Setcoe;

// setcoe.c
#define SETCOE_DEFS
#include "setcoe.h"    // Now EXTERN expands to nothing → definition
```

**After:**
```c
// setcoe.h  — just a declaration
typedef struct setcoe_state {
    double qsumcz;
    double sc1pc3;
    double c3w;
    double sumcf[4];
    double cumhct;
} setcoe_state;

// setcoe.c  — owned by hazard_context or passed explicitly
// No more macro tricks
static setcoe_state setcoe_compute(const hazard_context *ctx) { ... }
```

---

### Pattern F: logical type

**Before:**
```c
typedef short int logical;
#define TRUE  1
#define FALSE 0
```

**After (`compat.h` — one-time shim):**
```c
#include <stdbool.h>
typedef bool logical;    // keep name for backward source compat
#ifndef TRUE
#  define TRUE  true
#  define FALSE false
#endif
```

---

### Pattern G: Extended precision wrappers

**Before:**
```c
typedef double xtended;
#define QEXTD(x)    hzr_qextd(x)
#define QTOD(x)     hzr_qtod(x)
#define QSUM(x,y)   (x)+(y)     // macro — no type safety!

G.lnlike = QEXTD(ZERO);
G.lnlike = QSUM(G.lnlike, QEXTD(llike));
*llike   = QTOD(G.lnlike);
```

**After:**
```c
// extended_precision.h
typedef long double xtended;   // upgrade precision if platform supports it
                               // OR keep double if bit-compat required (see §3)

static inline xtended extended_from_double(double x)  { return (xtended)x; }
static inline double   double_from_extended(xtended x) { return (double)x;  }
static inline xtended  extended_add(xtended a, xtended b) { return a + b;   }
```

---

### Pattern H: Error handling

**Before:**
```c
Common.errorno = 0;
CLEAR(Common.errflg);
hzd_theta_to_parms(...);
if (Common.errorno) return;
```

**After:**
```c
int err = hzd_theta_to_parms(ctx, ...);
if (err) return err;
```

---

## 3. Breaking Changes to Watch For

These are changes that appear safe but will silently break results or ABI.

### B1 — Precision change in `xtended`
If you change `typedef double xtended` to `long double`, accumulated log-likelihoods will differ slightly across platforms. The optimizer may converge to different (arguably more correct) solutions. Output values for parameter estimates will not be bit-for-bit identical to the 2012 reference outputs. **Decision required:** bit-compat vs. precision upgrade.

### B2 — `static` locals in `linesr.c`
`lamprv` and `fpprv` are zero-initialized once at program start via C static semantics. If moved to a `struct`, you must explicitly zero-initialize on first use. Failure to do so causes the line search to start from a garbage state — optimizer diverges silently.

### B3 — `CLEAR` macro uses spaces, not zeros
`CLEAR(x)` does `memset(p, ' ', sizeof(p))` — it fills with ASCII 0x20, not 0x00. If you replace with `memset(p, 0, ...)` you change the content of error message buffers. This will not affect computation but may change error message formatting in output.

### B4 — `QSUM` macro operator precedence
```c
#define QSUM(x,y) (x)+(y)
```
This macro lacks outer parentheses. `a * QSUM(b, c)` expands to `a * (b) + (c)` — multiplication only applies to `b`. Fixing this (adding outer parens) changes evaluation order for any caller that composes QSUM inside larger expressions. Audit all callers before fixing.

### B5 — Parallel duplication of `Early` and `HZRstr.e`
`struct early Early` (global) and `HZRstr.e` (field) appear to be the same data in two places. Before migrating either, determine which one `SETG1` and `SETG3` write to. If both are written and read independently, they may be intentional (working copy vs. committed copy) — merging them will break the optimization rollback mechanism.

### B6 — `charVarName[8]` is a SAS protocol limit, not an implementation choice
SAS variable names are hard-limited to 8 characters in XPORT format (v5 transport). If you replace the fixed array with dynamic strings, you must still enforce the 8-char limit at the XPORT read/write boundary or you will produce files that SAS cannot read.

### B7 — Re-entrant Flex/Bison changes the call convention
Upgrading to `%option reentrant` in Flex changes `yylex()` to `yylex(yyscan_t scanner)`. Any code calling `yylex()` directly (outside the generated grammar) must be updated. Search for direct `yylex` calls before flipping this flag.

### B8 — Parser token overlap
The yacc grammar uses token names like `PROC`, `DATA`, `BY` that overlap with SAS reserved words. If new tokens are added to support new syntax, they must not shadow existing terminal symbols or parsing of old programs will silently change behavior.

---

## 4. Rollback Plan

### Prerequisite: establish a known-good baseline before any changes

```bash
# Tag the exact state before modernization begins
git tag v4.3.1-pre-modernization

# Build and capture reference outputs from all test programs
make clean && ./configure && make
cd tests/
./tests.sh > /tmp/reference_outputs.txt 2>&1
md5sum *.lst > /tmp/reference_md5sums.txt

# Store these — they are your ground truth
git add /tmp/reference_outputs.txt /tmp/reference_md5sums.txt
```

### Per-phase rollback

Each phase in §1 should be a separate git branch with a merge commit only after validation passes.

```
main
 └── phase-1/context-struct        ← branch, PR, merge only after tests pass
      └── phase-2/llike-refactor
           └── phase-3/vars-refactor
                └── ...
```

**To roll back a phase:**
```bash
git revert <merge-commit-sha>   # Preferred: creates a revert commit, preserves history
# OR
git reset --hard <tag-before-phase>  # Nuclear option: only if branch is not shared
```

### Transitional build flag
Add a build-time flag to enable/disable the context struct migration incrementally:

```c
// In configure.ac:
// AC_ARG_ENABLE([context-struct], ...)
// Generates: HAVE_HAZARD_CONTEXT in config.h

#ifdef HAVE_HAZARD_CONTEXT
    int dllike(hazard_context *ctx, double *llike) { ... }
#else
    void dllike(double *llike) { ... }  // legacy signature
#endif
```

This allows shipping a binary with both implementations and toggling via `./configure --enable-context-struct` without touching old code paths.

---

## 5. Validation Tests

### V1 — Bit-exact numerical regression (run after every phase)

```bash
#!/bin/bash
# tests/validate_numerical.sh
REFERENCE_DIR="tests/reference_outputs"
FAILURES=0

for sas_file in examples/*.sas; do
    name=$(basename "$sas_file" .sas)
    # Run through hazard (assumes SAS wrapper available)
    ./hazard < "tests/${name}.input" > "/tmp/${name}.output" 2>&1

    # Compare parameter estimates (first 6 decimal places)
    if ! diff <(grep "Coefficient" "/tmp/${name}.output" | awk '{printf "%.6f\n", $NF}') \
              <(grep "Coefficient" "${REFERENCE_DIR}/${name}.lst" | awk '{printf "%.6f\n", $NF}'); then
        echo "FAIL: ${name} — coefficient mismatch"
        FAILURES=$((FAILURES + 1))
    fi
done

echo "${FAILURES} failures"
exit $FAILURES
```

### V2 — Optimizer convergence test

The optimizer must reach the same final log-likelihood value (to 8 decimal places) and the same number of function evaluations (`nfncts`) as the reference run.

```bash
# Extract from output log:
grep "Log Likelihood" /tmp/test_output.lst | awk '{print $NF}' > /tmp/ll_new.txt
grep "Log Likelihood" tests/reference/test_output.lst | awk '{print $NF}' > /tmp/ll_ref.txt
diff /tmp/ll_new.txt /tmp/ll_ref.txt  # must be empty
```

### V3 — Stepwise selection path test

After Phase 3, the sequence of variables entered and removed must match exactly. The stepwise path is order-sensitive.

```bash
grep -E "^(Enter|Remove)" /tmp/test_output.lst > /tmp/sw_new.txt
grep -E "^(Enter|Remove)" tests/reference/test_output.lst > /tmp/sw_ref.txt
diff /tmp/sw_new.txt /tmp/sw_ref.txt
```

### V4 — XPORT round-trip test

```bash
# Convert XPORT → ASCII, re-read, verify no data loss
./xpt2ascii tests/AVC.xpt > /tmp/avc_ascii.txt
diff /tmp/avc_ascii.txt tests/reference/avc_ascii.txt
```

### V5 — Parser syntax regression

Every option in the example `.sas` programs must parse without error after Phase 6.

```bash
for f in examples/*.sas; do
    ./hazard --parse-only "$f" || echo "PARSE FAIL: $f"
done
```

### V6 — Memory safety (run after Phase 1 and Phase 2)

```bash
make clean
./configure CFLAGS="-fsanitize=address,undefined -g -O1"
make
./hazard < tests/AVC.input 2>&1 | grep -E "ERROR|LEAK|runtime error"
```

Expected output: nothing. Any AddressSanitizer or UBSan output is a defect introduced by the refactoring.

### V7 — Concurrent execution test (future-proofing, after Phase 1)

If context structs are added, verify two independent analyses can run without shared-state interference. Run two different datasets simultaneously and confirm both produce correct output:

```bash
./hazard < tests/AVC.input > /tmp/out1.txt &
./hazard < tests/AVC2.input > /tmp/out2.txt &
wait
diff /tmp/out1.txt tests/reference/AVC.lst
diff /tmp/out2.txt tests/reference/AVC2.lst
```

---

## Quick Reference: Risk Matrix

| Phase | Risk | Math Impact | Rollback Difficulty | Recommended First? |
|---|---|---|---|---|
| 1 — Context struct | High | None (mechanical) | Medium | Yes — enables everything else |
| 2 — llike/optim | High | Yes (subtle bugs possible) | Low (git revert) | After Phase 1 |
| 3 — vars/ stepwise | Medium | Yes (path-dependent) | Low | After Phase 2 |
| 4 — Type system | Medium | Minimal | Low | Parallel with Phase 3 |
| 5 — Error handling | Medium | None | Low | After Phase 1 |
| 6 — Parser/Lexer | Low | None | Low | Independent |
| 7 — Build system | Low | None | Very low | Independent, do first for CI |

**Recommended start sequence:** 7 → 1 → 5 → 4 → 2 → 3 → 6

Set up CI first so every subsequent phase has automated validation. Then tackle the god-object before anything else, because every other phase depends on it.
