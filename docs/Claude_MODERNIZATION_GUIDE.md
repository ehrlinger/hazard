# HAZARD Modernization Guide

> Generated: 2026-02-27
> Last updated: 2026-04-18
> Status: v4.4.1 shipped; v5.0 planning in progress
> Constraint: backwards-compatible input/output syntax preserved throughout

---

## 0. Release Plan & Current Status

HAZARD is in production. The only safe way to evolve it is two release lines in parallel: a maintenance line that never breaks, and a modernization line where the load-bearing walls get touched behind a version boundary.

### Version lines

| Line | Purpose | Policy |
|---|---|---|
| **v4.4.x** | Maintenance for production users | Bug fixes only. No new features, no refactors. Numerical output must remain bit-exact to v4.4.0. |
| **v5.0** (in development on `main`) | Modernization target | All Phase 1–6 work lands here. API and ABI changes allowed at the major-version boundary. |
| **v5.x** | Post-5.0 evolution | Feature work, incremental improvements, additional Phase 6 items. |

**v4.4.0 represents the production baseline.** Every phase of v5.0 work is validated by running the same example corpus against v4.4.0 and comparing outputs under the tolerance policy in §1.

### Branch model

```
main                         ← v5.0 development (all phase work lands here)
release/4.4                  ← maintenance branch for v4.4.x bug fixes only
feat/<phase>-<slug>          ← short-lived feature branches off main
hotfix/4.4.x-<slug>          ← short-lived fix branches off release/4.4
```

Cherry-pick between lines explicitly — never merge `main` into `release/4.4`.

### Completed work (on `main`, not yet in a tagged release)

- [x] **Phase 5 — error handling (v4.4.1)**: `hzd_Error` / `hzfxit` capture `__FILE__`/`__LINE__`/`__func__` via macro wrapping; legacy cryptic codes (DLG1935, DTRSFM1150, HZPM, HZPO*) get human descriptions via `hzd_error_describe()`; enriched catch handler in `hazard.c` prints origin + description. `ErrorPassBack` / `ErrorReturn` / `ErrorReset` macros still present — Phase 5.1 completes their replacement.
- [x] **Logger foundation (v4.4.1)**: `HZD_LOG_{DEBUG,INFO,WARN,ERROR}` in `src/common/hzd_log.{h,c}`, runtime level via `HZD_LOG_LEVEL` env. Removed 6 per-file `#define NDEBUG` hijacks that collided with `<assert.h>` semantics.
- [x] **Phase 7 partial — build/CI**: `--enable-warnings` / `--enable-sanitizers` configure flags, GitHub Actions CI with Unit / Memory-Safety / Full-Build / Coverage / Windows-MinGW jobs, Codecov patch + project gates, version-badge verification.
- [x] **Test harness**: 168-assertion unit suite across 5 binaries; integration scripts V1 (numerical), V2 (convergence — see caveat §1), V3 (stepwise), V6 (ASan), V7 (concurrent).

### v5.0 candidate roadmap (phases mapped to releases)

| Phase | Land in | Blocks which phase |
|---|---|---|
| 5.1 — complete error-handling migration | v5.0-alpha | — |
| 1 — context struct (god-object split) | v5.0-alpha | 2, 3 |
| 4 — type system (logical, const, enum errors) | v5.0-alpha (parallel) | — |
| 2 — numerical core (llike + optim) | v5.0-beta | 3 |
| 3 — vars/ stepwise | v5.0-beta | — |
| 6 — parser/lexer re-entrant | v5.0 or v5.1 | independent |
| 7 — remaining build cleanup | rolling in v4.4.x + v5.0 | — |

No phase merges to `main` until it passes the gate battery in §1.

### Pilot & rollout for v5.0

1. **v5.0-alpha** — all phases compile; unit tests + V1 + V3 green on `main`.
2. **v5.0-beta** — full example `.sas` corpus matches v4.4.0 reference outputs to 8 d.p. log-likelihood, exact stepwise path, parameter estimates within tolerance (§1).
3. **v5.0-rc1** — named pilot dataset (internal: `hm.death.AVC`; external: TBD) reproduces v4.4.0 output bit-exact OR within documented tolerance for any Phase 2 precision changes.
4. **v5.0 GA** — release notes, migration guide for binary/API consumers, `release/4.4` enters security-fix-only mode.

