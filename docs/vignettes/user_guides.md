---
title: "User Guides"
description: "Full procedure reference documentation for PROC HAZARD, PROC HAZPRED, and parametric hazard analysis"
---

# User Guides

Three reference documents cover the complete HAZARD system. Each is available in multiple formats in the `docs/user_guides/` directory.

---

## Overview of Procedures: Parametric Analysis of Time-Related Events

**Files:** `user_guides/overview.*` (HTML, PDF, PS, DOC)

Two SAS-interfaced procedures — PROC HAZARD and PROC HAZPRED — are described. These constitute a complete analysis system for generalized, fully parametric censored-data analysis and display.

**Contents:**

- Decomposition of the distribution of times to an event into up to 3 overlapping phases of hazard (competing risks)
- Each phase scaled by a parametric function of concomitant variables
- Full equations and general methods
- Conservation of Events theorem
- Relationship to non-parametric and semi-parametric methods

> **Start here** if you are new to parametric hazard analysis. The overview provides the mathematical foundation that the procedure-specific documents assume.

| Format | Link |
|---|---|
| HTML | [overview.html](../user_guides/overview.html) |
| PDF | [overview.pdf](../user_guides/overview.pdf) |
| PostScript | [overview.ps](../user_guides/overview.ps) |
| MS Word | [overview.doc](../user_guides/overview.doc) |

---

## The HAZARD Procedure

**Files:** `user_guides/hazard.*` (HTML, PDF, PS, DOC)

Complete syntax reference for PROC HAZARD, the parametric analysis procedure for time-related events.

**Contents:**

- All statement syntax (`PROC HAZARD`, `DATA=`, `TIME`, `EVENT`, `EARLY`, `CONSTANT`, `LATE`, `BY`, `WEIGHT`, `RESTRICT`, `STEPWISE`, etc.)
- Option descriptions and defaults
- Shaping parameter specifications (`DELTA=`, `THALF=`, `NU=`, `M=`, `FIX*` options)
- Optimization method selection (`QUASI`, `STEEPEST`, `NEWTON`)
- Output dataset structure
- Examples

| Format | Link |
|---|---|
| HTML | [hazard.html](../user_guides/hazard.html) |
| PDF | [hazard.pdf](../user_guides/hazard.pdf) |
| PostScript | [hazard.ps](../user_guides/hazard.ps) |
| MS Word | [hazard.doc](../user_guides/hazard.doc) |

---

## The HAZPRED Procedure

**Files:** `user_guides/hazpred.*` (HTML, PDF, PS, DOC)

Complete syntax reference for PROC HAZPRED, the prediction procedure. HAZPRED calculates time-related estimates from a parametric equation previously established by PROC HAZARD.

**Contents:**

- Reading and using HAZARD parameter estimate datasets
- Prediction statement syntax
- Specifying covariate values for patient-specific predictions
- Output dataset structure (survival, cumulative hazard, hazard rate at specified times)
- Examples

| Format | Link |
|---|---|
| HTML | [hazpred.html](../user_guides/hazpred.html) |
| PDF | [hazpred.pdf](../user_guides/hazpred.pdf) |
| PostScript | [hazpred.ps](../user_guides/hazpred.ps) |
| MS Word | [hazpred.doc](../user_guides/hazpred.doc) |

---

## Internal Developer Documentation

The following documents are not part of the original HAZARD distribution and are intended for developers and contributors:

| Document | Description |
|---|---|
| [CODEBASE_ANALYSIS.md](../CODEBASE_ANALYSIS.md) | Architecture overview: modules, data flow, entry points, red flags |
| [MODERNIZATION_GUIDE.md](../MODERNIZATION_GUIDE.md) | C modernization roadmap: context structs, type system, error handling |
| [R_MIGRATION_GUIDE.md](../R_MIGRATION_GUIDE.md) | R migration guide: functional patterns, validation tests, breaking changes |

---

*Previous: [SAS Utility Macros](macros.md) · Back to [Documentation Index](README.md)*

*Contact: [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org)*
