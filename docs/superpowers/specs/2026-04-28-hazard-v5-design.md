---
title: Hazard v5.0 Design Spec
date: 2026-04-28
author: John Ehrlinger (with Claude as brainstorming partner)
status: approved (architectural intent); implementation plans pending
supersedes: portions of docs/Claude_MODERNIZATION_GUIDE.md (see §8.2)
related:
  - docs/Claude_MODERNIZATION_GUIDE.md (active implementation tracker)
  - docs/v5-design-input/2026-04-28-silent-failure-postmortem.md (postmortem driving §7)
  - ../../../ObsidianVault/Projects/HAZARD.md (v5.0 candidate notes, 2026-04-28)
  - ../../../ObsidianVault/Projects/SAS to R Migration.md (Parquet landscape, 2026-04-28)
---

# Hazard v5.0 — Architecture & Release Design Spec

## Executive Summary

v5.0 modernizes hazard along four orthogonal axes, sequenced through three maintenance releases (v4.4.5–v4.4.7) on the `release/4.4` line and three v5.0 milestones (alpha → beta → GA) on `dev/v5.0-alpha` → `main`, followed by v5.1 for the generalized N-phase model.

**Four axes of change:**

1. **I/O modernization** — Parquet (via Arrow C++) becomes the primary input/output format. XPORT V5 stays as the legacy reader, frozen. XPORT V8 is rejected with a structured error pointing at Parquet.
2. **Internal modernization** — Phase 1–6 from the existing modernization guide carry forward: context struct, type system, numerical core cleanup, stepwise refactor, parser re-entrancy. Done in modern C (C17), not C++.
3. **C++ at the boundaries** — Parquet I/O and a new Python frontend (pybind11) are written in C++17. The numerical core stays C.
4. **Build system migration** — Autotools → CMake on the v5.0 line. `release/4.4` keeps autotools (untouched).

**Hard constraint:** the existing `PROC HAZARD` call structure remains stable. Existing `.sas` scripts run unchanged on v5.0. New behavior is additive (new options like `OUT_FORMAT=PARQUET`); old options keep their semantics bit-exact.

**Two engines, one data contract:** `hazard` (C, modernized) and `TemporalHazard` (R, peer pure-R implementation, on track for CRAN) are fully independent code paths. They share only Parquet as the file format. CI cross-validates them via a new V9 gate — same Parquet input, both engines, diff outputs against a documented inter-engine tolerance.

**v5.1 (post-GA)** adds Phase G — port of TemporalHazard's generalized N-phase parametric model to C — and removes Autotools entirely.

---

## §1 — System Architecture & Target Shape

### 1.1 Component model

Two fully independent engines, one shared file format:

```
   SAS users              R users              Python users (NEW in v5.0)
       │                     │                          │
       ▼                     ▼                          ▼
 ┌──────────────┐    ┌──────────────────┐    ┌────────────────────┐
 │  PROC HAZARD │    │  TemporalHazard  │    │  python -m hazard  │
 │  (.sas)      │    │  (R package,     │    │  (pybind11)        │
 └──────┬───────┘    │   pure R, CRAN)  │    └─────────┬──────────┘
        │            └──────────┬───────┘              │
        ▼                       │                      ▼
 ┌──────────────────┐           │           ┌──────────────────┐
 │  hazard (C)      │           │           │  hazard (C)      │
 │  binary          │           │           │  via libhazard   │
 │  libhazard.a     │           │           │  + libhazard_io  │
 └──────┬───────────┘           │           └─────────┬────────┘
        │                       │                     │
        ▼                       ▼                     ▼
   ┌──────────────────────────────────────────────────────────┐
   │   FILE FORMAT (shared substrate, not shared runtime)     │
   │   - Parquet (v5.0+, primary)                             │
   │   - XPORT V5 (legacy, frozen, hazard only)               │
   │   - XPORT V8 → rejected with migration message           │
   └──────────────────────────────────────────────────────────┘

           ┌─────────────────────────────────────────────┐
           │  CI cross-validation gate (parity test only)│
           │  Same .parquet → run through hazard AND     │
           │  TemporalHazard independently → diff outputs│
           │  against documented tolerance               │
           └─────────────────────────────────────────────┘
```

**Critical clarification:** TemporalHazard is **not** invoked by SAS, **not** a wrapper around `libhazard`, **not** a runtime dependency of hazard. It is a peer pure-R implementation. Cross-validation is a CI parity test, not a code coupling.

### 1.2 Layer responsibilities (the `hazard` C/C++ side; TemporalHazard out of v5.0 scope as code dependency)

| Layer | Language | Build | Purpose |
|---|---|---|---|
| `src/common/`, `src/hazard/`, `src/hazpred/`, `src/llike/`, `src/optim/`, `src/model/`, `src/vars/`, `src/stat/`, `src/vcov/`, `src/utils/` | Modern C (C17) | CMake | Numerical core. Phase 1 context-struct refactor. Validated bit-exact against v4.4.x. |
| `src/blackbox/` | C (frozen) | CMake | Legacy XPORT V5 reader. v4.4.6 adds version-banner check; rejects V8. No new development. |
| `src/parquet/` (NEW) | C++17 + Arrow C++ | CMake | Parquet read/write. Magic-byte input format detection. Metadata convention (`SAS.label` → `hazard.label` fallback). |
| `src/python/` (NEW) | C++17 + pybind11 | CMake | Python frontend. Wheel via PEP 517. |
| `macros/` | SAS | (data) | User-facing analysis macros. Unchanged in v5.0. |
| `src/scripts/` (`hazard.sas`, `hazpred.sas`, `bootstrap.hazard.sas`) | SAS | (data) | PROC drivers + bootstrap. **Updated in v4.4.7** for binary↔SAS protocol (sentinel + exit codes). |

### 1.3 Key invariants

1. **PROC HAZARD call structure is stable.** Existing `.sas` scripts run unchanged on v5.0. New options are additive only.
2. **TemporalHazard is fully independent.** Not invoked by SAS. Not a wrapper. Not a runtime dependency. v5.0 of `hazard` does not modify TemporalHazard.
3. **Library-first packaging.** `libhazard` is a versioned, reusable static library. The C `hazard` binary and the Python frontend both link it. Future reuse (e.g., selective `cpp11`/`Rcpp` hooks from TemporalHazard for profiled hot loops) is an opportunity, not a v5.0 commitment.
4. **One file format, two engines.** Parquet is the cross-validation substrate. Both engines independently read the same file; CI compares outputs. Parity gate, not coupling.
5. **Phase 1's context struct anticipates N-phase generalization.** v5.0 hardcodes `N_PHASES = 3` for backward-compatible math, but uses an array layout (`phases[N_PHASES_MAX]`) rather than separate `Early`/`Late`/`Constant` fields. This makes Phase G in v5.1 incremental rather than another structural refactor.

---

## §2 — Components & libhazard API Surface

### 2.1 Library structure

```
┌───────────────────────────────────────────────────────────────┐
│  libhazard (static, C17)                                      │
│  ├── numerical core (llike, optim, model, vars, vcov, stat)   │
│  ├── context struct + lifecycle                               │
│  ├── XPORT V5 reader (frozen, in-tree, src/blackbox/)         │
│  └── public C API: hazard.h                                   │
└────────────────┬──────────────────────────────────────────────┘
                 │ depended on by:
                 ▼
┌───────────────────────────────────────────────────────────────┐
│  libhazard_io (static, C++17)                                 │
│  ├── Parquet reader (Arrow C++)                               │
│  ├── Parquet writer (Arrow C++)                               │
│  ├── format auto-detect (magic-byte sniff)                    │
│  ├── metadata convention (SAS.label → hazard.label fallback)  │
│  └── extern "C" bridge for hazard.h consumers                 │
└────────────────┬──────────────────────────────────────────────┘
                 │ depended on by:
                 ▼
       ┌──────────────────┐    ┌──────────────────────┐
       │  hazard binary   │    │  Python frontend     │
       │  (C, link both)  │    │  (pybind11, C++17)   │
       └──────────────────┘    └──────────────────────┘
```

### 2.2 Public C API of `libhazard` (sketch — final API negotiated during Phase 1)

```c
/* hazard.h — opaque handles, paired create/destroy, error returns */

typedef struct hazard_context hazard_context_t;
typedef struct hazard_options hazard_options_t;
typedef struct hazard_dataset hazard_dataset_t;
typedef struct hazard_result  hazard_result_t;

/* Lifecycle */
hazard_context_t *hazard_context_create(const hazard_options_t *opts);
void              hazard_context_destroy(hazard_context_t *ctx);

/* Input — dataset is produced by an I/O layer (libhazard XPORT, libhazard_io Parquet, or test fixture) */
int  hazard_context_set_dataset(hazard_context_t *ctx, hazard_dataset_t *ds);
void hazard_dataset_destroy(hazard_dataset_t *ds);

/* Run */
int hazard_run(hazard_context_t *ctx, hazard_result_t **out);

/* Result inspection */
double hazard_result_loglik     (const hazard_result_t *r);
int    hazard_result_iterations (const hazard_result_t *r);
int    hazard_result_nfncts     (const hazard_result_t *r);
size_t hazard_result_n_phases   (const hazard_result_t *r);  /* future-proofs Phase G */
/* ... full accessor surface defined during Phase 1 */
void   hazard_result_destroy    (hazard_result_t *r);

/* Error handling — return-based per Phase 5.1 */
const char *hazard_error_describe(int err_code);
```

### 2.3 Design choices

1. **Opaque handles everywhere.** No public struct layouts in headers. ABI stays decoupled from internal data layout. Future internal refactors (Phase G) are non-breaking for binary consumers.
2. **`hazard_dataset_t` is the format-agnostic I/O abstraction.** XPORT V5 and Parquet readers both produce this opaque type. The numerical core has zero knowledge of input format.
3. **`extern "C"` bridge from `libhazard_io`.** The C++ Parquet layer exposes a C-ABI surface so the C binary doesn't have to be C++. pybind11 uses the C++ side directly.
4. **`hazard_result_n_phases()` is in the v5.0 public API** even though n=3 is hardcoded. Cheap insurance for Phase G.