---

## 1. Testing Strategy & Gates

Everything the phase work touches is load-bearing. The gate battery is the wall between a phase merging and staying on a branch. Gates are ordered — a phase can't attempt gate N until gate N-1 passes.

### The validator scripts

Real scripts, not pseudo-code — all under `tests/`:

| ID | Script | What it checks |
|---|---|---|
| V1 | `tests/integration/validate_numerical.sh` | Bit-exact regression vs. `tests/4.1.0/`: log-likelihood to 3 d.p., events conserved, key parameter estimates, convergence message |
| V2 | `tests/integration/validate_convergence.sh` | Optimizer convergence: LL to 8 d.p., exact `nfncts`, exact `iterations`, conditioning number |
| V3 | `tests/integration/validate_stepwise.sh` | Stepwise variable-selection path — order-sensitive string match of step sequence |
| V6 | `tests/integration/validate_memory.sh` | ASan + UBSan run-to-completion on reference data |
| V7 | `tests/integration/validate_concurrent.sh` | Two independent runs simultaneously + 3-way stress, determinism check |
| Unit | `tests/unit/make check` | 168 assertions across `test_math_helpers`, `test_shaping_functions`, `test_obs_layout`, `test_error_describe`, `test_log` |
| Master | `tests/run_all_tests.sh` | Orchestrates all of the above |

**Known blocker — V2 has a parser bug.** The `validate_convergence.sh` comparison script currently flags identical values (`got=-210.501 want=-210.501`) as mismatches. V2 cannot be a trusted gate until this is fixed. **Fixing V2 is blocking work for Phase 2.** Track as `hotfix/4.4.x-v2-parser` and cherry-pick to both lines.

### Per-PR gate (already enforced via CI)

Every PR against `main` or `release/4.4` must pass:

- Unit Tests (gcc)
- Memory Safety (ASan + UBSan)
- Full Build + Integration Tests (V1, V3, V7 — V2 and V6 aspirational until fixed)
- Coverage Report + Codecov patch ≥ configured threshold
- Windows Native Build (MSYS2/MinGW)

These run on every push and are the minimum bar. A green CI does **not** mean a phase is ready to merge — it means the PR hasn't regressed below the baseline.

### Per-phase merge gate

Before a phase branch merges to `main`, additionally:

- [ ] V1 + V3 green on the full `tests/*.sas` example corpus (20 files), not just the four currently in the script
- [ ] V2 green (once parser is fixed) to 8 d.p. LL across the full corpus
- [ ] ASan + UBSan clean on the full corpus under `--enable-sanitizers`
- [ ] No new uncovered lines in touched files (Codecov patch ≥ 80%)
- [ ] Build + tests pass on Linux-gcc, Linux-clang, macOS-clang, Windows-MinGW
- [ ] `docs/Claude_MODERNIZATION_GUIDE.md` progress log (§7) updated with the merge SHA range and validation results

### Per-v5.0-RC gate

Before tagging a release candidate:

- [ ] Everything above, plus:
- [ ] Performance regression ≤ 5% vs. v4.4.0 on the reference corpus (measure wall-time per run)
- [ ] Binary size within ±10% of v4.4.0
- [ ] Pilot dataset (see §0) reproduces v4.4.0 output within documented tolerance
- [ ] Release notes drafted, migration guide drafted
- [ ] `release/4.4` branch cut from the v4.4.1 (or latest 4.4.x) tag

### Tolerance policy for v5.0 vs v4.4.0

Per-phase tolerance when comparing v5.0 output to v4.4.0 baseline:

| What | Tolerance | Rationale |
|---|---|---|
| Log-likelihood | Bit-exact (all phases unless overridden) | The optimizer should converge to the same point |
| Parameter estimates | 8 d.p. | Matches the output format's own precision |
| `nfncts`, `iterations` | Exact | Any change = different optimizer path = investigate |
| Conditioning number (`condno`) | 4 d.p. | Small rearrangements in matrix ops can perturb this |
| Stepwise path | Exact sequence | Path is order-sensitive; any drift is a real regression |
| Wall-clock time | ≤ 5% slowdown | No unintended perf cliff |

