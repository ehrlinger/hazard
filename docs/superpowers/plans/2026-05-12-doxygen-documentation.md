# Doxygen Documentation Investment Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a pkgdown-equivalent documentation site for the HAZARD C codebase using Doxygen, fed by comments ported from the well-commented FORTRAN source and cross-referenced with the TemporalHazard R port.

**Architecture:** Doxygen reads `/** */` block comments from C source, renders LaTeX math via MathJax, and deploys to GitHub Pages via GitHub Actions. Comments are written file-by-file in priority order: mathematical core first (shaping functions, transforms, log-likelihood — the functions TemporalHazard mirrors directly), then architecture/global-state patterns, then entry points and I/O boundary. The FORTRAN source (historical generator of the C code) provides the mathematical ground truth for each function's docblock.

**Tech Stack:** Doxygen 1.9+, MathJax (via CDN for LaTeX rendering), GitHub Actions (`ubuntu-latest`), GitHub Pages (`gh-pages` branch). No new build dependencies added to the C build — Doxygen is docs-only.

---

## File Structure

Files **created** by this plan:

| File | Purpose |
|------|---------|
| `Doxyfile` | Doxygen configuration (repo root) |
| `.github/workflows/docs.yml` | GitHub Actions: build Doxygen HTML + deploy to gh-pages |
| `docs/doxygen/mainpage.md` | Landing page (`@mainpage`) — project overview, model summary, navigation |
| `docs/doxygen/architecture.md` | Developer page — STRUCT/HAZARD macro pattern, global state, error handling |
| `docs/fortran-c-mapping.md` | FORTRAN subroutine → C function → source file cross-reference table |

Files **modified** (comment additions only, no logic changes):

| File | What gets added |
|------|----------------|
| `src/common/structures.h` | `@file`, struct field docs for `Early`, `Late`, `Common`, `HazG1`, `HazG3`, `Opt` |
| `src/common/hzd_ln_G1_and_SG1.c/.h` | Full mathematical docblock: G1 formula, 6 flag cases, FORTRAN cross-ref |
| `src/common/hzd_ln_G3_and_SG3.c/.h` | Same for G3 shaping function |
| `src/common/hzd_ln_G1_and_SG1_m.c/.h` | G1 modified (M≠0) variant |
| `src/common/hzd_ln_G3_and_SG3_m.c/.h` | G3 modified variant |
| `src/common/hzd_early_p2t.c/.h` | Transform: Early phase parameters → optimizer theta |
| `src/common/hzd_early_t2p.c/.h` | Transform: optimizer theta → Early phase parameters |
| `src/common/hzd_late_p2t.c/.h` | Transform: Late phase parameters → optimizer theta |
| `src/common/hzd_late_t2p.c/.h` | Transform: optimizer theta → Late phase parameters |
| `src/common/hzd_ln_eX_min1.c/.h` | Math helper: ln(e^x - 1) with boundary handling |
| `src/common/hzd_ln_A_div_B.c/.h` | Math helper: ln(A/B) with error gating |
| `src/common/hzd_ln_1min_e_negeX.c/.h` | Math helper: ln(1 - e^(-e^x)) |
| `src/common/hzd_ln_eBXmin1divB.c/.h` | Math helper: ln((e^(Bx)-1)/B) |
| `src/common/hzd_ln_eX_plus1.c/.h` | Math helper: ln(e^x + 1) |
| `src/common/hzd_ln_eeX_min1.c/.h` | Math helper: ln(e^(e^x) - 1) |
| `src/common/hzd_ln_1min_enegX.c/.h` | Math helper: ln(1 - e^(-x)) |
| `src/common/dlgaob.c/.h` | Math helper: log gamma ratio ln(Γ(a)/Γ(b)) |
| `src/common/hzd_set_rho.c/.h` | RHO/delta relationship |
| `src/common/hzd_parms_to_theta.c/.h` | Full parameter → theta dispatch |
| `src/common/hzd_theta_to_parms.c/.h` | Full theta → parameter dispatch |
| `src/llike/setlik.c` | Log-likelihood function — expand existing FORTRAN-style comment block to Doxygen |
| `src/llike/setcoe.c` | Coefficient setup — document obs loop, scaling logic |
| `src/common/hzd_Error.c/.h` | Error handling — longjmp pattern, error code table |
| `src/hazard/hazard.c` | Main entry point — argument parsing, telemetry hooks, exit contract |
| `src/hazpred/hazpred.c` | Hazpred entry point — INHAZ file, prediction loop |
| `src/blackbox/xportHandler.c` | XPORT I/O — V5/V8 detection, format contract |

---

## Task 0: Bring FORTRAN Source Local

**Files:**
- Create: `docs/fortran-c-mapping.md`

> **Prerequisite:** Copy FORTRAN source from CCF network drive to a local working directory (e.g. `~/hazard-fortran/`). This is a manual step — the FORTRAN files do NOT go into the git repo (they are the unreleased proprietary source). Only the mapping document is committed.

- [ ] **Step 1: Copy FORTRAN source from CCF**

On CCF, locate the FORTRAN source (typically `hazard.f` or a directory of `.f` files). Copy to your Mac:

```bash
# Adjust path to match actual CCF location
scp -r lri-sas-p-02:/programs/apps/sas/hazard/src/fortran ~/hazard-fortran/
```

- [ ] **Step 2: Inventory the FORTRAN subroutines**

```bash
grep -n '^\s*SUBROUTINE\|^\s*FUNCTION' ~/hazard-fortran/*.f | sort > /tmp/fortran-subs.txt
cat /tmp/fortran-subs.txt
```