### 2.4 Build-time dependency graph

```
external deps:
  Arrow C++ (parquet, arrow)     — required for libhazard_io
  pybind11                       — required for Python frontend only
  libm                           — C standard math, as today

internal:
  hazard binary       → libhazard, libhazard_io
  Python frontend     → libhazard, libhazard_io, pybind11
  TemporalHazard      → (no compiled deps — pure R; v5.0 spec doesn't touch it)
```

---

## §3 — Data Flow & I/O Contract

### 3.1 Format auto-detection

```c
/* libhazard_io: sniff first 80 bytes, dispatch */
hazard_dataset_t *hazard_io_read_auto(const char *path) {
    /*   "PAR1"           → Parquet (any version)                    */
    /*   "HEADER RECORD"  → XPORT — read header #2 to detect version */
    /*       header #2 contains "LIBRARY"  → XPORT V5 (supported)    */
    /*       header #2 contains "LIBV8"    → XPORT V8 (rejected)     */
    /*   anything else    → error: "unrecognized input format"       */
}
```

Users do **not** specify input format. Detection is automatic.

### 3.2 Parquet input schema

| Field | Required | Type | Notes |
|---|---|---|---|
| Time-to-event column | yes | `float64` | Column name configured via `TIME=` PROC option. |
| Event indicator | yes | `int8` / `bool` | `EVENT=`. |
| Risk variables | ≥1 | `float64` (continuous), `int8`/`bool` (binary) | `RISK=`. UTF-8 names; no length limit. |
| Weight | optional | `float64` | `WEIGHT=`. |
| Stratum | optional | `int32` / `string` | `STRATA=` (if applicable). |

### 3.3 Parquet metadata convention (column-level)

Hazard reads metadata keys in fallback order:
1. `SAS.label` (written by SAS `PROC EXPORT DBMS=PARQUET`)
2. `hazard.label` (written by hazard, R `arrow::write_parquet`, pandas, etc.)
3. (no label — column name used as label)

Same fallback for `SAS.format` / `hazard.format`, `SAS.informat` / `hazard.informat`, `SAS.length` / `hazard.length`. Hazard preserves these on round-trip; only `length` is *applied* (for storage sizing). `format`/`informat` are passed through, not interpreted.

Documented in **`docs/PARQUET-FORMAT.md`** (new in v5.0-alpha).

### 3.4 Output formats — backward-compatible by default

| Option | v4.4.x behavior | v5.0 default | v5.0 opt-in |
|---|---|---|---|
| Text `.lst` listing | always written | always written (unchanged) | — |
| Results dataset | XPORT V5 `.dta` | XPORT V5 `.dta` (unchanged) | Parquet via `OUT_FORMAT=PARQUET` |
| Hazpred predictions | XPORT V5 `.haz` | XPORT V5 `.haz` (unchanged) | Parquet via `OUT_FORMAT=PARQUET` |

**v5.0 does not change default output behavior.** Existing `.sas` scripts that consume `.dta` / `.haz` files keep getting them. Default-Parquet is a v6.0 question.

### 3.5 Parquet output schema (when opt-in)

Three files per run, distinguished by Parquet metadata `hazard.payload`:

1. **`<job>.summary.parquet`** — single row: `loglik`, `iterations`, `nfncts`, `condno`, `convergence_status`, `n_phases`, `n_obs`, `n_events`, hazard version, run timestamp.
2. **`<job>.params.parquet`** — one row per parameter: `phase_index`, `variable_name`, `estimate`, `std_error`, `z`, `p_value`, `included` (bool, for stepwise).
3. **`<job>.predictions.parquet`** (hazpred only) — one row per (subject, time-point): `subject_id`, `time`, `hazard`, `cumulative_hazard`, `survival`, plus per-phase contributions if `DETAIL=` set.

### 3.6 The `hazard_dataset_t` internal shape (Phase 1–compatible)

```c
/* private — hidden behind the opaque handle */
struct hazard_dataset {
    size_t  n_obs;
    size_t  n_vars;
    char  **var_names;        /* heap-owned, UTF-8, unbounded length */
    char  **var_labels;       /* heap-owned, UTF-8, unbounded length, may be NULL */
    int    *var_types;        /* HAZARD_TYPE_F64 | HAZARD_TYPE_I8 | ... */
    double *obs_data;         /* row-major, n_obs × n_vars (numeric only) */
    /* ... per-variable metadata: format strings, lengths, etc. */
};
```

XPORT V5 reader and Parquet reader both produce this struct. Numerical core sees only the struct, never the source format. **The 8-char and 40-char limits exist nowhere in this struct** — enforced at the XPORT V5 boundary on the way in, lifted everywhere else.

### 3.7 Round-trip & format catalog story

- Round-trip Parquet → hazard → Parquet: lossless.
- Round-trip XPORT V5 → hazard → XPORT V5: lossless within V5 limits.
- XPORT V5 → hazard → Parquet: names and labels expanded to full UTF-8 on output; users wanting long names re-export from SAS source via `PROC EXPORT DBMS=PARQUET`.
- **`PROC FORMAT` catalogs:** v5.0 does not apply formats. Format metadata is preserved for downstream tools (SAS, R `haven`); hazard ignores it. Documented limitation. v5.1/v6.0 may add format application if user demand emerges — not in v5.0 scope.

---

## §4 — Release Plan & Phase Ordering

### 4.1 Release sequence

```
─────────────────────────────────────────────────────────────────
v4.4.5 — shipping now (release/4.4, autotools)
─────────────────────────────────────────────────────────────────
SCOPE
  • Edwards (Windows + SAS) reference capture committed
  • Linux + SAS reference bucket flagged as known gap (closes in v4.4.7)
  • Documentation updates, no new functional code

─────────────────────────────────────────────────────────────────
v4.4.6 — Hardened startup validation (release/4.4, autotools)
─────────────────────────────────────────────────────────────────
SCOPE
  • XPORT V8 detection + rejection at I/O boundary
  • Always-on stderr diagnostics — NOPRINT silences listing, NOT errors
  • Structured stdout error markers: HAZARD_ERROR_<code> <message>
  • Non-zero exit codes per existing hzfxit convention (15–20 range
    for input-format issues; 0 reserved for clean success)
  • User-base survey kicked off post-release
ACTIONS OUTSIDE CODE
  • Survey V8 users → "are you using LIBNAME XPORT V=8 anywhere?"
  • Survey V5-truncation users → "do you see SAS WARNING about name
    truncation in your pipeline logs?"
GATE
  • V1 + V3 unchanged
  • New unit tests: V8 banner detection, NOPRINT-bypass-for-stderr,
    exit code matrix, structured error markers
EFFORT: ~150 LoC binary side + 30 LoC tests.

─────────────────────────────────────────────────────────────────
v4.4.7 — Binary↔SAS protocol completion (release/4.4, autotools)
─────────────────────────────────────────────────────────────────
SCOPE
  • Sentinel file protocol: hazard writes
    $TMPDIR/hzr.<jobid>.<jobix>.ok on success containing
    {exit_code, version, n_obs_processed, n_vars_seen}
  • SAS macro updates (hazard.sas, hazpred.sas, bootstrap.hazard.sas):
    1. Check &SYSRC first — non-zero ⇒ %abort, no retry
    2. Check sentinel file second — missing ⇒ %abort, no retry
    3. Parse .lst third (and only if both above passed)
  • Removes retry-on-parse-failure loop entirely
  • Documents the binary↔SAS protocol in docs/BINARY-SAS-PROTOCOL.md
GATE
  • V1 + V3 + V8 (acceptance) unchanged on Windows + SAS
  • Linux + SAS reference capture UNBLOCKED — runs cleanly; corpus
    committed to tests/corpus/xport_v5/reference/v4.4.7-linux-gcc/
  • Failure injection tests: bad XPORT, missing variable, segfault —
    each produces non-zero rc + structured error + no retry storm
EFFORT: ~200 LoC binary side + 100 LoC SAS macro changes + tests.

─────────────────────────────────────────────────────────────────
v5.0-alpha — Parquet IN + foundation refactor (dev/v5.0-alpha)
─────────────────────────────────────────────────────────────────
SCOPE
  • Build system: CMake replaces autotools on dev/v5.0-alpha
    (release/4.4 stays on autotools — untouched)
  • libhazard_io (C++17 + Arrow C++): Parquet READ
  • Format auto-detection (magic-byte sniff)
  • Variable-name validation at startup (uses v4.4.7 protocol)
  • Metadata convention documented in docs/PARQUET-FORMAT.md
  • Phase 5.1: error-return migration
  • Phase 1: context struct (with N-phase-anticipating phases[] layout)
  • Phase 4: type system cleanup — parallel to Phase 1
GATE
  • V1 + V3 bit-exact on FULL corpus, both XPORT V5 and Parquet inputs
  • V2 + V6 (memory) clean on full corpus
  • All unit tests green on Linux gcc, Linux clang, macOS clang, Windows MinGW
  • libhazard public C API frozen (header file checked into docs/)
DECISION POINT (end of alpha)
  • XPORT V8 support: revisit based on v4.4.6 survey results.
    If >25% of surveyed users on V8 → add V8 reader before beta.
    Otherwise → confirm rejection, proceed.

─────────────────────────────────────────────────────────────────
v5.0-beta — Numerical core + Parquet OUT + Python
─────────────────────────────────────────────────────────────────
SCOPE
  • Phase 2: numerical core (llike + optim) — uses cleaned context struct
  • libhazard_io: Parquet WRITE (results, params, predictions)
  • OUT_FORMAT=PARQUET PROC option (extension; default unchanged)
  • Python frontend (src/python/, pybind11) — wheel-buildable
  • Cross-validation gate ONLINE (V9, report-only initially — see §6)
GATE
  • V1 + V2 + V3 bit-exact on FULL corpus, all input formats
  • V6 + V7 clean
  • V9 cross-validation: hazard vs TemporalHazard parity within
    documented tolerance on Parquet corpus (report-only)
  • Python wheel installs and runs reference example end-to-end

─────────────────────────────────────────────────────────────────
v5.0-rc → v5.0-GA — Stepwise + parser + autotools deprecation
─────────────────────────────────────────────────────────────────
SCOPE
  • Phase 3: vars/stepwise refactor
  • Phase 6: parser/lexer re-entrant (Flex %option reentrant, Bison %define api.pure)
  • Autotools build officially deprecated: CI runs CMake as primary,
    autotools as secondary
  • Migration guide for binary/API consumers (docs/MIGRATING.md)
  • V9 hard-fails (no longer report-only) on whatever subset
    TemporalHazard's framework has implemented; gaps documented
GATE
  • All §6 gates green
  • V9 cross-validation full corpus green (or documented exceptions)
  • Pilot dataset (TBD) reproduces v4.4.0 within tolerance
  • Performance ≤ 5% regression vs v4.4.0 wall-clock on reference corpus
  • Binary size within ±10% of v4.4.0
  • No P1 issues outstanding

─────────────────────────────────────────────────────────────────
v5.1 — Generalized N-phase model
─────────────────────────────────────────────────────────────────
SCOPE
  • Phase G: port TemporalHazard's generalized parametric phase form to C
  • PROC HAZARD: NPHASES= option (default 3 — preserves v5.0 behavior)
  • Autotools build REMOVED entirely
  • V9 cross-validation extended to N-phase test cases
GATE
  • All v5.0-GA gates remain green
  • V9 N-phase corpus: hazard vs TemporalHazard parity for n ∈ {2, 3, 4, 5}
  • Existing 3-phase scripts produce bit-exact v5.0 results
```