**Phase 2 precision upgrade (B1 — `xtended` → `long double`) is the exception.** If we decide to take it, v5.0 will document numerical drift and ship a tolerance table in release notes. **Default decision: stay with `double` for bit-compat. Revisit only if a concrete precision bug motivates it.**

### Test corpus expansion (work item)

Today's `validate_*.sh` scripts compare against four hand-picked examples. Before v5.0-beta we need reference outputs for every `.sas` under `tests/` (currently 20 files). Add `tests/4.4.0/` as the new reference set, populated by running v4.4.x against each example.

---

## 2. Migration Phases

> Ordered by risk. The global struct pattern and numerical core are the load-bearing walls — touch them wrong and the math silently breaks. Parser and build modernization are comparatively safe. **Do not reorder phases 1 → 3.** Phases 4, 6, and 7 are independent and can parallelize.

### Phase 5 — Error Handling ✅ (v4.4.1 partial; 5.1 completes)

Originally planned as higher risk; de-risked by landing early so we had the error-context plumbing in place before touching anything else.

Done in v4.4.1:
- [x] **5.a** `hzd_Error(msg, n)` macro-wraps capture of `__FILE__`/`__LINE__`/`__func__`
- [x] **5.b** `struct commn` extended with `errfile`/`errfunc`/`errline`; zeroed on init
- [x] **5.c** Top-level catch handler in `hazard.c` prints origin + human description
- [x] **5.d** `hzd_error_describe()` table maps 39 legacy codes

Remaining for **Phase 5.1** (v5.0-alpha):
- [ ] **5.1** Standardize error return: computation functions return `int` (0 = success, nonzero = error code), output via out-parameter
- [ ] **5.2** Audit `setjmp`/`longjmp` usage — with origin context captured we can now decide whether the longjmp flow survives or gets replaced
- [ ] **5.3** Replace `ErrorPassBack`, `ErrorReturn`, `ErrorReset` macros with explicit function calls
- [ ] **5.4** Promote `hzd_error_describe()` into a proper `struct hazard_error { int code; char message[256]; const char *file; int line; const char *func; }`

### Phase 1 — Foundation: Context Struct (HIGHEST RISK)

The global `HZRstr`, `Common`, `G`, `Early`, `Late`, `Constant` structs are the most dangerous. Every function touches them implicitly. This phase creates the scaffolding everything else depends on.

- [ ] **1.1** Audit all global structs: `HZRstr` (74 fields), `Common`, `G`, `Early`, `Late`, `Constant`, `Setcoe`, `HazG1` — document every field and which modules write to it
- [ ] **1.2** Create `struct hazard_context` that wraps all globals into a single owned context (see §3 Pattern A)
- [ ] **1.3** Add a `hazard_context_init()` / `hazard_context_free()` pair that replaces the scattered global initialization in `hzri()` / `HZ4I()`
- [ ] **1.4** Transitional compatibility shim: `#define HZRstr (ctx->hzr)` so existing code compiles during migration. Remove at end of Phase 1.
- [ ] **1.5** Validate: V1 + V3 bit-exact, V2 exact, V6 clean — before Phase 2 starts

### Phase 2 — Numerical Core: llike + optim (HIGH RISK)

The log-likelihood and optimizer are the mathematical heart. Bugs here produce wrong numbers, not crashes — the worst kind.

- [ ] **2.1** Add parameter signatures to `dllike()`, `setcoe()`, `setobj()`: pass `hazard_context *ctx` explicitly
- [ ] **2.2** Replace `Common.obs[G.Im1p7+N]` magic offsets with `struct obs_row { double time, c1, c2, c3, ..., weight; }` and a typed accessor (see §3 Pattern B)
- [ ] **2.3** Refactor `dnmr2.c` FORTRAN state machine (`next` variable): extract to explicit sub-functions named for each case label
- [ ] **2.4** Lift `static double lamprv, fpprv` in `linesr.c` into `struct optimizer_state` passed as parameter (see B2)
- [ ] **2.5** Replace `QEXTD`/`QTOD`/`QSUM` macros with inline functions with descriptive names; fix `QSUM` operator-precedence bug (B4)
- [ ] **2.6** Decide B1 (precision): default is to keep `double` for bit-compat; revisit if a concrete precision bug emerges
- [ ] **2.7** Validate: V1 + V2 + V3 bit-exact against v4.4.0 across the full corpus (not just 4 examples)

