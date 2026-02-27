---
title: "Analysis Examples"
description: "Annotated walkthrough of all included example programs: AVC, OMC, CABGKUL, and VALVES datasets"
---

# Analysis Examples

The `examples/` directory contains a series of example SAS programs covering a range of hazard analysis tasks. Each program is well-commented and designed to guide a new user through both the procedures and the analytical thinking required.

**Location:** `$HZEXAMPLES` (`$HAZARD/examples`)

---

## Datasets

### AVC — Atrioventricular Septal Defects

**File:** `$HZEXAMPLES/data/avc`
**N = 310 observations, 70 events, 240 right-censored**

A dataset from repair of atrioventricular septal defects (AV canal), a form of congenital heart disease. The spectrum of lesions ranges from partial through intermediate to complete forms. With this sample size, the data show a continuous spectrum rather than three isolated forms.

At current follow-up, two hazard phases are identifiable: an **early** post-operative phase and a **constant** phase. A late phase will likely emerge with longer follow-up (if only from old age). The actual distribution of events and duration of follow-up dictate how many phases are found.

> "Phases" is used as a more clinically understandable term than "mixture distribution components."

The dataset is read from a flat file for illustration; in practice it would be a SAS dataset built from raw data.

---

### OMC — Thromboembolism (Repeated Events)

**File:** `$HZEXAMPLES/data/omc`

A dataset with a **repeated event** (thromboembolism) and evaluation of event severity. Used to illustrate two approaches to repeated-event data:

- **Longitudinal segmentation** with left censoring (straight repeated event methodology)
- **Modulated renewal process** (more interpretable when valid)

The weighted outcome variant treats the event as a weighted variable using the `WEIGHT` statement.

---

### CABGKUL — Coronary Artery Bypass Grafting

**File:** `$HZEXAMPLES/data/cabgkul`

A coronary artery bypass grafting dataset. Events include death, return of angina, and reintervention. Useful for practicing fitting a variety of hazard shapes across multiple events. `hz.deadp.KUL` is an annotated setup for the death event.

---

### VALVES — Valve Replacement

**File:** `$HZEXAMPLES/data/valves`

A valve replacement dataset used for:

- Fitting interesting hazard shapes
- Exploring which events to "bundle" together
- Exercising variable selection capabilities

The example `hm.deadp.VALVES` provides a sample multivariable setup.

---

## Example Programs

### Actuarial / Life Table

#### `ac.death.AVC.sas`
Life table for death after repair of atrioventricular septal defects. Starting point for exploring the AVC dataset non-parametrically before fitting a parametric model.

---

### Hazard Function Fitting

#### `hz.death.AVC.sas`
**Determine the hazard function for death after AVC repair.**

This is the primary fitting exercise — the main task that distinguishes HAZARD from Cox regression. Key points:

- Start with the **non-parametric cumulative hazard function** as a visual guide for initial parameter estimates.
- Begin with **simple models** and work up — it is easy to reach an overdetermined state.
- Final models are rarely complex. A large coronary dataset with 20 years of follow-up typically yields a late Weibull phase, a constant phase, and an early phase where `M` or `NU` simplifies to 0 or 1.
- When the optimizer terminates abnormally with `M` or `NU` driven to large negative values (e.g., E3 or E4), this is a **signal to fix that parameter** (e.g., `M=0 FIXM`).
- Three optimization algorithms are available (Newton, Quasi-Newton, Steepest Descent). Some problems require `STEEPEST`; others work with `QUASI` alone.

> "For me, after about 3000 of these exercises, the time varies from about 5 minutes to an hour." — Dr. Blackstone

#### `hz.deadp.KUL.sas`
Survival after primary isolated CABG. Hazard function for death. Annotated setup for the CABGKUL dataset.

#### `hz.te123.OMC.sas`
Hazard function for **repeated thromboembolic events** using the OMC dataset. Demonstrates longitudinal segmentation and left-censoring methodology.

#### `hz.tm123.OMC.sas`
Hazard function for **permanent morbidity** from thromboembolic events. Uses the `WEIGHT` statement to treat event severity as a weighted outcome.

#### `lg.death.AVC.sas`
Descriptive analysis of death after AVC repair. Explores **transformations of scale** to investigate which distributional family best fits the data.

---

### Multivariable Analysis

#### `hm.death.AVC.sas`
**Multivariable analysis of death after AVC repair** — the main variable selection exercise.

Key workflow points:

- Fix the **shaping parameters** during variable selection; only re-estimate them in a final run after risk factors are identified.
- Variable options: `/E` to exclude from fitting but still compute Q-statistics for entry; `/S` to force selection; `/I` to force inclusion.
- Variables can be organized in medically meaningful, correlated groups — no need to move them around.
- **Stepwise** selection is useful for 3–5 steps but automatic model building without clinical knowledge is not recommended.
- The final run should unfix the shaping parameters and verify they have not changed substantially.

#### `hm.deadp.VALVES.sas`
Multivariable analysis of valve replacement outcomes.

---

### Prediction and Strength-of-Effect Exploration

The `hp.*` (hazard prediction) and `hs.*` (hazard simulation/validation) programs exploit the fully parametric model by solving the fitted equation for specific covariate values.

#### `hp.death.AVC.sas`
Basic prediction from the AVC model.

#### `hp.death.AVC.hm1.sas`
**Patient-specific prediction: comparing two patients.**

Illustrates a major strength of the parametric model — solving the equation for the time-related survival of an individual patient. This example compares survival in two otherwise similar patients, one of whom has an additional major cardiac anomaly.

#### `hp.death.AVC.hm2.sas`
Displays the **strength of date of repair** as a risk factor in partial and complete forms of AV canal. Cross-time exploration of a continuous covariate's effect.

#### `hp.death.COMPARISON.sas`
Compares predicted outcomes from **PTCA, CABG, and medical treatment** — an example of using parametric predictions to compare results from analyses of different datasets.

---

### Model Validation

#### `hs.death.AVC.hm1.sas`
**Internal validation using conservation of events.**

Demonstrates two goodness-of-fit strategies unique to the fully parametric model:

1. Generate a survival curve for each patient; the mean should equal the overall survival curve. For any subset, the subset mean should fit the empiric survival curve (failures indicate omitted risk factors).
2. Sum the cumulative hazard per patient and compare predicted with observed event count (conservation of events check).

Also overlays parametric and non-parametric (Kaplan-Meier) estimates to verify model fit for specific subgroups.

---

## Workflow Recommendation

For a new dataset, the recommended analytical sequence is:

```
1. ac.*   → Life table (non-parametric baseline)
2. hz.*   → Fit hazard shaping parameters (underlying distribution)
3. hm.*   → Multivariable covariate selection (fix shaping params)
4. hp.*   → Predict and explore risk factor strengths
5. hs.*   → Validate model via conservation of events
```

---

*Previous: [Installation](installation.md) · Next: [SAS Utility Macros](macros.md)*

*Contact: [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org)*
