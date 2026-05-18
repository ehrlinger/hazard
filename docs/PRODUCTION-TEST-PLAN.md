# HAZARD Production Test Plan

> Created: 2026-05-13
> Status: Planning — collecting examples from Rajeswaran / Blackstone production library
> Scope: CCF-local only. Files live at `tests/production/` (gitignored). Nothing here goes
>        into the public repository.

---

## Why this exists

The public example library (18 paired `.sas`/`.lst` fixtures in `tests/examples/`) covers the
standard tutorial cases well. Production clinical analyses use model structures that don't
appear there: 4-phase decompositions, phase-specific covariate entry, shaping modifiers,
high-dimensional fits, and fractional case weights. Regressions in those paths are invisible
to the public test suite.

This plan builds a CCF-local corpus of production model fixtures that validates both:
- The C/SAS HAZARD binary (`hazard` v4.4.6+) against its own prior output
- The R `TemporalHazard` package against the same `.lst` reference outputs

A single `.lst` file validates both codebases simultaneously.

---

## Repository layout

```
tests/production/          ← gitignored; CCF-local only
  README.md                ← this plan, plus operator notes
  <name>.sas               ← SAS driver (safe to store — no PHI)
  <name>.lst               ← reference output from C binary (safe — summary stats only)
  <name>.notes.md          ← per-model notes (tolerances, known gaps, clinical context)
```

`.dta` XPORT files are **not** stored here — they contain patient-level data.
Drivers reference them via their production network path.

---

## Target model types

Priority order for collection from Rajeswaran / Blackstone library:

| Priority | Type | Why it matters | Status |
|---|---|---|---|
| 🔴 | **4-phase model** (operative + early + constant + late) | First real test of 4-phase CoE algebra and optimizer; `.hzr_select_fixmu_phase()` with N=4 untested | Awaiting examples |
| 🔴 | **`hzr_decompos()` near-boundary** | nu≈0, m≈0, t_half→0/∞ — sign dispatch has 6 cases; edge values may fall between branches | Awaiting examples |
| 🟡 | **Phase-specific covariates** | `hzr_phase(formula = ~)` in R ships but no production fixture exercises it | Awaiting examples |
| 🟡 | **Shaping modifiers** (`/S`, `/I`, `/E`) | Used in production fits; not in public library; R API deferred pending examples | Awaiting examples |
| 🟡 | **Weighted multiphase + covariates** | Covariate + weight combination untested in both codebases | Awaiting examples |
| 🟠 | **High-dimensional fit** (12+ free parameters) | Hessian stability; `hm.death.AVC.deciles` (13 params) is borderline | Awaiting examples |
| 🟠 | **Bootstrap with weights** | Non-unit weights through resampling + refit loop | Awaiting examples |
| ⚪ | **Interval censoring** | Code path present; no production fixture | Awaiting examples |
| ⚪ | **Competing risks with weights** | Greenwood variance with case weights | Awaiting examples |

---

## Candidate datasets / models

### Acute aortic dissection — 4-phase model

**Clinical structure:**
1. **Operative** (G1/CDF): immediate post-repair hazard, hours to days
2. **Early** (G1/CDF): subacute complications, weeks to ~90 days
3. **Constant** (G2): background mortality plateau
4. **Late** (G3): structural deterioration, false-lumen expansion, reoperation

**Data sources under consideration:**
- CCF internal Type A dissection repair series (via Blackstone library)
- IRAD (International Registry of Acute Aortic Dissection) published series
  — published MLEs could anchor a reproducibility vignette without raw data

**Parallel deliverable:** If a suitable public-domain reference exists, this
model becomes the first 4-phase example vignette in the R `TemporalHazard`
package.

### Hsich / UNOS — post-heart-transplant mortality

**Clinical structure:** 3-phase early + constant + late
**Data source:** SRTR Standard Analysis Files (requires DUA)
**Contact:** Eileen Hsich, CCF
**Parallel deliverable:** New dataset + vignette in `TemporalHazard`

### Sargent — death after CABG

**Clinical structure:** 3-phase (distinct from the Belgium/Leuven KUL dataset)
**Data source:** Published paper — extract LL + MLE table as reference
**Parallel deliverable:** Reproducibility vignette in `TemporalHazard`

---

## Collection procedure

1. Obtain `.sas` driver + `.lst` output from Rajes / Blackstone production library
2. Confirm `.lst` contains only summary statistics (no patient-level rows)
3. Place in `tests/production/<name>.sas` and `tests/production/<name>.lst`
4. Document tolerances and known gaps in `tests/production/<name>.notes.md`
5. Add a `skip_on_cran()`-guarded parity test in `TemporalHazard` pointing at the same `.lst`
6. After CCF hazard v4.4.6 install: run driver against installed binary, verify `.lst` matches

---

## Sequencing

```
Rajes meeting → collect .sas/.lst examples
  → install hazard v4.4.6 at CCF
  → run drivers against v4.4.6 binary
  → verify .lst matches → add to tests/production/
  → port to TemporalHazard parity fixtures
```

**CCF install is blocked on example collection.** Run examples first so the
v4.4.6 install can be immediately validated against real production models.

---

## Related documents

| Document | Location |
|---|---|
| Public corpus acceptance plan | `docs/VALIDATION_PLAN.md` |
| v4.4.6 implementation plan | `docs/superpowers/plans/2026-04-29-v4.4.6-implementation-plan.md` |
| TemporalHazard Phase 7 roadmap | `inst/dev/DEVELOPMENT-PLAN.md` (TemporalHazard repo) |
| Production validation special cases | `[[Projects/HAZARD-production-validation]]` (Obsidian) |