### 4.2 Phase dependency graph

```
v4.4.5 (shipping)
      │
      ▼
v4.4.6 (startup validation)
      │
      ▼
v4.4.7 (protocol completion + Linux SAS unblock)
      │
      ├─ (release/4.4 maintenance line continues with hotfixes only)
      │
      ▼
v5.0-alpha
      │
      ├─ CMake migration ──────┐
      ├─ Parquet READ ─────────┤
      ├─ Phase 5.1 ────────────┤   (parallel — different files)
      ├─ Phase 1 (ctx struct) ─┤
      └─ Phase 4 (types) ──────┤
                               │
                               ▼
                       v5.0-alpha tag
                               │
                               ├─ Phase 2 (numerical core) ───┐
                               ├─ Parquet WRITE ──────────────┤   (parallel)
                               └─ pybind11 frontend ──────────┤
                                                              ▼
                                                     v5.0-beta tag
                                                              │
                                                              ├─ Phase 3 (stepwise) ──┐
                                                              ├─ Phase 6 (parser) ────┤   (parallel)
                                                              └─ autotools deprecate ─┤
                                                                                     ▼
                                                                              v5.0-GA tag
                                                                                     │
                                                                                     ├─ Phase G (N-phase) ─┐
                                                                                     └─ autotools remove ──┤
                                                                                                          ▼
                                                                                                    v5.1 tag
```

### 4.3 Sequencing rules

1. Phase 1 must complete before Phases 2 or 3 start (existing rule).
2. Parquet READ can land before or alongside Phase 1; small throwaway adapter (~100 LoC) plumbs into legacy globals during alpha; Phase 1 then re-plumbs through `hazard_context`.
3. Parquet WRITE waits for Phase 1 (writes from `hazard_context`, not from globals — avoids throwaway).
4. CMake migration runs in parallel with Phase 1; both are mechanical refactors.
5. Phase 4 (types) parallelizes freely — diff-local.
6. Phase 6 (parser) is independent; lands in rc/GA for safety.
7. Phase G is post-v5.0-GA — relies on Phase 1's array-of-phases layout.

### 4.4 Within-v5.0-alpha sub-sequencing

```
1. CMake migration (build system only, math unchanged)
   → V1+V3 bit-exact against v4.4.4 → tag dev/v5.0-alpha.cmake-base
2. Parquet READ (libhazard_io)
   → V1+V3 bit-exact on Parquet inputs → tag dev/v5.0-alpha.parquet-read
3. Phase 5.1 (error returns) — independent
4. Phase 4 (types) — independent, parallel to 5.1
5. Phase 1 (context struct) — touches every file
   → V1+V3 bit-exact full corpus → tag v5.0-alpha
```

### 4.5 Decision deadlines (revisit gates)

| Gate | Decision | Default |
|---|---|---|
| End of v4.4.6 + 30 days | Survey results in; classify users V5/V8/Parquet-ready | — |
| End of v5.0-alpha | Add V8 reader? Yes/no based on survey | No (reject V8) |
| End of v5.0-beta | Default output format change? Parquet-by-default in v5.0 vs defer to v6.0 | Defer |
| End of v5.0-GA | XPORT V5 deprecation timeline | Set in v6.0 planning |

### 4.6 Out of scope (deferred to v5.x or v6.0)

- B (rewriting user-facing analysis macros — `hazplot`, `kaplan`, `bootstrap.summary`, etc.)
- TemporalHazard performance optimization via `cpp11`/Rcpp hooks into libhazard
- SAS PROC FORMAT catalog application
- XPORT V5 deprecation/removal (v6.0 question)
- Default output format change to Parquet (v6.0 question)
- FDA submission packaging (regulatory work)

---

## §5 — Build System Migration to CMake

### 5.1 Migration model: clean per-release-line split

```
release/4.4 (maintenance)        dev/v5.0-alpha → main (v5.0+)
─────────────────────            ──────────────────────────
configure.ac                     CMakeLists.txt (top-level)
Makefile.am × 14                 src/*/CMakeLists.txt × 14
autoreconf -fi                   cmake -B build
./configure                      cmake --build build
make                             ctest --test-dir build
                                 cmake --install build

LANGUAGES: C only                LANGUAGES: C, CXX
NO Arrow dep                     find_package(Arrow REQUIRED)
                                 find_package(Parquet REQUIRED)
                                 find_package(pybind11)  [optional]
```

No transitional dual-build period. Each release line uses one build system.

### 5.2 Top-level `CMakeLists.txt` shape

```cmake
cmake_minimum_required(VERSION 3.20)
project(hazard VERSION 5.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(HAZARD_ENABLE_WARNINGS  "Enable -Wall -Wextra"          OFF)
option(HAZARD_ENABLE_SANITIZERS "Enable ASan + UBSan"          OFF)
option(HAZARD_BUILD_PYTHON     "Build pybind11 Python frontend" ON)
option(HAZARD_BUILD_TESTS      "Build unit + integration tests" ON)

find_package(Arrow 14.0 REQUIRED)
find_package(Parquet 14.0 REQUIRED)
if(HAZARD_BUILD_PYTHON)
  find_package(pybind11 2.11 REQUIRED)
endif()

add_subdirectory(src/common)      # libhazard_common
# ... (one per existing src/* subdir)
add_subdirectory(src/blackbox)    # XPORT V5 reader (frozen C)
add_subdirectory(src/parquet)     # libhazard_io (C++17 + Arrow)
add_subdirectory(src/hazard)      # hazard binary
add_subdirectory(src/hazpred)     # hazpred binary
if(HAZARD_BUILD_PYTHON)
  add_subdirectory(src/python)
endif()

if(HAZARD_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

add_library(hazard STATIC)
target_link_libraries(hazard PRIVATE
  hazard_common hazard_llike hazard_optim hazard_model hazard_vars
  hazard_vcov hazard_stat hazard_utils hazard_blackbox)
```

Per-subdir uses OBJECT libraries aggregated at top — keeps subdir build files small.

### 5.3 Dependency story per platform (no conda — licensing)

| Platform | Arrow C++ | pybind11 | CMake |
|---|---|---|---|
| **macOS** (Homebrew) | `brew install apache-arrow` | `brew install pybind11` | `brew install cmake` |
| **Linux** (Apache APT repo) | `apt install -y -V apache-arrow-apt-source && apt install libarrow-dev libparquet-dev` | `apt install pybind11-dev` | system or apt |
| **Windows MSYS2/UCRT64** | `pacman -S mingw-w64-ucrt-x86_64-arrow` | `pacman -S mingw-w64-ucrt-x86_64-pybind11` | `pacman -S mingw-w64-ucrt-x86_64-cmake` |
| **Cross-platform** (vcpkg) | `vcpkg install arrow[parquet]` | `vcpkg install pybind11` | system |
| **Reproducible / locked-down** (CMake FetchContent) | Vendored — built from source | Vendored or system | system |

**vcpkg is the conda-replacement** (MIT-licensed, cross-platform, no commercial restrictions).

**PyPI's role:** publishing the `hazardpy` wheel only. NOT used as Arrow C++ source. Wheel build uses system Arrow (per platform) + `auditwheel`/`delocate` to bundle Arrow `.so`/`.dylib` into the wheel for end users.

### 5.4 Autotools → CMake mapping

| Autotools | CMake equivalent | Notes |
|---|---|---|
| `configure.ac` | `CMakeLists.txt` (top-level) | All version + dep + flag logic. |
| `Makefile.am` per subdir | `CMakeLists.txt` per subdir | OBJECT libraries; aggregated at top. |
| `--enable-warnings` | `-DHAZARD_ENABLE_WARNINGS=ON` | Same flag set. |
| `--enable-sanitizers` | `-DHAZARD_ENABLE_SANITIZERS=ON` | Same compile + link flags. |
| `AC_PROG_CC` | `project(... LANGUAGES C CXX)` | Auto-detected. |
| `AM_PROG_LEX` / `AC_PROG_YACC` | `find_package(FLEX)` + `find_package(BISON)` | Built-in. |
| `AC_C_BIGENDIAN` | `include(TestBigEndian)` | Built-in. |
| `AC_CHECK_HEADERS` | `include(CheckIncludeFile)` | Per-header. |
| `AC_FUNC_*` | `include(CheckFunctionExists)` | Per-function. |
| `AC_CONFIG_HEADERS([config.h])` | `configure_file(config.h.in config.h)` | One-line. |
| `make capture-kit` | `add_custom_target(capture-kit ...)` | Same shell logic. |
| `make dist` | `cpack` (CPack source generator) | Tarball generation. |

