---
title: "User Guides"
description: "Full procedure reference documentation for PROC HAZARD, PROC HAZPRED, and parametric hazard analysis"
---

# User Guides

Three reference documents cover the complete HAZARD system. Each is checked in as Markdown alongside the other vignettes.

> Rendered HTML, PDF, PostScript, and DOC builds of these manuals are not currently shipped. The Markdown files below carry the authoritative content; the corresponding entries in `docs/user_guides/` only hold build scaffolding.

---

## Overview of Procedures: Parametric Analysis of Time-Related Events

Two SAS-interfaced procedures — PROC HAZARD and PROC HAZPRED — are described. These constitute a complete analysis system for generalized, fully parametric censored-data analysis and display.

**Contents:**

- Decomposition of the distribution of times to an event into up to 3 overlapping phases of hazard (competing risks)
- Each phase scaled by a parametric function of concomitant variables
- Full equations and general methods
- Conservation of Events theorem
- Relationship to non-parametric and semi-parametric methods

> **Start here** if you are new to parametric hazard analysis. The overview provides the mathematical foundation that the procedure-specific documents assume.

Read: [overview.md](overview.md)

---

## The HAZARD Procedure

Complete syntax reference for PROC HAZARD, the parametric analysis procedure for time-related events.

**Contents:**

- All statement syntax (`PROC HAZARD`, `DATA=`, `TIME`, `EVENT`, `EARLY`, `CONSTANT`, `LATE`, `BY`, `WEIGHT`, `RESTRICT`, `STEPWISE`, etc.)
- Option descriptions and defaults
- Shaping parameter specifications (`DELTA=`, `THALF=`, `NU=`, `M=`, `FIX*` options)
- Optimization method selection (`QUASI`, `STEEPEST`, `NEWTON`)
- Output dataset structure
- Examples

Read: [hazard.md](hazard.md)

---

## The HAZPRED Procedure

Complete syntax reference for PROC HAZPRED, the prediction procedure. HAZPRED calculates time-related estimates from a parametric equation previously established by PROC HAZARD.

**Contents:**

- Reading and using HAZARD parameter estimate datasets
- Prediction statement syntax
- Specifying covariate values for patient-specific predictions
- Output dataset structure (survival, cumulative hazard, hazard rate at specified times)
- Examples

Read: [hazpred.md](hazpred.md)

---

## Internal Developer Documentation

The following documents are not part of the original HAZARD distribution and are intended for developers and contributors:

| Document | Description |
|---|---|
| [Claude_CODEBASE_ANALYSIS.md](../Claude_CODEBASE_ANALYSIS.md) | Architecture overview: modules, data flow, entry points, red flags |
| [Claude_MODERNIZATION_GUIDE.md](../Claude_MODERNIZATION_GUIDE.md) | C modernization roadmap: context structs, type system, error handling |
| [Claude_R_MIGRATION_GUIDE.md](../Claude_R_MIGRATION_GUIDE.md) | R migration guide: functional patterns, validation tests, breaking changes |

---

*Previous: [SAS Utility Macros](macros.md) · Back to [Documentation Index](README.md)*

*Contact: [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org)*