### Phase 3 — Variable Selection: vars/ (MEDIUM RISK)

`STEPW()` has 14 parameters AND uses globals — a confused API. State flows via `HZRstr.hpsn` and `HZRstr.swadel` set as side effects.

- [ ] **3.1** Create `struct stepwise_result { int swadel; int hpsn; logical var_excluded; logical var_included; }` as return value
- [ ] **3.2** Change `void STEPW(...)` → `struct stepwise_result STEPW(struct hazard_context *ctx, ...)`
- [ ] **3.3** Replace 14-parameter flat signature with grouped parameter structs
- [ ] **3.4** Make the `SWVARX → SWVARQ → SWVARI` control flow explicit via return codes, not via global `hpsn`
- [ ] **3.5** Validate: V3 exact sequence match against reference

### Phase 4 — Type System (MEDIUM RISK, parallelizable)

Safe to do incrementally. Each change is small; aggregate effect is large.

- [ ] **4.1** Replace `typedef short int logical` with `#include <stdbool.h>` and `typedef bool logical` (C99) — keep `logical` name to avoid mass rename
- [ ] **4.2** Add `const` qualifiers to all read-only pointer parameters across the codebase
- [ ] **4.3** Replace `typedef char charVarName[8]` with bounded-string helpers using `snprintf` (retain 8-char SAS name limit as `SAS_NAME_MAX`; see B6)
- [ ] **4.4** Replace magic error strings with an enum `HAZARD_ERR_*` — builds on Phase 5 describe-table
- [ ] **4.5** Replace `CLEAR(x)` with zero-initialized structs or explicit `memset(p, 0, ...)` (watch B3 — CLEAR writes spaces not zeros)

### Phase 6 — Parser/Lexer (LOW RISK, isolated)

The Lex/Yacc parsers are already isolated. The risk here is regression in option parsing, not math.

- [ ] **6.1** Upgrade to re-entrant Flex (`%option reentrant`) and Bison (`%define api.pure full`) — eliminates `yytext`/`yylval` globals
- [ ] **6.2** Add `--` prefix check to all yacc tokens to future-proof option names without breaking existing syntax
- [ ] **6.3** Wrap scanner state in `struct parser_context { yyscan_t scanner; hazard_context *ctx; }`
- [ ] **6.4** Validate V5: every `.sas` under `examples/` and `tests/` parses without error; compare token streams against the old parser

### Phase 7 — Build System (LOW RISK, partial ✅)

Done:
- [x] **7.2** `--enable-warnings` (`-Wall -Wextra -Wno-unused-parameter`) via configure flag
- [x] **7.3** `--enable-sanitizers` (`-fsanitize=address,undefined`) via configure flag
- [x] **7.5** CI on GitHub Actions covering unit + integration + memory + Windows + coverage

Remaining:
- [ ] **7.1** Audit `configure.ac`: remove `AC_AIX`, `AC_ISC_POSIX`, `AC_MINIX` if still referenced
- [ ] **7.4** Evaluate a parallel CMake build alongside Autotools — do not remove Autotools until CMake is proven equivalent

---

## 3. Code Transformations for Common Patterns

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
#define QSUM(x,y)   (x)+(y)     // macro — no type safety, missing parens!
```

**After:**
```c
// extended_precision.h
typedef double xtended;   // default — keep for bit-compat with v4.4.0 reference
// If B1 precision upgrade is adopted in v5.x:
// typedef long double xtended;