Macros to retire from `configure.ac`: `AC_AIX`, `AC_ISC_POSIX`, `AC_MINIX`, `AC_FUNC_ALLOCA`, `AC_C_CONST`, `AC_TYPE_SIZE_T`, `AC_FUNC_MALLOC`, `AC_FUNC_MEMCMP`, `AC_FUNC_REALLOC` — all 1990s portability macros for systems we no longer target.

### 5.5 CI matrix

```yaml
strategy:
  matrix:
    os: [ubuntu-latest, macos-latest, windows-latest]
    compiler: [gcc, clang]
    sanitizers: [off, on]
    exclude:
      - os: macos-latest
        compiler: gcc        # macOS standardizes on clang
      - os: windows-latest
        compiler: clang      # MinGW gcc only on Windows

steps:
  - install Arrow (per-platform script)
  - cmake -B build -DHAZARD_ENABLE_WARNINGS=ON [-DHAZARD_ENABLE_SANITIZERS=ON]
  - cmake --build build --parallel
  - ctest --test-dir build --output-on-failure
  - HAZARD_BIN=build/hazard tests/run_all_tests.sh
```

### 5.6 Risks & mitigations

| Risk | Mitigation |
|---|---|
| Arrow `find_package` fails on some platform | Pre-CI smoke test on each target during alpha-1; fallback to vendored Arrow via FetchContent. |
| Bit-exact output regression from changed compiler flags | Run V1+V3 against v4.4.4 on every CMake build; gate at first sign of drift. Replicate autotools flags (`-O2`, no LTO unless requested). |
| Windows MinGW build breaks on Arrow's CMake config quirks | Validate on MSYS2/UCRT64 specifically — already in CI. Vendor Arrow via FetchContent if MSYS2 package is broken. |
| Phase 1 + CMake at the same time = compounded failure modes | CMake lands first within v5.0-alpha. Validate against v4.4.4 numerical bit-exact. *Then* Phase 1 starts. |

### 5.7 Contributor experience