- [ ] **Step 3: Create the FORTRAN → C mapping document**

Create `docs/fortran-c-mapping.md` with this structure, filling in the subroutine names found in Step 2:

```markdown
# FORTRAN → C Function Mapping

Historical FORTRAN subroutines and their C equivalents.
The FORTRAN source was the original implementation; C was
mechanically translated. FORTRAN comments are the authoritative
mathematical documentation source for Doxygen comment work.

| FORTRAN Subroutine | C Function | Source File | Notes |
|--------------------|------------|-------------|-------|
| DLG1 | hzd_ln_G1_and_SG1 | src/common/hzd_ln_G1_and_SG1.c | 6-case g1flag dispatch |
| DLG3 | hzd_ln_G3_and_SG3 | src/common/hzd_ln_G3_and_SG3.c | |
| DTRSFM | hzd_early_p2t, hzd_early_t2p, hzd_late_p2t, hzd_late_t2p | src/common/ | Split from single FORTRAN subroutine |
| SETLIK | SETLIK (setlik.c) | src/llike/setlik.c | Log-likelihood |
| SETCOE | SETCOE | src/llike/setcoe.c | Coefficient/obs loop |
| SETG1 | setg1 | src/model/setg1.c | G1 model setup |
| SETG3 | setg3 | src/model/setg3.c | G3 model setup |
| (fill from grep output) | | | |
```

- [ ] **Step 4: Commit the mapping document**

```bash
git add docs/fortran-c-mapping.md
git commit -m "docs: add FORTRAN-to-C function mapping table"
```

---

## Task 1: Doxygen Infrastructure

**Files:**
- Create: `Doxyfile`
- Create: `.github/workflows/docs.yml`
- Create: `docs/doxygen/mainpage.md`
- Create: `docs/doxygen/architecture.md`

- [ ] **Step 1: Install Doxygen locally to confirm version**

```bash
brew install doxygen
doxygen --version
# Expected: 1.9.x or 1.10.x
```

- [ ] **Step 2: Generate a baseline Doxyfile**

```bash
cd /Users/ehrlinj/Documents/GitHub/hazard
doxygen -g Doxyfile
```

- [ ] **Step 3: Edit Doxyfile — set project metadata**

Open `Doxyfile` and set these values (search for each key):

```
PROJECT_NAME           = "HAZARD"
PROJECT_NUMBER         = "4.4.6"
PROJECT_BRIEF          = "Three-phase parametric hazard analysis — Cleveland Clinic Foundation"
PROJECT_LOGO           =
OUTPUT_DIRECTORY       = docs/doxygen/html
```

- [ ] **Step 4: Edit Doxyfile — set input and extraction**

```
INPUT                  = src/ docs/doxygen/
FILE_PATTERNS          = *.c *.h *.md
RECURSIVE              = YES
EXCLUDE_PATTERNS       = */Makefile* */.git/* *.o *.a
EXTRACT_ALL            = NO
EXTRACT_STATIC         = YES
EXTRACT_ANON_NSPACES   = NO
```

- [ ] **Step 5: Edit Doxyfile — enable MathJax for LaTeX**

```
USE_MATHJAX            = YES
MATHJAX_VERSION        = MathJax_3
MATHJAX_RELPATH        = https://cdn.jsdelivr.net/npm/mathjax@3/es5/
FORMULA_FONTSIZE       = 14
```

- [ ] **Step 6: Edit Doxyfile — set mainpage and appearance**

```
USE_MDFILE_AS_MAINPAGE = docs/doxygen/mainpage.md
GENERATE_TREEVIEW      = YES
HTML_EXTRA_STYLESHEET  =
ALPHABETICAL_INDEX     = YES
GENERATE_LATEX         = NO
HAVE_DOT               = NO
```

- [ ] **Step 7: Add `docs/doxygen/html` to .gitignore**

```bash
echo "docs/doxygen/html/" >> .gitignore
git add .gitignore
```

- [ ] **Step 8: Write the mainpage**

Create `docs/doxygen/mainpage.md`:

```markdown
@mainpage HAZARD — Three-Phase Parametric Hazard Analysis

HAZARD is a parametric survival analysis system implementing
the three-phase hazard model of Blackstone, Naftel, and Turner
(1986). It fits time-varying hazard functions composed of:

- **Early phase** — rapidly decelerating initial hazard (G1 shaping function)
- **Constant phase** — time-independent background hazard
- **Late phase** — slowly accelerating chronic hazard (G3 shaping function)

The binary (`hazard`) reads a PROC HAZARD statement from stdin and
an XPORT-format dataset, fits the model by maximum likelihood, and
writes a formatted listing to stdout.

## Key References

- Blackstone EH, Naftel DC, Turner ME Jr. "The decomposition of
  time-varying hazard into phases, each incorporating a separate
  stream of concomitant information." *J Am Stat Assoc* 1986;81:615-624.
- Turner ME Jr., et al. *Survival Analysis: A Practical Approach*. 1994.

## Navigation

- @ref mathematical_core "Mathematical Core" — G1/G3 shaping functions,
  transforms, log-likelihood
- @ref architecture "Architecture" — global state, error handling,
  STRUCT/HAZARD macro pattern
- @ref io_boundary "I/O Boundary" — XPORT protocol, TMPDIR contract,
  exit codes

## Relationship to TemporalHazard (R port)

The R package [TemporalHazard](https://github.com/ehrlinger/TemporalHazard)
is a faithful port of the C numerical core. Function correspondence:

| C function | R function |
|-----------|-----------|
| hzd_ln_G1_and_SG1 | hzd_G1() |
| hzd_ln_G3_and_SG3 | hzd_G3() |
| hzd_early_p2t / t2p | early_p2t() / early_t2p() |
| hzd_late_p2t / t2p | late_p2t() / late_t2p() |
| SETLIK | loglik() |
```

