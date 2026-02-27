# HAZARD Codebase Analysis

> Generated: 2026-02-27
> Analyst: Claude Code (claude-sonnet-4-6)
> Purpose: Architectural overview for new contributors and maintainers

---

## 1. Architecture

**Monolithic C application with library modules.** Not MVC, not microservices — this is a classic 1990s Unix scientific computing design:

```
hazard.exe ──links──> llike + optim + model + vcov + vars + stat + utils + common
hazpred.exe ──links──> libhazp + common
```

Two main executables share 8–9 static libraries. Command input is parsed via a **Lex/Yacc grammar** that mimics SAS `PROC` statement syntax. Think "domain-specific language interpreter compiled to a monolith."

---

## 2. Entry Points

| Executable | Entry | Parser |
|---|---|---|
| `hazard.exe` | `src/hazard/` | `hazard_l.l` (lexer) + `hazard_y.y` (grammar) |
| `hazpred.exe` | `src/hazpred/` | `hazpred_l.l` + `hazpred_y.y` |

The user invokes the tool from SAS via `PROC HAZARD` / `PROC HAZPRED` statements. SAS passes data in XPORT binary format; the executable parses the proc options, reads the data, runs the analysis, and writes output.

---

## 3. Core Modules (most important)

| Module | Why It Matters |
|---|---|
| `src/common/` | 141 files — global struct definitions (`hzrstr`, `params`, etc.), all shared data structures |
| `src/llike/` | Log-likelihood engine — the mathematical heart; `setcoe.c`, `setlik.c`, `setobj.c` |
| `src/optim/` | BFGS/quasi-Newton optimizer — drives parameter estimation |
| `src/model/` | Hazard phase models (early/constant/late) — core statistical theory |
| `src/hazard/` | Main executable, 187 files; `hazard_l.c` (2362L) + `hazard_y.c` (2305L) are the parser-generated giants |

---

## 4. Data Flow

```
SAS PROC call
  → writes XPORT binary dataset
    → hazard.exe reads XPT file
      → Lex/Yacc parses PROC options → populates global struct hzrstr
        → vars/ selects covariates (stepwise/forward/backward)
          → llike/ computes log-likelihood for each iteration
            → optim/ minimizes -logL via BFGS
              → vcov/ computes variance-covariance (Cholesky decomp)
                → writes output tables + parameter estimates
                  → SAS reads results back
```

All state is carried through a large global struct `HZRstr` (74 fields). This is the "god object" of the codebase.

---

## 5. External Dependencies

Minimal — this is almost entirely self-contained:

| Dependency | Type | Purpose |
|---|---|---|
| **SAS** | Runtime | Data prep/post-processing; XPORT format I/O |
| **libm** | Runtime | Standard C math (`pow`, `sqrt`, etc.) |
| **Flex/Bison** | Build-time | Lexer/parser generation |
| **Autotools** | Build-time | `configure` / `make` build chain |

No network calls, no databases, no external APIs at runtime.

---

## 6. Red Flags

1. **God object** — `struct hzrstr` with 74 fields flows through everything. Changing its layout breaks the world.
2. **187 files in a flat directory** — `src/hazard/` is a maze. Zero subdirectory organization.
3. **Cryptic 5–8 char names** — `hzrbomb.h`, `przconc.h`, `vfynvar.h` — Fortran-era naming inherited from 1990s UAB code. Discovery is painful.
4. **No unit tests** — Tests in `tests/` are SAS output comparisons, not C-level tests. You cannot test a module in isolation.
5. **Parser-generated files checked in** — `hazard_l.c` (2362L) and `hazard_y.c` (2305L) are generated from `.l`/`.y` but appear maintained by hand. Drift risk.
6. **Last significant update: 2012** — The codebase is effectively frozen. Recent commits are minor portability fixes only.
7. **Global mutable state** — Heavy use of `#define STRUCT` pattern + file-level globals makes the code non-reentrant and nearly impossible to parallelize.

---

## 7. Where to Start for Specific Tasks

| Task | Start Here |
|---|---|
| Understand the math | `src/llike/setlik.c`, `src/model/`, then Turner et al. papers in `docs/` |
| Change hazard model parameters | `src/common/common.h` — `struct hzrstr`, `struct params` |
| Add a new covariate selection method | `src/vars/` — `stepwise.c`, `forward.c`, `backward.c` |
| Change optimizer behavior | `src/optim/` — BFGS implementation |
| Change input syntax / add new options | `src/hazard/hazard_y.y` (grammar) + `src/hazard/hazard_l.l` (tokens) |
| Change output format | `src/hazard/` — look for `print*` or `write*` named files |
| Debug a crash | `src/utils/` error handling, then trace back through `hzrstr` state |
| Port to modern build system | `configure.ac` + `Makefile.am` |

---

## 8. Module Reference

| Module | Files | Key Source Files |
|---|---|---|
| `common` | ~141 | Global structs, parameter/option/variable setup |
| `stat` | 2 | `normal.c`, `pdlike.c` |
| `vars` | 8 | Stepwise, backward, forward selection |
| `vcov` | 4 | Cholesky decomposition, gradient, covariance |
| `llike` | 5 | `setcoe.c` (549L), `setlik.c` (539L), `setobj.c` (498L) |
| `model` | 2 | `SETG1` (early phase), `SETG3` (late phase, 952L) |
| `optim` | — | BFGS, quasi-Newton, line search, Hessian |
| `utils` | — | Error handling, memory, I/O |
| `hazard` | ~79 | Main PROC executable |
| `hazpred` | ~95 | Prediction PROC executable |

---

## Summary

**HAZARD** is a specialized 1990s scientific C monolith for parametric survival/hazard analysis, developed at UAB and maintained at Cleveland Clinic. It implements the Conservation of Events theorem (Malcolm E. Turner, Ph.D.) for parametric multi-phase hazard modeling — primarily applied to cardiac surgery outcomes research.

**Strengths:** Solid, well-validated mathematical core; self-contained with minimal dependencies; cross-platform (Unix/Linux/Windows via Cygwin); SAS integration via standard XPORT format.

**Weaknesses:** Fortran-era naming conventions, no C-level unit tests, god-object design, effectively frozen since 2012, 187-file flat directories.

**The one thing to know:** Treat `struct hzrstr` in `src/common/common.h` as the ground truth for all system state. Everything flows through it.