static inline xtended extended_from_double(double x)     { return (xtended)x; }
static inline double   double_from_extended(xtended x)    { return (double)x;  }
static inline xtended  extended_add(xtended a, xtended b) { return a + b;      }
```

---

### Pattern H: Error handling (already shipped in v4.4.1 for raise path; call-site migration remaining)

**Before (v4.4.0):**
```c
Common.errorno = 0;
CLEAR(Common.errflg);
hzd_theta_to_parms(...);
if (Common.errorno) return;
```

**After v4.4.1 (raise path — shipped):**
```c
hzd_Error("DLG1935", 1);   // macro captures __FILE__/__LINE__/__func__
```

**Target v5.0 (return-based propagation):**
```c
int err = hzd_theta_to_parms(ctx, ...);
if (err) return err;
```

---

## 4. Breaking Changes to Watch For

These are changes that appear safe but will silently break results or ABI. Each is annotated with the phase that's most likely to encounter it.

### B1 — Precision change in `xtended` (Phase 2)
If you change `typedef double xtended` to `long double`, accumulated log-likelihoods will differ slightly across platforms. The optimizer may converge to different (arguably more correct) solutions. Output values for parameter estimates will not be bit-for-bit identical to the v4.4.0 reference outputs. **Default decision for v5.0: keep `double`. Revisit only if a concrete precision bug motivates it.** If adopted, ship a tolerance table in release notes.

### B2 — `static` locals in `linesr.c` (Phase 2.4)
`lamprv` and `fpprv` are zero-initialized once at program start via C static semantics. If moved to a `struct`, you must explicitly zero-initialize on first use. Failure to do so causes the line search to start from a garbage state — optimizer diverges silently.

### B3 — `CLEAR` macro uses spaces, not zeros (Phase 4.5)
`CLEAR(x)` does `memset(p, ' ', sizeof(p))` — it fills with ASCII 0x20, not 0x00. If you replace with `memset(p, 0, ...)` you change the content of error message buffers. This will not affect computation but may change error message formatting in output.

### B4 — `QSUM` macro operator precedence (Phase 2.5)
```c
#define QSUM(x,y) (x)+(y)
```
This macro lacks outer parentheses. `a * QSUM(b, c)` expands to `a * (b) + (c)` — multiplication only applies to `b`. Fixing this (adding outer parens) changes evaluation order for any caller that composes `QSUM` inside larger expressions. Audit all callers before fixing; the fix itself may perturb numerical output.

### B5 — Parallel duplication of `Early` and `HZRstr.e` (Phase 1)
`struct early Early` (global) and `HZRstr.e` (field) appear to be the same data in two places. Before migrating either, determine which one `SETG1` and `SETG3` write to. If both are written and read independently, they may be intentional (working copy vs. committed copy) — merging them will break the optimization rollback mechanism.

### B6 — `charVarName[8]` is a SAS protocol limit, not an implementation choice (Phase 4.3)
SAS variable names are hard-limited to 8 characters in XPORT format (v5 transport). If you replace the fixed array with dynamic strings, you must still enforce the 8-char limit at the XPORT read/write boundary or you will produce files that SAS cannot read.

### B7 — Re-entrant Flex/Bison changes the call convention (Phase 6.1)
Upgrading to `%option reentrant` in Flex changes `yylex()` to `yylex(yyscan_t scanner)`. Any code calling `yylex()` directly (outside the generated grammar) must be updated. Search for direct `yylex` calls before flipping this flag.

### B8 — Parser token overlap (Phase 6.2)
The yacc grammar uses token names like `PROC`, `DATA`, `BY` that overlap with SAS reserved words. If new tokens are added to support new syntax, they must not shadow existing terminal symbols or parsing of old programs will silently change behaviour.

### B9 — Error struct extension breaks ABI (Phase 1 + already landed in v4.4.1)
The v4.4.1 work added `errfile`/`errfunc`/`errline` to `struct commn`. Any consumer that serializes or passes this struct across a library boundary will observe the size change. Since HAZARD links `libcommon.a` statically and does not currently expose `struct commn` via a public C API, this is internal only — but flag it if Phase 1 changes layering.

---

## 5. Rollback Plan

### Prerequisite: establish a known-good baseline

```bash
# Tag v4.4.0 as the production baseline (done)
git tag v4.4.0 <commit-sha>

# Capture reference outputs from all test programs against v4.4.0
# (expand tests/4.1.0 into tests/4.4.0 as the new reference corpus — work item)
make clean && ./configure && make
cd tests/
./run_all_tests.sh > /tmp/reference_outputs.txt 2>&1
md5sum *.lst > /tmp/reference_md5sums.txt