- [ ] **Step 9: Write the architecture page**

Create `docs/doxygen/architecture.md`:

```markdown
@page architecture Developer Architecture Guide

## Global State and the STRUCT/HAZARD Macro Pattern

All model state lives in a set of global C structs declared in
`src/common/structures.h`. The `#ifdef STRUCT` / `#define EXTERN`
pattern ensures exactly one translation unit allocates storage:

```c
// In exactly ONE .c file (hazard.c or hazpred.c):
#define STRUCT
#include "structures.h"   // allocates storage

// In all other .c files:
#include "structures.h"   // gets extern declarations
```

Key global structs:

| Struct | Variable name | Purpose |
|--------|--------------|---------|
| `struct early` | `Early` | Early phase parameters (delta, tHalf, nu, m, rho) |
| `struct late` | `Late` | Late phase parameters (tau, gamma, alpha, eta, muL) |
| `struct common` | `Common` | Run-wide state (errtrap, status[], errorno, LnLim, Machn) |
| `struct hazG1` | `HazG1` | Per-call G1 inputs/outputs (T, lnG1, lnSG1) |
| `struct hazG3` | `HazG3` | Per-call G3 inputs/outputs |
| `struct opt` | (in HZRstr) | Optimizer state (itrcnt, nfncts, condition) |

## Error Handling — longjmp Pattern

Errors use `setjmp`/`longjmp` rather than return codes:

```c
// At program start (hazard.c main):
if (setjmp(Common.errtrap) != 0) {
    // landed here from hzd_Error() — clean up and exit
}

// Deep in numerical code:
hzd_Error("DLG1900", 2);  // longjmps to errtrap
// execution never continues past hzd_Error()
```

`ErrorPassBack(code)` propagates errors up one level when the caller
cannot call `hzd_Error` directly (e.g., inside a function that returns
a value).

## OBS Array Layout

Patient data is stored in a flat 1D array with `pp7 = 7` columns per row:

```c
// Row I, column J (1-based):
OBS[I*pp7 - pp7 + J - 1]

// Shorthand via Im1p7:
Im1p7 = I * pp7 - 1;
OBS[Im1p7 + J]  // J = 1..7
```

Columns: TIME(1), EVENT(2), LCENSOR(3), RCENSOR(4), WEIGHT(5), ID(6), STRATA(7).
```

- [ ] **Step 10: Run Doxygen locally — confirm it builds**

```bash
cd /Users/ehrlinj/Documents/GitHub/hazard
doxygen Doxyfile 2>&1 | grep -i "warning\|error" | head -20
open docs/doxygen/html/index.html
```

Expected: site opens in browser with minimal content (no function docs yet — that comes in later tasks).

- [ ] **Step 11: Write the GitHub Actions docs workflow**

Create `.github/workflows/docs.yml`:

```yaml
name: Docs

on:
  push:
    branches: [main]
    paths:
      - 'src/**'
      - 'docs/doxygen/**'
      - 'Doxyfile'
      - '.github/workflows/docs.yml'
  workflow_dispatch:

jobs:
  doxygen:
    name: Build and Deploy Doxygen
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Install Doxygen
        run: sudo apt-get update -q && sudo apt-get install -y doxygen

      - name: Build docs
        run: doxygen Doxyfile

      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        if: github.ref == 'refs/heads/main'
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: docs/doxygen/html
          publish_branch: gh-pages
```

- [ ] **Step 12: Enable GitHub Pages on the repo**

```bash
gh api --method PUT repos/ehrlinger/hazard/pages \
  --field source='{"branch":"gh-pages","path":"/"}' 2>&1 || \
gh api repos/ehrlinger/hazard/pages 2>/dev/null | grep url
```

If the Pages endpoint already exists, the second command shows the URL.

- [ ] **Step 13: Commit infrastructure**

```bash
git add Doxyfile .github/workflows/docs.yml docs/doxygen/ .gitignore
git commit -m "docs: add Doxygen infrastructure with GitHub Pages deployment"
git push origin main
```

Expected: CI `Docs` job runs, gh-pages branch created, site live at `https://ehrlinger.github.io/hazard/`.

---

## Task 2: Comment — Math Helpers (11 files)

**Files:** `src/common/hzd_ln_*.c/.h`, `src/common/dlgaob.c/.h`

These are the leaf-level numerical functions. Each has one clear mathematical identity. The FORTRAN source will have the formula as a comment; port it as a `\f[ ... \f]` LaTeX block.

**Doxygen comment template for each file:**

```c
/**
 * @file hzd_ln_eX_min1.c
 * @brief Numerically stable computation of \f$\ln(e^x - 1)\f$.
 *
 * For large positive \f$x\f$, \f$e^x - 1 \approx e^x\f$, so
 * \f$\ln(e^x - 1) \approx x\f$. For small \f$x > 0\f$, uses the
 * identity \f$\ln(e^x - 1) = \ln(x) + \ln\!\left(\frac{e^x-1}{x}\right)\f$
 * to avoid cancellation error.
 *
 * **FORTRAN origin:** Subroutine LN_EX_MIN1 in hazard.f (line ~NNN).
 *
 * **TemporalHazard R equivalent:** `hzd_ln_eX_min1()` in R/math_helpers.R.
 *
 * @see hzd_Error() for the error signalling convention used on domain errors.
 */
```

