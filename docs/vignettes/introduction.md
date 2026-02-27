---
title: "Introduction to Hazard Function Technology"
description: "Conceptual overview of parametric hazard analysis and the motivation for multi-phase models"
---

# Introduction to Hazard Function Technology

## What is a Hazard Function?

Some of the most relevant outcomes of medical procedures — or the life-history of machines — are time-related events. The raw data for such events is the time interval between a defined **time zero** (t = 0) and the occurrence of the event. The distribution of these time intervals can be visualized as:

- A **cumulative distribution** (or its complement, the **survivorship function**)
- A **probability density function** (histogram form)
- A **rate of occurrence** — the most natural domain for studying biological or mechanical phenomena across time

This rate of occurrence is the **hazard function**. The term was introduced by John Graunt in the 17th century, borrowing from the language of dice games. It is also known as the *force of mortality* in demography, and as the inverse of Mills ratio in finance.

The methodology is applicable to any positively distributed variable — not only survival times.

## Why Parametric?

The nature of living things and real machines is such that lifetimes and other time-related events often follow low-order distributions. This supports a **parametric** approach: fitting a compact, interpretable functional form rather than a purely non-parametric one.

The parametric approach taken in the HAZARD procedures — developed in the early 1980s at the University of Alabama at Birmingham — uses a **decompositional** strategy. The distribution of intervals is viewed as consisting of one or more overlapping **phases** (early, constant, and late), additive in hazard (competing risks). A generic functional form is used for each phase that encompasses a large number of hierarchically nested models.

## Multi-Phase Hazard

The hazard function is decomposed as:

```
h(t | X) = μ₁(X) · SG₁(t)    [early phase]
          + μ₂(X)              [constant phase]
          + μ₃(X) · SG₃(t)    [late phase]
```

Each phase is **scaled by a log-linear function of concomitant information** (covariates):

```
μⱼ(X) = exp(intercept_j + β_j' X)
```

This allows the model to be **non-proportional in hazards** — an assumption often made with Cox models but frequently unrealistic in clinical data.

## Enrichments Over Standard Survival Analysis

The HAZARD model handles three common complexities beyond simple right-censored data:

1. **Censoring types**: Right censoring, left censoring, and interval censoring are all supported.
2. **Repeating events**: The observation window can be longitudinally segmented, which also accommodates a wide class of time-varying covariates — specifically those that change at discrete intervals.
3. **Weighted events**: Events can carry a positive weight (such as cost or severity), enabling analysis of, e.g., time-related repeated cost data.

At its most general, HAZARD models time-related repeating weighted cost data with time-varying covariates and a non-proportional hazard structure.

## Conservation of Events

A key theoretical property of the model is the **Conservation of Events theorem** (Turner et al.): one of the phase intercepts can be solved in closed form from a constraint equation, reducing optimization dimensionality by one. This substantially improves numerical stability.

## Relationship to Cox Regression

| Feature | Cox (semi-parametric) | HAZARD (parametric) |
|---|---|---|
| Proportional hazards | Required | Not required |
| Baseline hazard | Unspecified | Fully parametric |
| Multiple hazard phases | Not supported | Supported |
| Patient-specific prediction | Approximate | Exact (plug in covariates) |
| Event conservation check | Not available | Built-in |
| Interval censoring | Not standard | Supported |

The fully parametric nature of HAZARD means that once parameter estimates are available, the resulting equation can be solved for any set of risk factors — enabling patient-specific survival prediction and goodness-of-fit checks via conservation of events.

---

*Next: [Installation](installation.md)*

*Contact: [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org)*