```bash
# Day-one onboarding (post-CMake migration):
git clone github.com/ehrlinger/hazard
cd hazard
git checkout dev/v5.0-alpha
brew install apache-arrow cmake pybind11   # or apt/pacman/vcpkg equivalent
cmake -B build -DHAZARD_ENABLE_WARNINGS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Modern IDEs (CLion, VSCode with C/C++ extension, neovim with `clangd`) consume CMake's `compile_commands.json` natively for completion, jump-to-definition, refactoring. Autotools needs third-party tools (`bear`, `compiledb`) to produce this. Material contributor-experience improvement.

---

## §6 — Test & CI Strategy

### 6.1 The corpus bootstrap problem solves itself

The "blocked on SAS access" issue from `Claude_MODERNIZATION_GUIDE.md` §0 dissolves with Parquet:

```
TODAY (XPORT pipeline, SAS-required):
  examples/data/*.flat  →  [SAS]  →  examples/*.dta (XPORT)  →  hazard  →  *.lst
                                  ▲
                                  └── BLOCKED: needs SAS access

V5.0 (Parquet pipeline, SAS-free):
  examples/data/*.flat  →  [R or Python]  →  tests/corpus/*.parquet  →  hazard  →  *.lst, *.parquet
                                            ▲
                                            └── ONE-TIME: haven::read_xpt() |> arrow::write_parquet()
```

**One-time conversion script** at `tests/corpus/convert_xport_to_parquet.R`:

```r
library(haven); library(arrow)
for (xpt in Sys.glob("examples/*.dta")) {
  parquet_path <- sub("examples/", "tests/corpus/parquet/", xpt)
  parquet_path <- sub("\\.dta$", ".parquet", parquet_path)
  haven::read_xpt(xpt) |> arrow::write_parquet(parquet_path)
}
```

After v5.0-alpha, the test corpus is self-contained, reproducible, and SAS-independent.

### 6.2 Validator inventory

| ID | Script | Input format | Engine | What it checks | Status |
|---|---|---|---|---|---|
| V1 | `validate_numerical.sh` | XPORT V5, Parquet | hazard | Bit-exact: LL, events, params | Existing; extends in alpha |
| V2 | `validate_convergence.sh` | XPORT V5, Parquet | hazard | LL 8 d.p., exact `nfncts`, exact iters | Existing; extends in alpha |
| V3 | `validate_stepwise.sh` | XPORT V5, Parquet | hazard | Stepwise variable-selection path | Existing; extends in alpha |
| V6 | `validate_memory.sh` | XPORT V5, Parquet | hazard | ASan + UBSan clean | Existing; extends in alpha |
| V7 | `validate_concurrent.sh` | XPORT V5, Parquet | hazard | Two simultaneous + 3-way stress | Existing; extends in alpha |
| V8 | `validate_corpus.sh` | XPORT V5, Parquet | hazard | Acceptance harness, byte-diff | Existing; extends in alpha |
| **V9** | `validate_cross_engine.sh` | Parquet only | hazard + TemporalHazard | Same input, both engines, diff outputs | **NEW in v5.0-beta** |
| **V10** | `validate_python_frontend.sh` | Parquet | hazard via pybind11 | Wheel installs and runs reference example | **NEW in v5.0-beta** |
| Unit | `tests/unit/` | (synthetic) | hazard | 168 assertions, in-process | Existing; ports to CTest |
| **C++ unit** | `tests/unit_cxx/` | (synthetic) | libhazard_io | Parquet roundtrip, metadata, format detection | **NEW in v5.0-alpha** |

### 6.3 V9 — cross-engine validation

```
                 ┌──────────────────────┐
                 │  tests/corpus/       │
                 │   parquet/*.parquet  │
                 └──────────┬───────────┘
                            │
                  ┌─────────┴─────────┐
                  ▼                   ▼
       ┌────────────────┐   ┌──────────────────┐
       │  hazard (C)    │   │  TemporalHazard  │
       │  reads parquet │   │  (R) reads same  │
       │  → result.    │   │  parquet → result│
       │    parquet    │   │    .parquet      │
       └────────┬──────┘   └────────┬─────────┘
                │                    │
                └─────────┬──────────┘
                          ▼
              ┌──────────────────────────────┐
              │ V9 diff:                      │
              │ - LL within 6 d.p.            │
              │ - Param estimates within 4 dp │
              │ - Stepwise: same final var set│
              │ - Iteration count: NOT gated  │
              │ - Failures → CI red           │
              └──────────────────────────────┘
```

**TemporalHazard installation in CI:** pinned git SHA (NOT CRAN, even after publish) for deterministic builds. Both repos pin each other's specific commit/tag — bidirectional versioned validation contract.

**Bidirectional pinning protocol:**
- hazard release v5.0.X declares: *"validated against TemporalHazard at commit SHA `abc123`."*
- TemporalHazard release v0.Y.Z declares: *"validated against hazard at tag `v5.0.X`."*
- Updating either pin is a deliberate joint operation: run V9 with the new pin, confirm tolerance, update both READMEs, ship synced releases.

**V9 timing:** report-only in v5.0-beta (TemporalHazard's framework still under construction). Hard-fail at v5.0-rc on whatever subset is implemented; documented exceptions noted in release notes. **Don't block hazard's GA on TemporalHazard's pace.**

### 6.4 Tolerance policy (consolidated, supersedes existing §1.6)

| Comparison | LL | Params | nfncts | iters | stepwise | wall-clock |
|---|---|---|---|---|---|---|
| v5.0 vs v4.4.0 (intra-engine, XPORT V5 input) | bit-exact | 8 d.p. | exact | exact | exact | ≤ 5% |
| v5.0 vs v5.0 (intra-engine, Parquet vs XPORT input) | bit-exact | 8 d.p. | exact | exact | exact | ≤ 5% |
| v5.0 hazard vs TemporalHazard (V9) | 6 d.p. | 4 d.p. | not gated | not gated | same final set | not gated |
| v5.1 hazard (n_phases=3) vs v5.0 (V1-V8 regression-tested) | bit-exact | 8 d.p. | exact | exact | exact | ≤ 5% |
| v5.1 hazard vs TemporalHazard (n_phases ∈ {2,3,4,5}) | 6 d.p. | 4 d.p. | not gated | not gated | same final set | not gated |

Inter-engine tolerance is looser because R's optimizer + BLAS/LAPACK ≠ hazard's hand-written code + libm. **What V9 actually proves: independent reimplementations of the same model agree on the answer.**

### 6.5 CI workflow updates

```
JOB 1: build-and-unit (matrix: 3 OSes × 2 compilers, sanitizers off)
JOB 2: memory-safety (Linux gcc only, sanitizers on)
JOB 3: integration (Linux gcc, full corpus, both formats)
JOB 4: cross-engine (Linux gcc + R + TemporalHazard, v5.0-beta+)
JOB 5: python-frontend (Linux gcc, v5.0-beta+)
JOB 6: coverage (Linux gcc only, gcov + Codecov)
JOB 7: windows (MSYS2/UCRT64)
JOB 8: release-wheels (only on tag push, cibuildwheel × 9 wheels)
```

### 6.6 Test corpus layout post-v5.0

```
tests/
├── corpus/
│   ├── xport_v5/                ← legacy reference, frozen
│   │   ├── hazard/reference/v4.4.x-{platform}/
│   │   └── hazpred/
│   ├── parquet/                  ← NEW v5.0
│   │   ├── inputs/               ← test inputs (committed)
│   │   └── reference/            ← expected outputs per release
│   │       ├── v5.0.0-{platform}/
│   │       └── v5.1.0-{platform}/
│   ├── convert_xport_to_parquet.R   ← one-time bootstrap script
│   └── FINDINGS.md                  ← cross-toolchain FP notes
├── unit/                         ← C unit tests (existing, ported to CTest)
├── unit_cxx/                     ← NEW: C++ unit tests for libhazard_io
├── integration/                  ← V1-V8 scripts (existing, extended to Parquet)
├── cross_engine/                 ← NEW v5.0-beta: V9 hazard vs TemporalHazard
├── python/                       ← NEW v5.0-beta: V10 + pytest suite
└── run_all_tests.sh              ← orchestrator (existing, extended)
```

---

## §7 — Backward Compatibility & Error Handling

This section incorporates the empirical findings from `docs/v5-design-input/2026-04-28-silent-failure-postmortem.md`.

### 7.1 The binary↔SAS protocol contract (formalized in v4.4.7)

**Today's protocol (broken — observed 2026-04-28 on Linux + SAS):**
```
SAS macro                 hazard binary
─────────                 ─────────────
   write control file → /tmp/hzr.<id>.sas
   write !16 stub      → /tmp/hzr.<id>.lst
   %sysexec hazard < ctl > lst   (exit code IGNORED)
                            ┌─ reads ctl, may be NOPRINT NOLOG ──┐
                            │  on success: writes nothing        │
                            │  on failure: prints to stdout      │
                            │  exit code: usually 0              │
                            └────────────────────────────────────┘
   parse .lst (the only signal we have)
   if parse fails → RETRY  ← infinite loop bug lives here
```

**v4.4.7 protocol (hardened):**
```
SAS macro                                    hazard binary
─────────                                    ─────────────
   write control file → /tmp/hzr.<id>.sas
   %sysexec hazard < ctl > lst 2> err
                                                ┌── always:
                                                │   - validate input format
                                                │   - validate var names vs ctl
                                                │   - if reject:
                                                │       stderr: HAZARD_ERROR_<code> <msg>
                                                │       exit non-zero (15-20 range)
                                                │   - if accept:
                                                │       run analysis (NOPRINT respected for .lst)
                                                │       on success:
                                                │         write $TMPDIR/hzr.<id>.ok
                                                │           {exit_code, version, n_obs, n_vars}
                                                │         exit 0
                                                │       on failure:
                                                │         stderr: HAZARD_ERROR_<code> <msg>
                                                │         exit non-zero (per hzfxit)
                                                └─────────────────────────────────────
   STEP 1: check &SYSRC — non-zero ⇒ surface stderr, %abort, NO retry
   STEP 2: check sentinel /tmp/hzr.<id>.ok — missing ⇒ %abort, NO retry
   STEP 3: parse .lst — only if steps 1 and 2 both passed
```

**Three invariants:**

1. **stderr is never silenced.** `NOPRINT` and `NOLOG` PROC options affect the listing and the SAS log respectively; they do not affect stderr. Errors always write to stderr, always with `HAZARD_ERROR_<code> <message>` prefix.
2. **Exit code is the primary success signal.** Exit `0` ⇒ analysis ran to completion. Exit non-zero per `hzfxit` convention ⇒ binary refused or aborted; details on stderr.
3. **Sentinel file is the secondary success signal.** Written only on exit `0`. Contains structured metadata. Existence ⇒ "binary completed and wrote out what it was supposed to"; absence on exit `0` is itself a fault.

Documented in `docs/BINARY-SAS-PROTOCOL.md` (new in v4.4.7).

### 7.2 Structured error code table

```
HAZARD_ERROR_<CODE> <human-readable message>
```

| Code | Meaning | Exit | Phase introduced |
|---|---|---|---|
| `XPORT_V8_REJECTED` | XPORT V8 detected; only V5 supported | 16 | v4.4.6 |
| `XPORT_INVALID_HEADER` | XPORT magic not recognized | 17 | v4.4.6 |
| `INPUT_FORMAT_UNKNOWN` | File doesn't match Parquet or XPORT | 17 | v5.0-alpha |
| `VAR_NOT_FOUND` | Control file references variable not in dataset | 18 | v5.0-alpha |
| `VAR_TYPE_MISMATCH` | Variable type incompatible with PROC option | 18 | v5.0-alpha |
| `OPTIMIZER_DIVERGED` | Numerical optimizer failed to converge | 12 | existing |
| `MEMORY_EXHAUSTED` | Could not allocate working buffers | 14 | existing |
| `LICENSE_INVALID` | (reserved, not currently used) | 19 | future |
| `INTERNAL_ERROR` | Catch-all; should never be user-visible | 20 | existing |

Additions require entries in `docs/BINARY-SAS-PROTOCOL.md` and the `hzd_error_describe()` table.

### 7.3 v4.4.5 / v4.4.6 message text (updated for v5 context)

v4.4.6's structured message for V8 rejection:

> *"This file appears to be SAS XPORT V8. Hazard reads V5 only — V8's long variable names are silently lost. Either:*
>
> *(a) Re-export as V5 with `LIBNAME ... XPORT;` (variable names truncated to 8 chars by SAS — check SAS log for warnings).*
>
> *(b) Use Parquet for full long-name support — hazard v5.0+ reads it natively; export from SAS with `PROC EXPORT DBMS=PARQUET;` (requires SAS 9.4M8 or later).*
>
> *Hazard cannot continue with this input."*

The two failure modes addressed:

| Scenario | Detection by v4.4.6? |
|---|---|
| `LIBNAME ... XPORT;` (default V5) with 32-char source variable names — SAS truncates 32→8, emits WARNING in SAS log, writes well-formed V5 file with truncated names. PROC HAZARD references to long names fail with "variable not found." | **No** — hazard can't tell from a V5 file that names were truncated; original is gone. User must read SAS log. |
| `LIBNAME ... XPORT V=8;` (deliberate V8) with 32-char source variable names — SAS writes V8 with long-name records preserving full names AND a V5-compat namestr with SAS-generated short names. Hazard ignores the long-name records and processes generated short names. | **Yes** — header banner check detects V8; structured rejection. |

### 7.4 NOPRINT semantics (codified in v4.4.6)

- `NOPRINT` ⇒ suppress the analysis listing on stdout. Errors still go to stderr.
- `NOLOG` ⇒ suppress the SAS-log-style diagnostic notes on stderr (NOT errors).
- **Errors and the `HAZARD_ERROR_*` prefix go to stderr unconditionally.** No PROC option silences them.

Matches Unix convention: stderr is for things you need to know about; flags suppress informational output, never errors.

### 7.5 Backward compatibility guarantees (PROC HAZARD)

| Surface | v4.4.x | v5.0 | Compatibility statement |
|---|---|---|---|
| PROC HAZARD existing options (TIME, EVENT, RISK, WEIGHT, EARLY, LATE, CONSTANT, SELECTION, NOPRINT, NOLOG, etc.) | as documented | as documented | Identical behavior. Existing `.sas` scripts run unchanged. |
| PROC HAZARD output format (default) | text `.lst` + XPORT V5 `.dta`/`.haz` | text `.lst` + XPORT V5 `.dta`/`.haz` | Identical default. |
| `OUT_FORMAT=PARQUET` | n/a | new | Extension. Default if omitted = existing behavior. |
| `OUT_FORMAT=PARQUET ONLY` | n/a | new | Suppresses legacy XPORT output. Opt-in. |
| Input format detection | manual via SAS LIBNAME | auto via magic-byte sniff | User unchanged — they don't specify; hazard figures it out. |
| Input format support | XPORT V5 only (silent V8 truncation) | XPORT V5 + Parquet; V8 rejected with structured error | Behavior change for V8 users: silent ⇒ explicit. **Intentional surfacing of the latent bug.** |
| Binary exit codes | 0 = "did something" (often even on failure) | 0 = success, non-zero = structured failure | Behavior change. v4.4.7 updates bundled macros. Third-party macros may need a one-line fix — documented in MIGRATING.md. |
| Stderr output | suppressed under NOPRINT NOLOG | always-on for `HAZARD_ERROR_*` | Behavior change. Affects only failure paths. |
| Sentinel file | none | written on success | New file in `$TMPDIR`. Auto-cleaned. |
| `tests/4.4.0/` reference output stability | bit-exact | bit-exact (intra-engine, where Phase 2 doesn't change precision) | Per tolerance policy (§6.4). |

**Behavior changes summary** (release-notes inventory):

1. XPORT V8 input rejected (was: silent truncation). v4.4.6+.
2. Errors always go to stderr (was: suppressed under NOPRINT NOLOG). v4.4.6+.
3. Exit codes mean what they say. v4.4.6+.
4. Sentinel file written on success. v4.4.7+.
5. Bundled SAS macros updated for new protocol. v4.4.7+.

### 7.6 Migration guide for binary/API consumers

For users of the C binary directly (not via SAS):

| Currently | v5.0 way | Required? |
|---|---|---|
| `cat ctl.sas \| hazard > out.lst; <ignore exit>` | `cat ctl.sas \| hazard > out.lst 2> err.txt; check exit code` | Yes if scripting around hazard. |
| Parse `out.lst` to determine success | Check exit code first, sentinel file second, parse `.lst` for content | Recommended. |
| Read XPORT V5 `.dta` results | XPORT V5 `.dta` (still default) **or** add `OUT_FORMAT=PARQUET` to PROC and read `.parquet` | Optional. |

For users wrapping the binary via Python/shell scripts:
- v5.0 ships a Python frontend (`pip install hazard`); recommended over direct binary invocation for new code.
- Existing direct-binary wrappers continue to work; updating to check exit codes is recommended but not required for correctness if the wrapper isn't doing retries.

---

## §8 — Documentation Reconciliation & Workflow

### 8.1 Document architecture (post-spec)

```
┌─────────────────────────────────────────────────────────────┐
│ docs/superpowers/specs/2026-04-28-hazard-v5-design.md       │
│   — THIS SPEC. Architectural intent + decisions for v5.0+.  │
│   — Single source of truth for "what we're building & why". │
│   — Append-only after user approval; new specs added as     │
│     follow-ups (e.g., Phase G spec for v5.1).               │
└─────────────────────────────────────────────────────────────┘
                             │
                             │ guides
                             ▼
┌─────────────────────────────────────────────────────────────┐
│ docs/Claude_MODERNIZATION_GUIDE.md                          │
│   — IMPLEMENTATION TRACKER. Phase work, validation results, │
│     decision log, breaking-change ledger, risk matrix.      │
│   — Updated as work lands; §7 Progress Log appends per PR.  │
│   — Cross-references the spec for "why"; owns the "how".    │
└─────────────────────────────────────────────────────────────┘
                             │
                             │ produces
                             ▼
┌─────────────────────────────────────────────────────────────┐
│ docs/ (in-tree, user/contributor-facing)                    │
│   — BINARY-SAS-PROTOCOL.md   (new in v4.4.7)                │
│   — PARQUET-FORMAT.md         (new in v5.0-alpha)           │
│   — MIGRATING.md              (new in v5.0-rc)              │
│   — INSTALL.md                (existing; updated for CMake) │
│   — VALIDATION_PLAN.md        (existing; absorbs §6 here)   │
│   — vignettes/                (existing; new for Parquet)   │
└─────────────────────────────────────────────────────────────┘
```

Three layers, three audiences:
- **Spec** = "why is hazard built this way?"
- **Modernization guide** = "where are we, what's next, what landed?"
- **In-tree docs** = "how do I use this?"

### 8.2 Mapping: existing modernization guide → status under v5.0 design

| Existing § | Title | Status | Action |
|---|---|---|---|
| §0 | Release Plan & Current Status | superseded | Replace with link to spec §4 + concise current-state summary. Update version lines. |
| §1 | Testing Strategy & Gates | expanded | Append V9, V10, Parquet bootstrap, consolidated tolerance table from spec §6. Mark SAS-corpus blocker resolved by v4.4.7. |
| §2 — Phase 5 (errors, raise) | done | carry forward | No change. |
| §2 — Phase 5.1 (errors, return) | planned | carry forward | No change. |
| §2 — Phase 1 (context struct) | planned | modified | Phases array layout (anticipates Phase G). |
| §2 — Phase 4 (type system) | planned | carry forward | Update Pattern 4.3: `charVarName[8]` is "lifted on Parquet path; retained at XPORT V5 boundary only." |
| §2 — Phase 2 (numerical core) | planned | carry forward | No change. |
| §2 — Phase 3 (vars/stepwise) | planned | carry forward | No change. |
| §2 — Phase 6 (parser/lexer) | planned | carry forward | No change. |
| §2 — Phase 7 (build system) | planned | superseded | Replace with: full CMake migration on `dev/v5.0-alpha`/`main`; `release/4.4` keeps autotools. See spec §5. Drop §7.4. |
| §2 — Phase E (NEW) | — | new | Engine I/O = Parquet read (alpha) + write (beta) + format detection. |
| §2 — Phase P (NEW) | — | new | Python frontend (pybind11). v5.0-beta. |
| §2 — Phase G (NEW) | — | new | Generalized N-phase model. v5.1. Math feature, not modernization. |
| §3 | Code transformation patterns A–H | carry forward + extend | Add Pattern I (opaque handles), J (`extern "C"` bridge), K (sentinel file), L (`HAZARD_ERROR_*`). |
| §4 — B1 (precision) | open | carry forward | Default unchanged: keep `double`. |
| §4 — B2..B5, B7..B9 | scoped | carry forward | No change. |
| §4 — B6 (charVarName 8-char) | scoped | modified | Update: lifted on Parquet path. XPORT V5 boundary still enforces 8 chars. V8 rejected in v4.4.6. |
| §4 — B10 (NEW) | — | new | Binary↔SAS protocol change in v4.4.7: third-party macros that ignore exit codes break silently if they relied on retry-on-parse-failure. |
| §4 — B11 (NEW) | — | new | NOPRINT semantics change in v4.4.6: stderr now bypasses NOPRINT. |
| §4 — B12 (NEW) | — | new | Exit code semantics change in v4.4.6. |
| §4 — B13 (NEW) | — | new | CMake build-system migration; flags differ. |
| §5 | Rollback Plan | carry forward + extend | Add per-release rollback notes for v4.4.6 (binary) vs v4.4.7 (binary + SAS macro — coordinated rollback required). |
| §6 | Risk Matrix | carry forward + extend | Add Phase E (Low), Phase P (Low), Phase G (Medium). |
| §7 | Progress Log | carry forward (append-only) | Continue. |

Net diff: ~70% carry-forward, ~20% extended, ~10% superseded.

### 8.3 New documentation deliverables

| File | New in | Owner | Audience | Purpose |
|---|---|---|---|---|
| `docs/superpowers/specs/2026-04-28-hazard-v5-design.md` | now | John | future contributors | architectural intent + decisions |
| `docs/BINARY-SAS-PROTOCOL.md` | v4.4.7 | C engine team | binary integrators | exit codes, sentinel, structured errors |
| `docs/PARQUET-FORMAT.md` | v5.0-alpha | I/O layer | Parquet writers/readers | metadata convention, type mapping |
| `docs/MIGRATING.md` | v5.0-rc | spec author | existing v4.4.x users | breaking changes; recipe-by-recipe migration |
| `tests/corpus/convert_xport_to_parquet.R` | v5.0-alpha | test infra | maintainers | one-time corpus conversion |
| `docs/vignettes/parquet-quickstart.md` | v5.0-beta | C engine team | new SAS users | PROC EXPORT → PROC HAZARD walkthrough |
| `docs/vignettes/python-quickstart.md` | v5.0-beta | Python frontend | new Python users | `pip install` → `hazard.fit(...)` |
| `docs/vignettes/cross-validation.md` | v5.0-rc | spec author | both engine users | hazard ↔ TemporalHazard parity locally |

Updated in-tree: `INSTALL.md` (CMake), `README.md`, `NEWS.md`, `ChangeLog`, `docs/VALIDATION_PLAN.md`.

### 8.4 What goes where

| If asking… | Look in… |
|---|---|
| Why is hazard packaged as `libhazard` + `libhazard_io`? | spec §2 |
| Why Parquet over XPORT V8? | spec §3 |
| What's the v5.0 release plan? | spec §4 (mirrored in mod guide §0) |
| How do I run V9? | mod guide §1 + `docs/vignettes/cross-validation.md` |
| How do I add a new error code? | `docs/BINARY-SAS-PROTOCOL.md` + Phase 5/5.1 in mod guide §2 |
| What's the Parquet metadata convention? | `docs/PARQUET-FORMAT.md` |
| How do I migrate my third-party SAS macro to v4.4.7+? | `docs/MIGRATING.md` |
| Why did Phase 1 use array-of-phases layout? | spec §1.3 invariant 5 |
| What landed in v4.4.6? | mod guide §7 |
| Where's the rollback plan for CMake? | mod guide §5 |

### 8.5 Workflow during v5.0 execution

```
1. Spec is the entry point
   - Phase about to start? Re-read spec §X.
   - Decision gate? Spec §4.5 lists deadlines.

2. Implementation lives in modernization guide §2 (per phase) + §7 (log)

3. Validation gates from spec §6 / mod guide §1
   - Per-PR: V1, V3 (always)
   - Per-phase merge: full corpus + tolerance policy
   - Per-release: full V1–V10 + cross-engine + performance

4. New behavior changes captured in mod guide §4 (B-codes)
   - Anything affecting users → MIGRATING.md entry
   - Anything affecting design → spec amendment (rare)

5. Release notes from mod guide §7 + §4 entries
```

Spec amendment process: amendments only for decision-revisit gate flips, postmortem-driven corrections, or new phase additions. Format: append a dated "Amendment" section; log change in mod guide §7.

---

## Appendix A — Decision Log (Brainstorming Session 2026-04-28)

| Q | Decision | Alternatives considered |
|---|---|---|
| Q1 — Scope of "modern SAS workflow" | A + C: input-format modernization + R/Python first-class. B (rewriting analysis macros) deferred. | A only; B included; C only. |
| Q2 — C++ scope | B′: modern C numerical core + C++ at boundaries (Parquet, Python). | A (C only); B″ (Phase 1 in C++); C (full C++ rewrite). |
| Q3 — hazard ↔ TemporalHazard relationship | A: peer engines, Parquet contract, CI cross-validation. | B (TemporalHazard becomes thin C wrapper); C (engines diverge). |
| Q4 — Maintainability strategy | B′ confirmed. Phase 1 context struct in C; modern-C idioms (opaque handles, paired create/destroy, `cleanup` attribute, arena allocators) deliver C++-equivalent maintainability. | Switch to C++ for the language change. |
| Q5 — XPORT compat | A′: read both XPORT V5 and Parquet; reject V8 with structured error pointing at Parquet. Sequenced as v4.4.6 (validation) + v5.0 (Parquet path). | A (read both V5+V8); B (Parquet only, ship converter); C (Parquet only, no converter). |
| Q6 — Sequencing | C: hybrid — Parquet read in alpha, write + pybind11 in beta. | A (Parquet first); B (Parquet last). |
| Q7 — Build system | C: full CMake migration in v5.0; `release/4.4` keeps autotools. | A (autotools + AC_PROG_CXX); B (parallel CMake). |
| Phase G | v5.1 — generalized N-phase model with Phase 1 designed to anticipate it via `phases[N_PHASES_MAX]` array. | v5.0 (too much scope); v6.0 (delays value). |

## Appendix B — Open Decisions with Deadlines

| Gate | Decision | Default | Deadline |
|---|---|---|---|
| End of v4.4.6 + 30 days | Survey results in; classify users V5/V8/Parquet-ready | — | rolling |
| End of v5.0-alpha | Add V8 reader? Yes/no based on survey | No (reject V8) | v5.0-alpha tag |
| End of v5.0-beta | Default output format change? Parquet-by-default in v5.0 vs defer | Defer | v5.0-beta tag |
| End of v5.0-GA | XPORT V5 deprecation timeline | Set in v6.0 planning | v5.0-GA tag |
| Pre-v5.0-rc | Pilot dataset selection | AVC reference unless better candidate emerges | v5.0-rc planning |

## Appendix C — Glossary

| Term | Meaning |
|---|---|
| `libhazard` | Static C library (C17) containing the numerical core, context struct, and XPORT V5 reader. Public API in `hazard.h`. Linked by the C binary and the Python frontend. |
| `libhazard_io` | Static C++17 library for Parquet I/O via Arrow C++. Exposes `extern "C"` bridge for C consumers. |
| `hazardpy` | The pybind11-based Python frontend; published to PyPI as `hazard`. |
| `TemporalHazard` | Pure-R port of the parametric hazard model; peer to `hazard` (C); independent codebase; pre-CRAN. |
| `hazard_dataset_t` | Opaque format-agnostic dataset handle. Both XPORT V5 and Parquet readers produce it. |
| `hazard_context_t` | Opaque analysis-context handle. Replaces the global `HZRstr`/`Common`/etc. structs in v5.0. |
| `Phase G` | v5.1 work item — port TemporalHazard's generalized N-phase parametric form to C. |
| Phase E, P | New phases introduced by this spec — Engine I/O (Parquet), Python frontend. |
| `HAZARD_ERROR_<CODE>` | Structured stderr error prefix introduced in v4.4.6. Machine-parseable. |
| Sentinel file | `$TMPDIR/hzr.<jobid>.<jobix>.ok` — written by hazard binary on success; checked by SAS macros. v4.4.7+. |
| V9 | Cross-engine validation gate — hazard ↔ TemporalHazard on Parquet corpus. v5.0-beta+. |
| V10 | Python frontend validation gate — wheel install + reference example. v5.0-beta+. |
| Bidirectional pinning protocol | hazard and TemporalHazard release tags pin each other's specific commit/tag for V9 reproducibility. |

---

## Amendment 2026-04-29 — Telemetry subsystem

**Amends:** §4.1 (v4.4.6 + v4.4.7 scope), §7 (adds §7.7, §7.8), Appendix C (glossary).
**Process:** per §8.5, this amendment is appended dated and committed as a follow-on to the original spec; original 8-section design preserved as the audit trail of the 2026-04-28 brainstorming session.
**Driver:** during spec review the user observed that v4.4.6's structured-error rejection of XPORT V8 surfaces the bug to individual users but doesn't aggregate the data — the v5.0-alpha decision gate ("if >25% of users on V8 → add V8 reader") still requires a manual user-base survey with all the response-bias problems that entails. Adding a telemetry log converts the gate from "survey" to "empirical query." The user then noted the telemetry must log every invocation (not just V8 rejections) so the gate has a denominator, and that a utility script is needed to make the log actionable.

---

### A.1 Motivation & strategic value

The original spec relied on a manual post-v4.4.6 outreach survey to determine V8 prevalence. That has three problems:
1. **Response bias.** Users who don't respond ≠ users who don't use V8.
2. **Counts wrong things.** "Users who answered the email" is not the same as "users actually running hazard."
3. **One-shot.** A survey at month 1 doesn't catch the user who runs hazard once a quarter.

A passive log written by the binary itself eliminates all three. As a side benefit, the same infrastructure resolves the v5.0-beta decision gate ("default Parquet by output?") with the same machinery — measure `OUT_FORMAT=PARQUET` adoption directly instead of guessing.

This is reusable infrastructure: ~230 LoC of write-side machinery in v4.4.6 + ~330 LoC of read-side tooling in v4.4.7, against a payoff of empirically-resolvable architecture decisions across the entire v5.x cycle.

---

### A.2 Effect on §4.1 — scope amendments

**v4.4.6 scope gains a bullet:**
> • **Telemetry log (write side):** append-only JSONL log of every `invocation` event capturing input format, output format, exit code, duration, scale, and bootstrap-job marker. PHI-safe by construction (no variable names, no content). Path resolved via fallback chain ending at `~/.hazard/events.log` so the feature works without admin coordination. Opt-out via `HAZARD_NO_TELEMETRY=1` or `--no-telemetry`. Self-rotates at 10 MB or 30 days. Specified in §7.7.

**v4.4.6 effort estimate:** updated from `~150 LoC binary + 30 LoC tests` to `~300 LoC binary + 70 LoC tests`.

**v4.4.7 scope gains a bullet:**
> • **Telemetry summary script (read side):** `scripts/hazard-telemetry-summary.py` — Python 3 stdlib-only. Default invocation produces a single-screen summary that directly evaluates the v5.0-alpha "add V8 reader?" gate and the v5.0-beta "default Parquet?" gate. Supports multi-log merging, time-windowed analysis, per-user breakdown, machine-readable CSV/JSON output. Specified in §7.8.

**v4.4.7 effort estimate:** updated from `~200 LoC binary + 100 LoC SAS macro + tests` to `~200 LoC binary + 100 LoC SAS macro + 250 LoC Python + 80 LoC Python tests`.

**v4.4.6 → v4.4.7 split rationale:** putting the read-side script in v4.4.7 lets v4.4.6's telemetry log accumulate ~3-4 weeks of data before the summary tool ships against it. The v4.4.7 release narrative also strengthens — "now with usage analytics" instead of just "protocol fixes."

---

### A.3 §7.7 — Telemetry contract (NEW)

#### A.3.1 Event schema (JSONL, one event per line)

```json
{
  "event": "invocation",
  "schema_version": 1,
  "ts": "2026-04-29T14:23:11Z",
  "host": "lri-sas-p-02.lerner.ccf.org",
  "user": "ehrlinj",
  "hazard_version": "4.4.6",
  "input_format": "XPORT_V5",
  "input_size_bytes": 124816,
  "output_format": "XPORT_V5",
  "exit_code": 0,
  "duration_ms": 1247,
  "n_obs": 1500,
  "n_vars": 12,
  "parent_job_id": null
}
```

| Field | Type | Description |
|---|---|---|
| `event` | string | Always `"invocation"` in v4.4.6. Reserved for future event types. |
| `schema_version` | int | Currently `1`. Bumped on any breaking schema change. |
| `ts` | RFC 3339 string | UTC. Parseable by every standard library. |
| `host` | string | `gethostname(2)`. |
| `user` | string | `$USER` env, falling back to `getpwuid(getuid())->pw_name`. |
| `hazard_version` | string | Compile-time version string. |
| `input_format` | enum | `XPORT_V5`, `XPORT_V8`, `PARQUET`, `UNKNOWN`. |
| `input_size_bytes` | int | `stat(2)` of input file. Null if input unreadable. |
| `output_format` | enum/null | `XPORT_V5` (default), `PARQUET` (when `OUT_FORMAT=PARQUET`), null on rejection. |
| `exit_code` | int | 0 on success; 12-20 on structured failure (per §7.2 table). |
| `duration_ms` | int | Wall-clock from `main()` entry to `exit()`. Millisecond precision. |
| `n_obs` | int/null | Observation count actually processed. Null on rejection. |
| `n_vars` | int/null | Variable count from input. Null on rejection. |
| `parent_job_id` | string/null | Bootstrap job marker (from SAS macro env var `HAZARD_BOOTSTRAP_JOB_ID` if set). Null for non-bootstrap invocations. |

#### A.3.2 Privacy invariants — what is NEVER logged

The schema is fixed at v4.4.6. Adding fields requires a `schema_version` bump and a privacy review. The following are excluded **by design**:

- Variable names (the columns themselves)
- Variable values, labels, formats, informats
- Raw bytes from the input file (beyond size in bytes)
- Contents of the SAS control file (PROC HAZARD options)
- Hostname components beyond `gethostname()` (no domain enrichment, no reverse DNS)
- IP addresses
- Anything that could identify a study, subject, or PHI element

If a future use case requires logging something not on the existing field list, the answer is to revisit this section explicitly, not to add the field silently.

#### A.3.3 Path resolution (fallback chain)

```
1. $HAZARD_TELEMETRY_LOG       (explicit override)
2. /var/log/hazard/events.log  (system install — admin-deployed; only if dir exists and is group-writable)
3. $XDG_STATE_HOME/hazard/events.log  (Linux user install if XDG_STATE_HOME is set)
4. $HOME/.hazard/events.log    (universal fallback)
5. (none — telemetry silently disabled)
```

**Critical: the binary never errors because of telemetry-write failure.** If every path in the chain is unwritable, the invocation completes normally with a single one-time `HZD_LOG_WARN` line on stderr noting that telemetry was disabled. Telemetry is observability, not a correctness path.

#### A.3.4 Opt-out

- **Environment:** `HAZARD_NO_TELEMETRY=1` skips the log write entirely.
- **CLI flag:** `--no-telemetry` for direct-binary invocations.
- **PROC option:** none — telemetry runs even under `NOPRINT NOLOG`. The PROC options control the listing, not the binary's own observability.

The opt-out is a first-class feature, not a hidden escape hatch. Documented alongside the telemetry contract.

#### A.3.5 Rotation

Self-rotation by the binary itself, no external `logrotate` required:
- Trigger: log file size ≥ 10 MB OR oldest event ≥ 30 days old.
- Action: rename `events.log` → `events.log.1`, start fresh `events.log`. Keep one rotation; delete `events.log.2` on next rotation. Bounded ≤ 20 MB on disk per host.
- Centralized deployments (`/var/log/hazard/`) MAY install a `logrotate` config that takes precedence; the binary detects an external `events.log.1` newer than its own rotation and skips its own rotate.

#### A.3.6 Bootstrap aggregation strategy

Bootstrap mode invokes hazard ~1000× per `bs.death.AVC` job. Without aggregation, bootstrap dominates the log and skews any per-user denominator.

**Approach:** the bootstrap macros in `macros/bootstrap.hazard.sas` set `HAZARD_BOOTSTRAP_JOB_ID` as an env var before each iteration's `%sysexec`. The binary copies it into `parent_job_id` on the event. The summary script (§7.8) groups events by `parent_job_id` for aggregate statistics, while preserving per-iteration data for users who want it.

**Fallback if env-var detection turns out to be fiddly during v4.4.7 SAS macro work:** drop `parent_job_id` from v4.4.6's payload; log every iteration raw. The summary script uses time-clustering (events within 100 ms from same user with same input path) as a heuristic. Strictly worse than the env-var path but still functional.

#### A.3.7 Mechanism choice — flat file vs syslog (admin-coordinated)

Two implementation paths for the centralized log on shared servers. The v4.4.6 binary supports both; the **deployment choice** is made after the admin team conversation.

**Path (a) — flat file (`/var/log/hazard/events.log`):**
- Binary calls `fopen()` / `fprintf()` / `fclose()` per event.
- Requires: `/var/log/hazard/` directory created by root with group ownership (`hazard` group), mode 0775, sticky group bit (`chmod g+s`).
- Requires: SAS users added to `hazard` group at server-setup time.
- Optional: `/etc/logrotate.d/hazard` config installed for system-side rotation.
- Pro: simple, no syslog facility allocation, file locality matches user mental model.
- Con: admin has to manage a new log file in their backup/monitoring policy.

**Path (b) — syslog (`syslog(LOG_INFO, ...)` with `LOG_LOCAL0` facility):**
- Binary calls `syslog(3)` per event.
- Requires: institutional syslog already running (it usually is); facility `LOG_LOCAL0` (or whichever) allocated to hazard.
- Pro: zero new files for admin to manage; events flow into existing centralized aggregation (Splunk / ELK / SIEM); admin already has rotation, retention, monitoring.
- Con: events are interleaved with other syslog traffic; the summary script needs to filter by program name; slightly more friction for ad-hoc grep.

**Decision:** ask admin team which they prefer. Both paths share the same JSON event payload — only the write call differs. The binary chooses based on a compile-time flag (`-DHAZARD_TELEMETRY_BACKEND=FILE` or `=SYSLOG`).

**Default for v4.4.6 if admin conversation hasn't happened by ship time:** path (a) flat file with the user-local fallback chain doing all the work; centralized path remains unconfigured. Zero binary changes when admin later approves either deployment.

---

### A.4 §7.8 — Telemetry summary script (NEW)

#### A.4.1 Tool: `scripts/hazard-telemetry-summary.py`

- **Language:** Python 3, **stdlib only**. No `pandas`, no `jq`, no third-party deps. Runs on every shared SAS server out of the box (Linux, macOS, Windows MSYS2).
- **Ships in:** v4.4.7.
- **Documented in:** `docs/BINARY-SAS-PROTOCOL.md` §telemetry, with cross-reference from `docs/MIGRATING.md`.

#### A.4.2 Default output (canonical example)

```
$ hazard-telemetry-summary
Hazard telemetry summary
========================
Period:  2026-04-29 → 2026-05-29 (30 days)
Source:  /var/log/hazard/events.log  (24,318 events)

By input format:
  XPORT_V5     18,231   74.9%  ████████████████████████████
  XPORT_V8      1,847    7.6%  ███
  PARQUET         821    3.4%  █
  (rejected)    3,419   14.1%  █████   ← of which V8: 1,847 / format errors: 3 / other: 1,569

By output format (successful runs, n=20,902):
  XPORT_V5 (default)  20,081   96.1%
  PARQUET                821    3.9%   ← v5.0-beta "Parquet by default?" datapoint

Distinct users: 14
  V5  users: 12
  V8  users:  4    ← 28.6% of users
  Pq  users:  3

────────────────────────────────────────────────────────
DECISION GATES
────────────────────────────────────────────────────────
v5.0-alpha "add V8 reader?" gate (threshold: >25% of users on V8)
  → 4/14 users = 28.6%   TRIGGERED — recommend adding V8 reader

v5.0-beta "Parquet by default?" gate (threshold: >50% of runs use OUT_FORMAT=PARQUET)
  → 821/20,902 runs = 3.9%   NOT TRIGGERED — defer to v6.0

────────────────────────────────────────────────────────
TOP V8 USERS (for outreach)
────────────────────────────────────────────────────────
  ehrlinj         612 V8 events
  user_b          478 V8 events
  user_c          412 V8 events
  user_d          345 V8 events

Bootstrap aggregation: 21,344 invocations grouped into 47 distinct jobs.
  Non-bootstrap denominator: 2,974 invocations (these reflect "real" usage).
```

A single screen directly evaluating both v5.0 decision gates is the explicit design target. If the script's default output requires more than one screen to read, the design has drifted.

#### A.4.3 CLI flags

| Flag | Default | Behavior |
|---|---|---|
| `--log <path>` | (fallback chain) | Read specific log file. Repeat to merge multiple sources. |
| `--since <YYYY-MM-DD>` | 30 days ago | Inclusive start of analysis window. |
| `--until <YYYY-MM-DD>` | today | Inclusive end. |
| `--csv` | off | Machine-readable CSV (for piping to R / Excel). |
| `--json` | off | Machine-readable JSON. |
| `--per-user` | off | Detail breakdown per user with full event count by format. |
| `--per-day` | off | Daily time series with format split. |
| `--bootstrap-mode {include\|exclude\|only}` | `include` | How to handle bootstrap invocations. `include` reports both raw + aggregated; `exclude` drops them entirely; `only` shows just bootstrap. |
| `--gates` | off | Print only the gate evaluations, suppress everything else (for automation). |
| `--help` | — | Standard. |

Exit code: 0 on success; 1 on log read failure; 2 on no events in window. `--gates` mode additionally exits 3 if any gate is triggered (for CI automation that wants to fire on threshold cross).

#### A.4.4 Awk cookbook (in script docstring)

For SAS-shop users not running Python:

```bash
# Format breakdown
awk -F'"input_format":"' '{print $2}' events.log | cut -d'"' -f1 | sort | uniq -c

# V8 users
awk -F'"' '/XPORT_V8/ {for(i=1;i<=NF;i++) if($i=="user") print $(i+2)}' events.log | sort | uniq -c

# Daily volume
awk -F'"' '/"ts":"/{print substr($4,1,10)}' events.log | sort | uniq -c
```

These spot-check the same questions the summary script answers in detail. Documented as fallback when Python isn't available.

#### A.4.5 Multi-source merging

Common case: a user with `~/.hazard/events.log` from their laptop wants to see their personal usage merged with the shared-server log they have read access to. Or: an admin wants to combine logs from multiple SAS servers.

```bash
hazard-telemetry-summary \
  --log /var/log/hazard/events.log@lri-sas-p-02 \
  --log /var/log/hazard/events.log@AWOR-PDSASAPP03 \
  --log ~/.hazard/events.log
```

Logs are concatenated, deduplicated by `(ts, host, user, ts_ns)` tuple, sorted by timestamp, then aggregated. Host attribution preserved — the per-user breakdown can group by host or roll up.

---

### A.5 Infrastructure prerequisites & admin-team coordination

The telemetry feature is designed to **ship in v4.4.6 without any admin involvement** — the user-local fallback at `~/.hazard/events.log` works for every user immediately. Admin coordination unlocks the centralized deployment, which is strictly an upgrade, not a prerequisite for the feature itself.

#### A.5.1 What ships independently of admin

| Capability | Mechanism |
|---|---|
| Telemetry feature works | Fallback chain ends at `~/.hazard/events.log`, always writable |
| Per-user analytics | User runs `hazard-telemetry-summary` on their own log |
| V8 detection user-visible | Structured stderr message (independent of telemetry) |
| Opt-out | `HAZARD_NO_TELEMETRY=1`, `--no-telemetry` flag |

#### A.5.2 What requires admin coordination

| Capability | Requires |
|---|---|
| Centralized log at `/var/log/hazard/` | root-created dir, `hazard` group, group membership for SAS users, optional logrotate config |
| Syslog forwarding alternative | facility allocation (e.g., `LOG_LOCAL0`), institutional aggregator routing rules |
| Aggregate analytics across all server users | Whichever centralized path above |
| User notification before v4.4.6 ship | Communication channel decision (email blast, DWH-team standup, etc.) |
| Policy review | "Is application-level telemetry OK on shared servers?" institutional sign-off |

#### A.5.3 Three asks for the admin conversation

1. **Policy:** is application-level telemetry (invocation logging, no PHI, opt-out available) on shared SAS servers OK? Is there an existing policy or precedent for similar tools?
2. **Mechanism:** preferred — (a) flat file at `/var/log/hazard/` with logrotate config, (b) syslog forwarding with dedicated facility, or (c) something institutional we should plug into?
3. **Communication:** do users need to be notified before v4.4.6 ships, and through what channel?

The admin conversation runs in parallel with v4.4.6 development. Outcome shapes the deployment, not the binary feature.

---

### A.6 Updated effort estimates

| Release | Original | With amendment | Delta |
|---|---|---|---|
| v4.4.6 | ~150 LoC binary + ~30 LoC tests | ~300 LoC binary (incl. ~150 LoC telemetry write side) + ~70 LoC tests | +120 binary, +40 tests |
| v4.4.7 | ~200 LoC binary + ~100 LoC SAS macros + tests | ~200 LoC binary + ~100 LoC SAS macros + ~250 LoC Python + ~80 LoC Python tests | +250 Python, +80 Python tests |
| v5.0-alpha | unchanged | unchanged | 0 |
| v5.0-beta | unchanged | unchanged | 0 |

**Net amendment cost:** ~370 LoC new code + ~120 LoC tests across two maintenance releases. Both releases remain comfortably maintenance-scoped.

---

### A.7 Decision gates this amendment enables

| Gate | Original mechanism | Post-amendment mechanism |
|---|---|---|
| v5.0-alpha "add V8 reader?" | Manual user survey, response-bias-vulnerable | `hazard-telemetry-summary --gates`, empirical |
| v5.0-beta "default Parquet?" | Defer to v6.0 (no data) | `hazard-telemetry-summary` (`OUT_FORMAT=PARQUET` adoption rate visible from day one of v4.4.6) |
| Future: "deprecate XPORT V5?" | TBD in v6.0 planning | Trend data already accruing — `XPORT_V5` invocation count over 12 months gives a real basis |
| Future: "is hazard usage growing or shrinking?" | Anecdotal | Direct from log |

Telemetry is paying for itself before v5.0 even ships.

---

### A.8 Glossary additions (Appendix C)

| Term | Meaning |
|---|---|
| `events.log` | Hazard's append-only JSONL telemetry log. Path resolved via fallback chain. PHI-safe by schema design. v4.4.6+. |
| `parent_job_id` | Bootstrap-job marker propagated from SAS bootstrap macros via env var. Lets the summary script aggregate bootstrap iterations. v4.4.6+. |
| `hazard-telemetry-summary` | Python stdlib-only utility script that summarizes `events.log`. Default output evaluates v5.0 decision gates directly. v4.4.7+. |
| Telemetry opt-out | `HAZARD_NO_TELEMETRY=1` env var or `--no-telemetry` CLI flag. First-class feature, not hidden. |
| `LOG_LOCAL0` | syslog facility candidate for hazard if admin team prefers syslog over flat file. Decision deferred to admin-team conversation. |

---

*End of Amendment 2026-04-29.*

---

*End of spec.*