And for each function:

```c
/**
 * @brief Compute \f$\ln(e^x - 1)\f$ with domain check.
 *
 * Sets `Common.errorno` and calls hzd_Error() if \f$x \leq 0\f$
 * (domain error — argument must be strictly positive).
 *
 * @note Called inside the G1/G3 shaping function evaluation loops.
 *       Global side-effect: reads/writes `Common.errorno`.
 */
double hzd_ln_eX_min1(double x);
```

- [ ] **Step 1: Add file + function docblocks to `hzd_ln_eX_min1.c/.h`**

Open `src/common/hzd_ln_eX_min1.h`. Add above the function declaration:

```c
/**
 * @brief Numerically stable \f$\ln(e^x - 1)\f$.
 *
 * Boundary handling:
 * - \f$x \leq 0\f$: calls hzd_Error("DMATH_EXMIN1", 2)
 * - \f$x \gg 1\f$: returns \f$x\f$ (asymptotic)
 * - \f$x \approx 0^+\f$: uses Taylor expansion to avoid cancellation
 */
double hzd_ln_eX_min1(double x);
```

Open `src/common/hzd_ln_eX_min1.c`. Add `@file` block at top (after includes):

```c
/**
 * @file hzd_ln_eX_min1.c
 * @brief \f$\ln(e^x - 1)\f$ — numerically stable implementation.
 * @see FORTRAN: docs/fortran-c-mapping.md
 */
```

- [ ] **Step 2: Repeat for all 10 remaining math helper files**

Apply the same pattern to each file below. For each: (a) read the FORTRAN comment for the formula, (b) add `@file` block to `.c`, (c) add `@brief` + LaTeX formula to function declaration in `.h`.

Files (check FORTRAN mapping for formula details):
- `hzd_ln_A_div_B.c/.h` — \f$\ln(A/B)\f$
- `hzd_ln_1min_e_negeX.c/.h` — \f$\ln(1 - e^{-e^x})\f$
- `hzd_ln_eBXmin1divB.c/.h` — \f$\ln\!\left(\frac{e^{Bx}-1}{B}\right)\f$
- `hzd_ln_eX_plus1.c/.h` — \f$\ln(e^x + 1)\f$
- `hzd_ln_eeX_min1.c/.h` — \f$\ln(e^{e^x} - 1)\f$
- `hzd_ln_1min_enegX.c/.h` — \f$\ln(1 - e^{-x})\f$
- `hzd_ln_G1_and_SG1_m.c/.h` — G1 modified (M≠0)
- `hzd_ln_G3_and_SG3_m.c/.h` — G3 modified
- `dlgaob.c/.h` — \f$\ln\!\left(\frac{\Gamma(a)}{\Gamma(b)}\right)\f$
- `hzd_set_rho.c/.h` — RHO/delta relationship

- [ ] **Step 3: Run Doxygen and confirm functions appear**

```bash
doxygen Doxyfile 2>&1 | grep -c "warning"
open docs/doxygen/html/hzd__ln__eX__min1_8c.html
```

Expected: function page renders with LaTeX formula.

- [ ] **Step 4: Commit**

```bash
git add src/common/hzd_ln_*.c src/common/hzd_ln_*.h src/common/dlgaob.c src/common/dlgaob.h src/common/hzd_set_rho.c src/common/hzd_set_rho.h
git commit -m "docs: add Doxygen docblocks to math helper functions (11 files)"
```

---

## Task 3: Comment — G1/G3 Shaping Functions

**Files:** `src/common/hzd_ln_G1_and_SG1.c/.h`, `src/common/hzd_ln_G3_and_SG3.c/.h`

These are the highest-value files — the G1 and G3 functions are the core of the three-phase model and the most complex. The FORTRAN DLG1/DLG3 subroutines will have the six `g1flag` cases documented.

- [ ] **Step 1: Read FORTRAN DLG1 comments**

```bash
grep -A 100 'SUBROUTINE DLG1\|SUBROUTINE dlg1' ~/hazard-fortran/*.f | head -120
```

Extract the description of each case (flag 1–6) into a text scratch file.

- [ ] **Step 2: Add docblock to `hzd_ln_G1_and_SG1.h`**

```c
/**
 * @brief Early phase log-hazard and log-survival contribution: G1 shaping function.
 *
 * Computes \f$\ln G_1(T)\f$ and \f$\ln S_{G_1}(T)\f$ for the early phase
 * of the three-phase hazard model (Blackstone, Naftel, Turner 1986).
 *
 * The G1 function takes six forms depending on `Early.g1flag`:
 *
 * | g1flag | Condition | Equation |
 * |--------|-----------|---------|
 * | 1 | M=0, Nu=0, Delta=0 | Exponential (constant hazard) |
 * | 2 | M=0, Nu<0, Delta=0 | Weibull-type |
 * | 3 | M=0, Nu>0, Delta=0 | Log-logistic-type |
 * | 4 | M≠0, Delta=0 | Full gamma model |
 * | 5 | Delta≠0, M=0 | Location-shifted |
 * | 6 | Delta≠0, M≠0 | General form |
 *
 * **Inputs (via globals):**
 * - `HazG1.T` — evaluation time (must be > 0)
 * - `Early.g1flag` — selects equation form (1–6)
 * - `Early.delta`, `Early.nu`, `Early.m` — shape parameters
 *
 * **Outputs (via globals):**
 * - `HazG1.lnG1` — \f$\ln G_1(T)\f$
 * - `HazG1.lnSG1` — \f$\ln S_{G_1}(T)\f$
 *
 * **Error codes:**
 * - `DLG1900` — T ≤ 0 (invalid time)
 * - `DLG1910` — g1flag out of range [1,6]
 * - `DLG1920` — internal computation error in eBXmin1divB
 *
 * **FORTRAN origin:** Subroutine DLG1, hazard.f.
 * **TemporalHazard R equivalent:** `hzd_G1()`.
 */
void hzd_ln_G1_and_SG1(void);
```