# Commit reference outputs under tests/4.4.0/
git add tests/4.4.0/
git commit -m "Freeze v4.4.0 reference outputs"
```

### Per-phase rollback

Each phase branch merges via **merge commit** (not squash) so it can be reverted atomically.

```
main
 └── feat/phase-1/context-struct   ← branch, open PR, merge only after §1 gate passes
      └── feat/phase-2/llike-refactor
           └── feat/phase-3/vars-refactor
                └── ...
```

**To roll back a phase:**
```bash
git revert -m 1 <merge-commit-sha>   # Preferred: creates a revert commit, preserves history
# OR
git reset --hard <tag-before-phase>  # Nuclear: only if branch is not shared
```

### Transitional build flag

For Phase 1 specifically, the transitional compat shim (`#define HZRstr (ctx->hzr)`) allows a half-migrated tree to compile. Gate it on a configure flag:

```m4
# configure.ac
AC_ARG_ENABLE([context-struct],
  [AS_HELP_STRING([--enable-context-struct], [use new context struct (v5.0)])],
  [enable_context_struct=$enableval], [enable_context_struct=yes])
```

This allows shipping a binary with both implementations during Phase 1 bring-up.

---

## 6. Quick Reference: Risk Matrix

| Phase | Risk | Math Impact | Rollback Difficulty | Lands In |
|---|---|---|---|---|
| **5 — Error handling (raise)** | ~~Medium~~ **DONE** | None | — | v4.4.1 |
| **7 — Build system (CI/sanitizers)** | ~~Low~~ **DONE (partial)** | None | Very low | v4.4.1 |
| 5.1 — Error handling (return) | Medium | None | Low | v5.0-alpha |
| 1 — Context struct | High | None (mechanical) | Medium | v5.0-alpha |
| 4 — Type system | Medium | Minimal | Low | v5.0-alpha (parallel) |
| 2 — llike/optim | High | Yes (subtle bugs possible) | Low (git revert) | v5.0-beta |
| 3 — vars/ stepwise | Medium | Yes (path-dependent) | Low | v5.0-beta |
| 6 — Parser/Lexer | Low | None | Low | v5.0 or v5.1 |

**Recommended sequence:** 5 ✅ → 7 (partial) ✅ → 5.1 → 1 → 4 (parallel) → 2 → 3 → 6.

Phase 1 must complete before 2 or 3 start. Phase 6 is independent and can start any time after the test corpus is expanded. Phase 4 can run alongside 1, 2, or 3 since its changes are diff-local.

---

## 7. Progress Log

Append-only record of phase merges, validation results, and decisions. Update in the same PR that lands the work.

### 2026-04-18 — v4.4.1 (Phase 5 raise path + Phase 7 partial)

- **Branch**: `feat/logging-error-context` → merged to `main` as PR #17 (merge commit `8379d64`)
- **Commits**: `d24487d`..`8d7ea23`
- **What landed**:
  - `HZD_LOG_{DEBUG,INFO,WARN,ERROR}` logger in `src/common/hzd_log.{h,c}`, runtime level via `HZD_LOG_LEVEL`
  - `hzd_Error(msg, n)` and `hzfxit(type)` macro-wrapped to capture origin
  - `struct commn` gained `errfile`/`errfunc`/`errline`
  - `hzd_error_describe()` table for 39 legacy codes
  - Enriched catch handler in `hazard.c`
  - Unit tests: 168 assertions across 5 binaries (added `test_error_describe`, `test_log`)
- **Validation**:
  - V1 numerical regression: 13/13 PASS
  - V3 stepwise: 3/3 PASS
  - V2: pre-existing parser bug (flags identical values as mismatches) — not a regression
  - Unit: 168/168 PASS
  - Codecov patch: passed after targeted coverage tests added
  - Windows MinGW: passed after `setenv`/`mkstemp` portability fixes
- **Decisions**:
  - Keep `double` as default for `xtended` (B1) pending concrete motivation to change
  - `release/4.4` branch to be cut when first v4.4.x hotfix is needed; not pre-emptively
- **Next**: fix V2 parser bug; expand `tests/4.4.0/` reference corpus; begin Phase 5.1 (return-based error propagation)
