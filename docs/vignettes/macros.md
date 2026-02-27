---
title: "SAS Utility Macros"
description: "Reference for all SAS helper macros included in $HAZARD/macros"
---

# SAS Utility Macros

The `macros/` directory (`$MACROS`) contains a set of SAS helper macros used in the example programs and available for use in your own analyses.

**Location:** `$HAZARD/macros`

---

## Variable Selection

### `bootstrap.hazard.sas`

Implements **bootstrap "bagging" for variable selection** with PROC HAZARD.

One of the most important features exploited by this macro is the `RESTRICT` statement, which links closely related variables (e.g., different transformations of age, or all variables related to patient size) so that only one from each group can be selected in any given bootstrap sample. A count of unique variable occurrences across bootstrap samples can then be made manually or automatically.

**Use case:** Stabilizing variable selection in small-to-moderate datasets where stepwise procedures may be unstable.

---

## Goodness-of-Fit

### `chisqgf.sas`

Chi-square **goodness-of-fit** macro for comparing expected and observed events. Called by the `hs.*` simulation/validation jobs in the examples directory.

**Use case:** Formal statistical test of model fit after visual inspection.

---

### `deciles.hazard.sas`

Sorts patients by **predicted survival at a specified time point** (set via `TIME=`), divides them into groups (set via `GROUPS=`), and examines expected vs. observed events within each group.

**Use case:** Examining calibration of a multivariable model across the range of predicted risk.

---

## Hazard Visualization

### `hazplot.sas`

Determines **goodness-of-fit of the fitted hazard function** to the distribution of event times. Compares the parametric estimate against the Kaplan-Meier non-parametric estimate using multiple transformations of the time scale to examine both early and late time frames.

Also reports:

- Observed vs. predicted event counts (departure is a key indicator of lack of fit)
- Number of events associated with each hazard phase (to avoid overdetermined models)

**Use case:** Diagnosing model fit before proceeding to multivariable analysis.

---

## Non-Parametric Estimates

### `kaplan.sas`

Kaplan-Meier life table analysis. The primary difference from standard implementations is the **confidence limit calculation**, which is more exact than the Greenwood formula.

**Use case:** Non-parametric baseline estimate before fitting a parametric model.

---

### `nelsonl.sas`

Calculates **non-parametric estimates for weighted repeated events** using the algorithm of Wayne Nelson. Unlike Kaplan-Meier, the Nelson estimator is defined in the **cumulative hazard domain**, making it particularly suited to:

- Repeated events
- Weighted events (Nelson calls the weighted form the "cost function")

**Use case:** Non-parametric baseline for OMC-style analyses with repeating or weighted outcomes.

---

### `nelsont.sas`

Companion to `kaplan.sas` with identical outputs and variable names, but calculates the life table by the **Nelson method** rather than Kaplan-Meier. When an event would drive the Kaplan-Meier estimate to zero, this macro instead sets the value consistent with the cumulative hazard function.

**Use case:** Life table display where late-time behavior matters more than absolute zero-probability events.

---

## Variable Calibration

### `logit.sas`

Calibrates a **continuous or ordinal variable** against the event outcome. Groups patients into roughly equal-sized bins, calculates observed event rates, and displays results on the appropriate risk scale for your analysis (logit for logistic regression, log-cumulative-hazard for time-related analyses).

Ideally, a well-calibrated variable shows a linear relationship on the risk scale. Examining multiple transformations (log, square root, etc.) reveals the best functional form.

**Use case:** Variable transformation investigation before entry into a multivariable model.

---

### `logitgr.sas`

Similar to `logit.sas` but groups patients by a **specified grouping variable** rather than equal bin sizes. Otherwise produces the same output.

**Use case:** Calibration display when natural groupings exist (e.g., age decades, disease severity categories).

---

## Plotting

### `plot.sas`

A large macro for producing publication-quality plots.

> **Note:** Due to changes in SAS graph modules between version 6 and version 8, this macro is not currently recommended for SAS v8+. Updated versions of the affected sections are in progress.

**Use case:** Final publication figures (SAS v6 only until updated).

---

## Usage

All macros are accessible within SAS after [configuring the `sasautos` path](installation.md#sas-configuration) to include `!HAZAPPS`. Call any macro directly:

```sas
%hazplot(data=my_estimates, time=followup, event=death);
%kaplan(data=mydata, time=t, censor=c, group=treatment);
%deciles.hazard(data=predictions, time=5, groups=10);
```

Refer to the individual macro source files in `$MACROS` and to the annotated example programs in `$HZEXAMPLES` for full parameter documentation and usage patterns.

---

*Previous: [Examples](examples.md) · Next: [User Guides](user_guides.md)*

*Contact: [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org)*