- [ ] **Step 3: Add `@file` block to `hzd_ln_G1_and_SG1.c`**

```c
/**
 * @file hzd_ln_G1_and_SG1.c
 * @brief Early phase G1 shaping function — six-case dispatch.
 *
 * Ported from FORTRAN subroutine DLG1. The six cases correspond to
 * limiting forms of the generalized gamma distribution used to model
 * early-phase hazard that decelerates rapidly from a high initial value.
 */
```

- [ ] **Step 4: Repeat for G3 (`hzd_ln_G3_and_SG3.c/.h`)**

Same structure as G3, but from FORTRAN DLG3. G3 models late-phase hazard (accelerating chronic risk). Read FORTRAN comments:

```bash
grep -A 80 'SUBROUTINE DLG3\|SUBROUTINE dlg3' ~/hazard-fortran/*.f | head -90
```

Add equivalent `@brief`, `@note`, `@param` (via globals table), FORTRAN cross-ref, and TemporalHazard R equivalent.

- [ ] **Step 5: Run Doxygen and verify**

```bash
doxygen Doxyfile 2>&1 | grep -i warning | grep -v "is not documented"
open docs/doxygen/html/hzd__ln__G1__and__SG1_8h.html
```

Expected: function page shows the g1flag case table and LaTeX formulas.

- [ ] **Step 6: Commit**

```bash
git add src/common/hzd_ln_G1_and_SG1.* src/common/hzd_ln_G1_and_SG1_m.* \
        src/common/hzd_ln_G3_and_SG3.* src/common/hzd_ln_G3_and_SG3_m.*
git commit -m "docs: add Doxygen docblocks to G1/G3 shaping functions with 6-case table"
```

---

## Task 4: Comment — Phase Transform Functions

**Files:** `src/common/hzd_early_p2t.c/.h`, `src/common/hzd_early_t2p.c/.h`, `src/common/hzd_late_p2t.c/.h`, `src/common/hzd_late_t2p.c/.h`

These are the parameter ↔ optimizer-theta transforms. In FORTRAN, these were a single DTRSFM subroutine; in C they were split into four functions. The transform equations are the key content — document each parameter's transform formula.

- [ ] **Step 1: Read FORTRAN DTRSFM comments**

```bash
grep -A 150 'SUBROUTINE DTRSFM\|SUBROUTINE dtrsfm' ~/hazard-fortran/*.f | head -160
```

Record the transform for each parameter (delta, tHalf, nu, tau, gamma, alpha, eta).

- [ ] **Step 2: Add docblock to `hzd_early_p2t.h`**

```c
/**
 * @brief Transform Early phase parameters to optimizer theta vector.
 *
 * Maps bounded physical parameters to unconstrained optimizer space:
 *
 * | Parameter | status[] | Transform | theta index |
 * |-----------|----------|-----------|-------------|
 * | delta | [0] | \f$\theta_0 = \ln(-\ln|\delta|)\f$ | 0 |
 * | tHalf | [1] | \f$\theta_1 = \ln(t_{1/2})\f$ | 1 |
 * | nu | [2] | \f$\theta_2 = \ln(|\nu|)\f$ | 2 |
 *
 * Only parameters with `Common.status[i] == 1` (estimated) are transformed.
 * Fixed parameters are left unchanged.
 *
 * **FORTRAN origin:** DTRSFM case P2T (parameters to theta), early block.
 * **TemporalHazard R equivalent:** `early_p2t()`.
 *
 * @param[out] sTheta  Unconstrained theta vector; indices 0–2 written.
 */
void hzd_early_p2t(double *sTheta);
```

- [ ] **Step 3: Add docblocks to the other three transform files**

Apply the same pattern:

`hzd_early_t2p.h` — reverse transform (theta → Early params):
```c
/**
 * @brief Transform optimizer theta back to Early phase parameters.
 *
 * Inverse of hzd_early_p2t(). Recovers bounded parameters from
 * unconstrained theta after each optimizer step:
 *
 * | theta index | Parameter | Inverse transform |
 * |-------------|-----------|------------------|
 * | 0 | delta | \f$\delta = -e^{-e^{\theta_0}}\f$ |
 * | 1 | tHalf | \f$t_{1/2} = e^{\theta_1}\f$ |
 * | 2 | nu | \f$\nu = -e^{\theta_2}\f$ (sign from initial) |
 *
 * @param[in] sTheta  Unconstrained theta vector from optimizer.
 */
void hzd_early_t2p(double *sTheta);
```

`hzd_late_p2t.h` — Late phase params → theta (tau, gamma, alpha, eta).
`hzd_late_t2p.h` — theta → Late phase params.

Fill the transform table from the FORTRAN DTRSFM comments for the late block.

- [ ] **Step 4: Add `@file` blocks to all four `.c` files**

```c
/**
 * @file hzd_early_p2t.c
 * @brief Early phase parameter-to-theta transform.
 * @see hzd_early_t2p.c for the inverse.
 * @see FORTRAN: DTRSFM subroutine, P2T block (docs/fortran-c-mapping.md).
 */
```

Repeat for `hzd_early_t2p.c`, `hzd_late_p2t.c`, `hzd_late_t2p.c`.

- [ ] **Step 5: Run Doxygen and verify transform table renders**

```bash
doxygen Doxyfile 2>&1 | grep -c warning
open docs/doxygen/html/hzd__early__p2t_8h.html
```

Expected: parameter transform table visible with LaTeX formulas.

- [ ] **Step 6: Commit**

```bash
git add src/common/hzd_early_p2t.* src/common/hzd_early_t2p.* \
        src/common/hzd_late_p2t.* src/common/hzd_late_t2p.* \
        src/common/hzd_parms_to_theta.* src/common/hzd_theta_to_parms.*
git commit -m "docs: add Doxygen docblocks to phase transform functions with formula tables"
```

---

## Task 5: Comment — Log-Likelihood (setlik.c, setcoe.c)

**Files:** `src/llike/setlik.c`, `src/llike/setcoe.c`

`setlik.c` already has a partial FORTRAN-style comment block (the likelihood formula in ASCII art). Expand it into a proper Doxygen `@mainpage`-linked group with LaTeX formulas. `setcoe.c` documents the observation loop and coefficient setup.

- [ ] **Step 1: Read FORTRAN SETLIK comments**

```bash
grep -A 200 'SUBROUTINE SETLIK\|SUBROUTINE setlik' ~/hazard-fortran/*.f | head -210
```

- [ ] **Step 2: Replace the existing ASCII-art comment block in `setlik.c` with Doxygen**

The current block (lines ~14–60) uses `C` FORTRAN comment style with ASCII art formulas. Replace with:

```c
/**
 * @file setlik.c
 * @brief Log-likelihood function for the three-phase hazard model.
 * @ingroup log_likelihood
 *
 * Computes the full-data log-likelihood:
 *
 * \f[
 *   \ell = \sum_{i=1}^{N} \Bigl[
 *     C_1(i) W_1(i) \ln h(T_i)
 *     - \bigl(C_1(i)W_1(i) + C_2(i) + C_3(i)W_3(i)\bigr)
 *       \bigl(\Lambda(T_i) - \Lambda(ST_i)\bigr)
 *   \Bigr]
 * \f]
 *
 * where:
 * - \f$h(t)\f$ = hazard function (time derivative of cumulative hazard)
 * - \f$\Lambda(t)\f$ = cumulative hazard function
 * - \f$C_1(i)\f$ = event indicator (1 if event, 0 if censored)
 * - \f$W_1(i)\f$ = event weight (NELSONL WEIGHT= variable; 1.0 if unweighted)
 * - \f$C_2(i)\f$ = right-censoring indicator
 * - \f$C_3(i)\f$ = interval-censoring indicator
 * - \f$W_3(i)\f$ = interval-censoring weight
 * - \f$ST_i\f$ = left-censoring start time (repeating events / time-varying covariates)
 *
 * **FORTRAN origin:** Subroutine SETLIK, hazard.f.
 * **Called by:** optimizer at each function evaluation.
 */
```

- [ ] **Step 3: Add `@file` block to `setcoe.c`**

```c
/**
 * @file setcoe.c
 * @brief Coefficient and scaling setup for the log-likelihood observation loop.
 * @ingroup log_likelihood
 *
 * Walks the OBS array (pp7=7 columns per row) computing per-observation
 * contributions to the likelihood. Handles:
 * - Event vs. right-censored vs. interval-censored classification
 * - Left-truncation (ST column) for repeating events
 * - Conservation of events (CONSERVE option): scales contributions so
 *   the fitted event count equals the observed count
 *
 * **FORTRAN origin:** Subroutine SETCOE, hazard.f.
 */
```

- [ ] **Step 4: Run Doxygen and verify**

```bash
doxygen Doxyfile 2>&1 | grep warning | grep -v "is not documented"
open docs/doxygen/html/setlik_8c.html
```

Expected: LaTeX likelihood formula renders via MathJax.

- [ ] **Step 5: Commit**

```bash
git add src/llike/setlik.c src/llike/setcoe.c src/llike/setcoe_*.c
git commit -m "docs: add Doxygen docblocks to log-likelihood files with LaTeX likelihood formula"
```

---

## Task 6: Comment — Global State (structures.h)

**Files:** `src/common/structures.h`

The `@struct` Doxygen commands document each field of each struct. This is the most important architectural documentation — it explains what every global variable means.

- [ ] **Step 1: Add `@file` block to `structures.h`**

```c
/**
 * @file structures.h
 * @brief Global struct definitions for all HAZARD model state.
 *
 * All model state is global. Exactly ONE translation unit must
 * `#define STRUCT` before `#include "structures.h"` to allocate
 * storage. All others get `extern` declarations via the EXTERN macro.
 *
 * Usage pattern:
 * ```c
 * // In hazard.c only:
 * #define STRUCT
 * #include "structures.h"
 *
 * // In all other .c files:
 * #include "structures.h"
 * ```
 */
```

- [ ] **Step 2: Add field docs to `struct early`**

```c
/**
 * @brief Early phase hazard model parameters.
 *
 * Corresponds to the G1 shaping function parameters.
 * Set by setparm() from PROC HAZARD PARMS= statement.
 */
struct early {
  double muE;     /**< Early phase scale: expected events in early hazard stream. */
  double delta;   /**< Early shape: rate of hazard deceleration (bounded (-1,1)). */
  double tHalf;   /**< Early shape: time at which hazard declines to half its peak. */
  double rho;     /**< Derived from delta: \f$\rho = -\ln|\delta|\f$. */
  double nu;      /**< Early shape: controls early hazard tail. */
  double m;       /**< Early shape: mixing parameter (0 = no mixing). */
  int g1flag;     /**< G1 case selector 1–6; set by setg1() from delta/nu/m values. */
  logical delta0; /**< TRUE if delta == 0 (exponential/Weibull special case). */
  logical deltaN; /**< TRUE if delta < 0. */
```

- [ ] **Step 3: Add field docs to `struct late`, `struct common`, `struct hazG1`, `struct hazG3`**

For `struct late`:
```c
/**
 * @brief Late phase hazard model parameters.
 * Corresponds to the G3 (Weibull-family) shaping function.
 */
struct late {
  double muL;   /**< Late phase scale: expected events in late hazard stream. */
  double tau;   /**< Late shape: onset time of the late hazard rise. */
  double gamma; /**< Late shape: acceleration parameter. */
  double alpha; /**< Late shape: power parameter (Weibull exponent when eta=1). */
  double eta;   /**< Late shape: stretching parameter. */
  ...
```

For `struct common`, document `errtrap`, `status[]`, `errorno`, `LnLim`, `Machn`:
```c
/**
 * @brief Run-wide state shared across all subsystems.
 */
struct common {
  jmp_buf errtrap; /**< longjmp target for hzd_Error(). Set by setjmp() in main(). */
  int status[N_PARMS]; /**< Estimation flags: 1=estimated, 0=fixed, for each parameter. */
  int errorno;     /**< Current error code; 0 = no error. Checked by ErrorPassBack(). */
  double LnLim;    /**< Ln of the smallest representable positive number. Set by hzd_set_LnLim(). */
  double Machn;    /**< Machine epsilon. Set by hzd_set_Machn(). */
  ...
```

- [ ] **Step 4: Run Doxygen and verify struct pages**

```bash
doxygen Doxyfile 2>&1 | grep -c warning
open docs/doxygen/html/structearly.html
```

Expected: struct page shows field table with descriptions.

- [ ] **Step 5: Commit**

```bash
git add src/common/structures.h
git commit -m "docs: add Doxygen struct field documentation to structures.h"
```

---

## Task 7: Comment — Architecture Files (hzd_Error, hzfxit, xexit)

**Files:** `src/common/hzd_Error.c/.h`, `src/common/hzfxit.c/.h`, `src/common/xexit.c/.h`, `src/common/hzd_exit_codes.h`

- [ ] **Step 1: Add docblock to `hzd_Error.h`**

```c
/**
 * @brief Signal a fatal error and longjmp to Common.errtrap.
 *
 * The primary error mechanism for HAZARD. Calling hzd_Error() never
 * returns — execution resumes at the setjmp() call in main() with
 * a non-zero return value.
 *
 * Side effects:
 * - Writes `HAZARD_ERROR_<code> <description>` to stderr
 *   (bypasses NOPRINT/NOLOG suppression — always emitted)
 * - Sets `Common.errorno = errnum`
 * - Calls `longjmp(Common.errtrap, errnum)`
 *
 * @param code    Error code string, e.g. "DLG1900". Written to stderr.
 * @param errnum  Integer error number stored in Common.errorno.
 *                Convention: 2 = numerical domain error.
 */
void hzd_Error(const char *code, int errnum);
```

- [ ] **Step 2: Add docblock to `hzd_exit_codes.h`**

```c
/**
 * @file hzd_exit_codes.h
 * @brief Process exit code constants for hazard and hazpred.
 *
 * | Code | Constant | Meaning |
 * |------|----------|---------|
 * | 0 | HZD_EXIT_SUCCESS | Clean exit, results in listing |
 * | 12 | HZD_EXIT_OPTIMIZER_DIVERGED | Optimizer failed to converge |
 * | 14 | HZD_EXIT_MEMORY_EXHAUSTED | malloc() failure |
 * | 16 | HZD_EXIT_XPORT_V8_REJECTED | SAS Transport V8 format not supported |
 * | 17 | HZD_EXIT_XPORT_INVALID_HEADER | Unrecognised XPORT header |
 * | 20 | HZD_EXIT_INTERNAL_ERROR | Unexpected internal state |
 */
```

- [ ] **Step 3: Commit**

```bash
git add src/common/hzd_Error.* src/common/hzfxit.* src/common/xexit.* src/common/hzd_exit_codes.h
git commit -m "docs: add Doxygen docblocks to error handling and exit code files"
```

---

## Task 8: Comment — Entry Points (hazard.c, hazpred.c)

**Files:** `src/hazard/hazard.c`, `src/hazpred/hazpred.c`

- [ ] **Step 1: Add `@file` block to `hazard.c`**

```c
/**
 * @file hazard.c
 * @brief HAZARD binary entry point — PROC HAZARD SAS procedure implementation.
 *
 * ## Invocation
 * ```
 * hazard < job.input > job.lst
 * ```
 *
 * ## Execution flow
 * 1. Parse `--no-telemetry` / `--version` / `-h` CLI flags
 * 2. Initialize telemetry (hzd_telemetry_open()) — writes JSONL event on exit
 * 3. setjmp(Common.errtrap) — error recovery point
 * 4. Read PROC HAZARD statement from stdin (opnfils.c)
 * 5. Fit model (setoptim() → optimizer loop → setlik())
 * 6. Write listing to stdout (OPTPRTX(), PRTHDR(), etc.)
 * 7. hzd_telemetry_close() — finalises event record
 * 8. Exit with HZD_EXIT_* code
 *
 * ## Environment variables
 * | Variable | Effect |
 * |----------|--------|
 * | HAZARD_NO_TELEMETRY=1 | Disable telemetry |
 * | HAZARD_TELEMETRY_LOG | Override log file path |
 * | HAZARD_LOG_LEVEL | Diagnostic verbosity (DEBUG/INFO/WARN/ERROR) |
 * | TMPDIR | Working directory for XPORT .dta and .haz files |
 * | HAZARD_BOOTSTRAP_JOB_ID | Parent job ID for telemetry parent_job_id field |
 */
```

- [ ] **Step 2: Add `@file` block to `hazpred.c`**

Same structure, but document the PROC HAZPRED flow:
1. Read INHAZ `.haz` parameter file
2. Read DESIGN dataset
3. For each observation, evaluate cumulative hazard at TIME values
4. Write prediction listing

- [ ] **Step 3: Commit**

```bash
git add src/hazard/hazard.c src/hazpred/hazpred.c
git commit -m "docs: add Doxygen @file blocks to hazard and hazpred entry points"
```

---

## Task 9: Comment — I/O Boundary (xportHandler.c)

**Files:** `src/blackbox/xportHandler.c`, `src/blackbox/xport_version.c/.h`

- [ ] **Step 1: Add `@file` block to `xportHandler.c`**

```c
/**
 * @file xportHandler.c
 * @brief SAS XPORT Transport format reader — V5 and V8 detection.
 *
 * ## XPORT Format
 * SAS XPORT (Transport) files begin with an 80-byte header record:
 * ```
 * HEADER RECORD*******LIBRARY HEADER RECORD!!!!!!!000000000000...
 * ```
 * The second 80-byte record identifies the version:
 * - `"LIBRARY "` — V5 (supported)
 * - `"LIBV8   "` — V8 (rejected: exit HZD_EXIT_XPORT_V8_REJECTED)
 *
 * ## File location convention
 * XPORT files are read from `$TMPDIR/hzr.<JOBID>.<JOBIX>.dta`.
 * The JOBID and JOBIX values come from the PROC HAZARD statement.
 * After reading, the file is deleted (in-place consumption pattern).
 *
 * @see xport_version.h for the V5/V8 detection function.
 * @see opnfils.c for the path construction and fopen() call.
 */
```

- [ ] **Step 2: Add docblock to `xport_version.h`**

```c
/**
 * @brief Detect SAS XPORT transport file version from header.
 *
 * Reads the second 80-byte record of an open XPORT file and
 * classifies it as V5 (supported) or V8 (rejected).
 *
 * @param f  Open file pointer positioned at byte 0.
 * @return   XPORT_V5 (0) if supported, XPORT_V8 (1) if rejected,
 *           XPORT_INVALID (-1) if header unrecognised.
 */
int xport_detect_version(FILE *f);
```

- [ ] **Step 3: Commit**

```bash
git add src/blackbox/xportHandler.c src/blackbox/xport_version.*
git commit -m "docs: add Doxygen docblocks to XPORT I/O boundary"
```

---

## Task 10: Final — Run Full Doxygen, Verify, Push

- [ ] **Step 1: Run Doxygen and count undocumented warnings**

```bash
doxygen Doxyfile 2>&1 | grep "is not documented" | wc -l
doxygen Doxyfile 2>&1 | grep -v "is not documented" | grep -i warning
```

The first number is the remaining undocumented symbol count (acceptable to leave non-zero for internal helpers). The second line should be empty or near-empty.

- [ ] **Step 2: Open and review the generated site**

```bash
open docs/doxygen/html/index.html
```

Check:
- Mainpage renders with correct project description
- Math formulas render via MathJax (requires internet connection)
- G1/G3 pages show the flag-case table
- `struct early` page shows field descriptions
- `setlik.c` shows the likelihood formula

- [ ] **Step 3: Push main — triggers GitHub Actions deployment**

```bash
git push origin main
```

Expected: `Docs` workflow runs, gh-pages branch updated, site live at `https://ehrlinger.github.io/hazard/`.

- [ ] **Step 4: Verify live site**

```bash
open https://ehrlinger.github.io/hazard/
```

---

## Self-Review

**Spec coverage check:**
- ✅ Doxygen infrastructure (Task 1)
- ✅ FORTRAN → C mapping (Task 0)
- ✅ Math helpers (Task 2) — all 11 `hzd_ln_*` + dlgaob
- ✅ G1/G3 shaping functions (Task 3)
- ✅ Phase transform functions (Task 4)
- ✅ Log-likelihood (Task 5)
- ✅ Global state / structures.h (Task 6)
- ✅ Error handling architecture (Task 7)
- ✅ Entry points hazard.c / hazpred.c (Task 8)
- ✅ I/O boundary xportHandler.c (Task 9)
- ✅ GitHub Pages deployment (Task 1, Step 11–13; Task 10)

**Not covered (acceptable — out of scope for this plan):**
- `src/optim/` internals (optimizer is 3rd-party BFGS; math is well-known)
- `src/vars/stepw.c` (stepwise selection detail)
- `src/vcov/` (variance-covariance computation detail)
- SAS utility macros (documented separately in `docs/vignettes/macros.md`)

These can be a follow-on plan once the mathematical core is documented.
