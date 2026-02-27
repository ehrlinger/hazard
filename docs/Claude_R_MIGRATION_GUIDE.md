# HAZARD → R Migration Guide

> Generated: 2026-02-27
> Source: Deep static analysis of C implementation (~33K LOC)
> Strategy: Pure R with functional patterns first; C/C++ performance layer later

---

## Mathematical Summary (Read This First)

HAZARD fits a **three-phase parametric hazard model**:

```
h(t) = MU_1 * SG1(t; δ, ρ, ν, M)   [early phase]
     + MU_2 * 1                       [constant phase]
     + MU_3 * SG3(t; τ, γ, α, η)    [late phase]
```

Each `MU_j = exp(intercept_j + β_j' X)`. The **Conservation of Events theorem** means one intercept is solvable in closed form, reducing the optimization by one dimension. This is the most mathematically distinctive part of the system — everything else is relatively standard survival analysis.

---

## 1. Migration Checklist (Riskiest First)

### Phase 1 — Numerical Core: Shaping Functions G1/SG1 (HIGHEST RISK)
The early-phase model has **6 cases** driven by signs of `M` and `ν`. Getting even one case wrong produces silently incorrect hazard values — no crash, no warning, just wrong estimates. This is the hardest math and the riskiest translation.

- [ ] **1.1** Implement `ln_base_transform(t, delta)` — `B(t) = (exp(δt)-1)/δ` with `B(t)=t` when `δ=0`
- [ ] **1.2** Implement `ln_g1_sg1(t, delta, rho, nu, m)` returning `list(ln_g1, ln_sg1)` for all 6 cases (see §2)
- [ ] **1.3** Implement `set_rho_from_thalf(thalf, nu, m)` — closed-form RHO from half-time (4 sub-cases)
- [ ] **1.4** Implement `ln_g3_sg3(t, tau, gamma, alpha, eta)` — late phase analogue
- [ ] **1.5** Validate each case individually against C reference outputs before proceeding (see §5, V1)

### Phase 2 — Log-Likelihood: Core Loop (HIGH RISK)
The likelihood formula involves interval censoring, starting times, and extended-precision summation. A sign error or wrong branch costs you nothing at compile time.

- [ ] **2.1** Implement `obs_loglik(obs, params)` — single-observation log-likelihood (see §2, Pattern B)
- [ ] **2.2** Implement `total_loglik(data, params)` — vectorized sum using `purrr::map_dbl` + `sum()`
- [ ] **2.3** Handle all three censoring types: exact events (C1), right-censored (C2), interval-censored (C3)
- [ ] **2.4** Handle starting times (STIME > 0) for repeating events / time-varying covariates
- [ ] **2.5** Validate: log-likelihood at known parameters matches C output to 6 decimal places (see §5, V2)

### Phase 3 — Conservation of Events (HIGH RISK)
This is mathematically subtle. The scaling factor `FACTOR` is solved in closed form from a constraint equation. Implementing this wrong means the optimizer starts from a different point or wanders — results diverge silently.

- [ ] **3.1** Implement `compute_sumcf(data, params)` — accumulate `Σ (C1+C2+C3) * CF_j(t_i)` per phase
- [ ] **3.2** Implement `solve_conservation_factor(events, sumcf)` — closed-form `FACTOR = Σ(C1+C3) / Σ(C1+C2+C3)*CF_j`
- [ ] **3.3** Implement `select_fixed_phase(sumcf)` — choose the phase with largest cumulative hazard
- [ ] **3.4** Wire conservation into the objective function so the fixed intercept is excluded from optimization
- [ ] **3.5** Validate: conserved events count matches reference (`Events conserved ≈ 70.000`)

### Phase 4 — Objective Function + Optimizer (MEDIUM RISK)
Wrapping the likelihood into `optim()` is straightforward. The risk is in parameter indexing — the C code uses a `STATUS` array to include/exclude parameters dynamically during stepwise selection.

- [ ] **4.1** Implement `make_objective(data, fixed_params, status)` — returns a closure over `(theta) -> -loglik`
- [ ] **4.2** Wire to `optim(method = "BFGS")` with numerical gradient, or `nlm()` with analytic gradient
- [ ] **4.3** Implement parameter packing/unpacking: active `theta` vector ↔ full parameter list (see §2, Pattern D)
- [ ] **4.4** Validate: optimizer converges to same parameter estimates as C code (see §5, V3)

### Phase 5 — Covariate Scaling: MU Computation (MEDIUM RISK)
Each phase has its own intercept and covariate vector. The linear predictor feeds into `exp()` to give `MU_j`. This is standard GLM-style, but must match the C indexing exactly.

- [ ] **5.1** Implement `compute_log_mu(params, X)` — returns `list(log_mu1, log_mu2, log_mu3)`
- [ ] **5.2** Implement `compute_cumhaz(t, params, X)` — total CF and HF across all phases
- [ ] **5.3** Validate against `hzr_calc_loge_mu` + `hzr_cum_haz_func` outputs

### Phase 6 — Stepwise Variable Selection (MEDIUM RISK)
Wald tests and likelihood-ratio tests for entry/exit. The risk here is reproducing the exact stopping criterion and oscillation prevention.

- [ ] **6.1** Implement `wald_test(coef, se)` — Z-score and p-value for exclusion test
- [ ] **6.2** Implement `score_test(data, params, candidate_var)` — for inclusion
- [ ] **6.3** Implement `stepwise_select(data, params, config)` returning a history of moves (see §2, Pattern E)
- [ ] **6.4** Add oscillation guard: track moves per variable, cap at `mxmove`
- [ ] **6.5** Validate: variable entry/exit sequence matches C reference (see §5, V4)

### Phase 7 — Data I/O: XPORT + Output Formatting (LOW RISK)
The SAS XPORT reader and report formatter are completely independent of the math.

- [ ] **7.1** Use `foreign::read.xport()` or `haven::read_xpt()` for XPORT input — do not rewrite this
- [ ] **7.2** Implement output tables matching the `.lst` format (see §5, V5)
- [ ] **7.3** Implement a user-facing `hazard_fit()` function with a formula interface (see §2, Pattern F)

---

## 2. Code Transformations for Common Patterns

### Pattern A: Six-Case G1/SG1 Shaping Function

This is the hardest translation. Six cases driven by `sign(M)` and `sign(nu)`.

**C (setg1.c — implicit, via g1flag 1-6):**
```c
// Case 1: M > 0, NU > 0
LN_G1  = mInvM  * log(1 + pow(B/rho, mInvNu));
LN_SG1 = mInvNu * log(B/rho) + LN_G1 + dBt - log(m*nu*rho);
```

**R — functional, one function per case, dispatched by sign:**
```r
# helpers/shaping_functions.R

ln_base_transform <- function(t, delta) {
  # B(t) = (exp(delta*t) - 1) / delta, with B(t) = t when delta = 0
  if (abs(delta) < .Machine$double.eps) return(log(t))
  log(expm1(delta * t)) - log(delta)    # log((exp(delta*t)-1)/delta)
}

d_ln_base_transform <- function(t, delta) {
  # d/dt of ln(B(t)) — used in SG1 derivative term
  if (abs(delta) < .Machine$double.eps) return(0)
  delta * t - log(expm1(delta * t))     # log(delta*t / (exp(delta*t) - 1))
}

# Each case returns list(ln_g1 = ..., ln_sg1 = ...)

.g1_case1 <- function(ln_bt, ln_rho, nu, m, dbt) {
  # M > 0, nu > 0 — Negative Generic
  x     <- ln_bt - ln_rho                       # log(B(t)/rho)
  ln_g1 <- (-1/m) * log1p(exp(x / (-nu)))       # log(1 + (B/rho)^(-1/nu)) * (-1/m)
  list(
    ln_g1  = ln_g1,
    ln_sg1 = x / (-nu) + ln_g1 + dbt - log(m * nu) - ln_rho
  )
}

.g1_case2 <- function(ln_bt, ln_rho, nu, m, dbt) {
  # M = 0, nu > 0 — Exponential limiting case
  x     <- ln_bt - ln_rho
  ln_g1 <- -exp(x / (-nu))
  list(
    ln_g1  = ln_g1,
    ln_sg1 = ln_g1 + x / (-nu) + dbt - log(nu) - ln_rho
  )
}

.g1_case3 <- function(ln_bt, ln_rho, nu, m, dbt) {
  # M < 0, nu > 0 — Positive Generic
  x      <- ln_bt - ln_rho
  ln_x   <- log1p(exp(x)) / nu                  # log(1 + (B/rho)^(1/nu)) — NOTE sign flip
  ln_g1  <- (-1/m) * log1p(-exp(ln_x))
  list(
    ln_g1  = ln_g1,
    ln_sg1 = x / (-nu) + ln_g1 + dbt - log(-m * nu) - ln_rho
  )
}

.g1_case4 <- function(ln_bt, ln_rho, nu, m, dbt) {
  # M < 0, nu = 0 — Positive Limiting
  bt_over_rho <- exp(ln_bt - ln_rho)
  ln_g1 <- (-1/m) * log1p(-exp(-bt_over_rho))
  list(
    ln_g1  = ln_g1,
    ln_sg1 = ln_g1 - bt_over_rho + dbt - log(-m) - ln_rho
  )
}

.g1_case5 <- function(ln_bt, ln_rho, nu, m, dbt) {
  # M > 0, nu < 0 — Mixed Generic
  x      <- ln_bt - ln_rho
  term_m <- log1p(exp(x / (-nu)))
  ln_g1  <- log1p(-exp(term_m / m))
  list(
    ln_g1  = ln_g1,
    ln_sg1 = x / (-nu) + ln_g1 + dbt - log(-m * nu) - ln_rho
  )
}

.g1_case6 <- function(ln_bt, ln_rho, nu, m, dbt) {
  # M = 0, nu < 0 — Limiting Case 3
  x     <- ln_bt - ln_rho
  ln_g1 <- log1p(-exp(-exp(x / (-nu))))
  list(
    ln_g1  = ln_g1,
    ln_sg1 = x / (-nu) - exp(x / (-nu)) + dbt - log(-nu) - ln_rho
  )
}

ln_g1_sg1 <- function(t, delta, rho, nu, m) {
  ln_bt  <- ln_base_transform(t, delta)
  ln_rho <- log(rho)
  dbt    <- d_ln_base_transform(t, delta)

  case <- dplyr::case_when(
    m  > 0 & nu > 0 ~ 1L,
    m == 0 & nu > 0 ~ 2L,
    m  < 0 & nu > 0 ~ 3L,
    m  < 0 & nu == 0 ~ 4L,
    m  > 0 & nu < 0 ~ 5L,
    m == 0 & nu < 0 ~ 6L,
    TRUE ~ NA_integer_
  )

  switch(case,
    `1` = .g1_case1(ln_bt, ln_rho, nu, m, dbt),
    `2` = .g1_case2(ln_bt, ln_rho, nu, m, dbt),
    `3` = .g1_case3(ln_bt, ln_rho, nu, m, dbt),
    `4` = .g1_case4(ln_bt, ln_rho, nu, m, dbt),
    `5` = .g1_case5(ln_bt, ln_rho, nu, m, dbt),
    `6` = .g1_case6(ln_bt, ln_rho, nu, m, dbt),
    stop("ln_g1_sg1: invalid (m, nu) combination")
  )
}
```

---

### Pattern B: Single-Observation Log-Likelihood

**C (setlik.c / SETLIK_obs_loop — implicit structure):**
```c
llike_i = -(c1w + c2 + c3w) * (cumhaz - cumhst)
         + c1w * log(hazard)        // if c1 > 0
         + c3w * log(dcf / dt);    // if c3 > 0, interval censored
```

**R — pure function, no side effects:**
```r
# core/loglik.R

obs_loglik <- function(time, c1, c2, c3, weight, ctime, stime,
                       cumhaz_fn, hazard_fn) {
  # cumhaz_fn(t) and hazard_fn(t) are closures over current params + covariates

  c1w <- c1 * weight
  c3w <- c3 * weight

  ch_t   <- cumhaz_fn(time)
  ch_st  <- if (stime > 0) cumhaz_fn(stime) else 0

  # Base term: all subjects contribute time at risk
  ll <- -(c1w + c2 + c3w) * (ch_t - ch_st)

  # Events term
  if (c1 > 0) ll <- ll + c1w * log(hazard_fn(time))

  # Interval censoring term
  if (c3 > 0 && ctime > 0) {
    ch_ct <- cumhaz_fn(ctime)
    dcf   <- ch_t - ch_ct
    dt    <- time - ctime
    if (dcf <= 0 || dt <= 0) stop("obs_loglik: invalid interval censoring window")
    ll <- ll + c3w * log(dcf / dt)
  }

  ll
}

total_loglik <- function(data, params) {
  # data: data.frame with columns time, c1, c2, c3, weight, ctime, stime, [covariates]
  # params: list(early=list(...), constant=list(...), late=list(...))

  # Build phase functions once for the whole dataset
  cf_fn  <- make_cumhaz_fn(params)
  haz_fn <- make_hazard_fn(params)

  purrr::pmap_dbl(data, function(time, c1, c2, c3, weight, ctime, stime, ...) {
    covs <- list(...)
    obs_loglik(
      time, c1, c2, c3, weight, ctime, stime,
      cumhaz_fn = \(t) cf_fn(t, covs),
      hazard_fn = \(t) haz_fn(t, covs)
    )
  }) |> sum()
}
```

---

### Pattern C: Conservation of Events

**C (setcoe.c — implicit across many functions):**
```c
FACTOR = QSUMCZ / SUMCF[FIXMU];   // closed-form solve
// Then multiply intercept[FIXMU] by FACTOR
```

**R — explicit, transparent:**
```r
# core/conservation.R

compute_phase_sumcf <- function(data, params) {
  # Returns vector of length 3: sum of (C1+C2+C3)*CF_j(t_i) for each phase
  purrr::pmap(data, function(time, c1, c2, c3, weight, ctime, stime, ...) {
    covs      <- list(...)
    total_obs <- c1 + c2 + c3
    cf_phases <- phase_cumhaz(time, params, covs)   # list of length 3
    purrr::map_dbl(cf_phases, \(cf) total_obs * cf)
  }) |>
    purrr::transpose() |>
    purrr::map_dbl(purrr::reduce, `+`)
}

solve_conservation_factor <- function(data, params, fixed_phase) {
  total_events <- sum(data$c1 + data$c3)          # Σ(C1+C3) — events only
  sumcf        <- compute_phase_sumcf(data, params)
  total_events / sumcf[[fixed_phase]]
}

select_fixed_phase <- function(sumcf) which.max(sumcf)

apply_conservation <- function(params, factor, fixed_phase) {
  # Multiply the fixed intercept by FACTOR in log space (add log(factor))
  params[[fixed_phase]]$log_intercept <-
    params[[fixed_phase]]$log_intercept + log(factor)
  params
}
```

---

### Pattern D: Parameter Packing/Unpacking

The C code uses a `STATUS` array to dynamically include/exclude parameters. In R, use a named list and a `status` logical vector — then close over them in the objective function.

```r
# core/params.R

make_param_spec <- function(phases = c("early", "constant", "late"),
                            covariates = character(0)) {
  # Defines which parameters exist and their initial status
  list(
    early = list(
      log_mu    = list(init = 0, status = "active"),
      delta     = list(init = 0, status = "active"),
      thalf     = list(init = 1, status = "active"),
      nu        = list(init = 1, status = "active"),
      m         = list(init = 1, status = "fixed"),    # often fixed by user
      betas     = purrr::set_names(
        purrr::map(covariates, \(v) list(init = 0, status = "active")), covariates
      )
    ),
    constant = list(log_mu = list(init = 0, status = "active")),
    late     = list(
      log_mu = list(init = 0, status = "active"),
      tau    = list(init = 0, status = "active"),
      # ...
    )
  )
}

pack_theta <- function(spec) {
  # Extract only active parameters into a named numeric vector
  purrr::keep(purrr::flatten(spec), \(p) p$status == "active") |>
    purrr::map_dbl(\(p) p$init) |>
    unname()
}

unpack_theta <- function(theta, spec) {
  # Fill active slots back from theta vector
  active_idx <- 0L
  purrr::modify_deep(spec, \(p) {
    if (!is.list(p) || is.null(p$status)) return(p)
    if (p$status == "active") {
      active_idx <<- active_idx + 1L
      p$value <- theta[[active_idx]]
    }
    p
  })
}

make_objective <- function(data, spec, fixed_phase = NULL) {
  function(theta) {
    params <- unpack_theta(theta, spec)

    # Apply conservation of events if a fixed phase is designated
    if (!is.null(fixed_phase)) {
      factor <- solve_conservation_factor(data, params, fixed_phase)
      params <- apply_conservation(params, factor, fixed_phase)
    }

    -total_loglik(data, params)   # minimise negative log-likelihood
  }
}
```

---

### Pattern E: Stepwise Variable Selection

**C (stepw.c) — mutates global STATUS array via side effects**

**R — returns a history list, no mutation:**
```r
# core/stepwise.R

stepwise_select <- function(data, initial_spec, config = list(
  alpha_enter = 0.05,
  alpha_exit  = 0.05,
  max_moves   = 5L
)) {
  spec    <- initial_spec
  history <- list()
  moves   <- purrr::set_names(integer(length(spec$covariates)), spec$covariates)

  repeat {
    fit    <- fit_model(data, spec)
    action <- decide_action(fit, spec, config, moves)

    if (action$type == "none") break

    history <- c(history, list(action))
    moves[[action$variable]] <- moves[[action$variable]] + 1L
    spec     <- update_spec(spec, action)        # returns new spec, no mutation
  }

  list(final_spec = spec, history = history, final_fit = fit)
}

decide_action <- function(fit, spec, config, moves) {
  # 1. Test for exclusion (Wald test on active variables)
  active_vars <- get_active_vars(spec)
  exit_tests  <- purrr::map(active_vars, \(v) wald_test(fit$coef[[v]], fit$se[[v]]))
  worst_p     <- max(purrr::map_dbl(exit_tests, "p_value"))

  if (worst_p > config$alpha_exit) {
    worst_var <- active_vars[[which.max(purrr::map_dbl(exit_tests, "p_value"))]]
    if (moves[[worst_var]] < config$max_moves)
      return(list(type = "remove", variable = worst_var, p = worst_p))
  }

  # 2. Test for inclusion (score test on inactive variables)
  inactive_vars  <- get_inactive_vars(spec)
  entry_tests    <- purrr::map(inactive_vars, \(v) score_test(data, fit, v))
  best_p         <- min(purrr::map_dbl(entry_tests, "p_value"))

  if (best_p < config$alpha_enter) {
    best_var <- inactive_vars[[which.min(purrr::map_dbl(entry_tests, "p_value"))]]
    if (moves[[best_var]] < config$max_moves)
      return(list(type = "enter", variable = best_var, p = best_p))
  }

  list(type = "none")
}
```

---

### Pattern F: User-Facing Formula Interface

**R — tidyverse-style API that replaces the SAS PROC syntax:**
```r
# hazard_fit.R

hazard_fit <- function(
  data,
  time,
  status,            # 0 = censored, 1 = event
  early   = NULL,    # formula: ~ age + sex
  constant = NULL,   # formula: ~ age
  late    = NULL,    # formula: ~ age + sex
  early_params = list(delta = 0, m = 1),   # fixed shape params
  selection = list(method = "stepwise", alpha_enter = 0.05, alpha_exit = 0.10),
  ...
) {
  # 1. Parse data into internal format
  obs <- prepare_observations(
    data,
    time     = rlang::enquo(time),
    status   = rlang::enquo(status),
    formulas = list(early = early, constant = constant, late = late)
  )

  # 2. Build initial parameter spec
  spec <- make_param_spec(
    phases     = detect_active_phases(early, constant, late),
    covariates = extract_covariates(early, constant, late),
    fixed      = early_params
  )

  # 3. Conservation of events setup
  fixed_phase <- select_fixed_phase(
    compute_phase_sumcf(obs, init_params(spec))
  )

  # 4. Run stepwise selection (or fit directly)
  result <- if (selection$method == "stepwise") {
    stepwise_select(obs, spec, selection)
  } else {
    list(final_spec = spec, final_fit = fit_model(obs, spec))
  }

  # 5. Return S3 object
  structure(
    list(
      coefficients = extract_coefs(result$final_fit),
      loglik       = result$final_fit$loglik,
      converged    = result$final_fit$converged,
      n_events     = sum(obs$c1),
      conserved    = sum(obs$c1 + obs$c3),   # should match n_events
      selection_history = result$history,
      data         = obs,
      spec         = result$final_spec,
      call         = match.call()
    ),
    class = "hazard_fit"
  )
}

# S3 methods
print.hazard_fit  <- function(x, ...) { ... }
summary.hazard_fit <- function(object, ...) { ... }
predict.hazard_fit <- function(object, newdata, times, ...) { ... }
```

---

## 3. Breaking Changes to Watch For

### B1 — `log1p` vs direct `log` for small arguments (SILENT)
The C code uses custom helpers (`hzd_ln_eX_min1`, `hzd_ln_1min_e_negeX`) for numerical stability when arguments are near zero. R's `log1p()` and `expm1()` do this correctly — but only if you use them. `log(1 + x)` when `x` is near zero will produce the same catastrophic cancellation as naive C. Always use `log1p` / `expm1`.

### B2 — Extended precision accumulation (SILENT)
The C code accumulates log-likelihoods in `xtended` (extended precision, potentially `long double`) using `QEXTD`/`QTOD`. R's `double` is IEEE 754 64-bit. For large datasets with many small contributions, the final log-likelihood sum may differ by a small amount. This is acceptable unless you are reproducing exact C output — in which case use `Rmpfr` for that comparison only.

### B3 — Conservation of events: floating-point FIXMU selection (SILENT)
The fixed phase (`FIXMU`) is chosen as `which.max(SUMCF)`. If two phases have nearly identical `SUMCF`, the selected phase may differ between C and R due to floating-point evaluation order. This changes which intercept is solved explicitly, which changes the optimizer starting point, which may produce a different (but valid) local optimum. **Not a bug, but a different answer.**

### B4 — G1 case dispatch at boundary (CRASH or SILENT)
When `M = 0` or `ν = 0` exactly, the case dispatch must match C's exact boundary condition. The C code tests `m==0` after converting from estimation parameters — if your parameter transformation differs by even floating-point epsilon, you dispatch to the wrong case. Use `abs(m) < 1e-10` rather than `m == 0`.

### B5 — Stepwise oscillation guard (DIVERGENCE)
The C code tracks `mxmove` per variable to prevent a variable from entering and exiting indefinitely. If you omit this, `stepwise_select` may loop forever on some datasets. The `moves` counter in Pattern E is mandatory.

### B6 — `ctime = 0` vs `ctime` missing (SILENT)
In the C data format, `CTIME = 0` means "no interval censoring" (not a genuine time of 0). In R, represent this as `NA` and test `!is.na(ctime)` rather than `ctime > 0`. If you test `ctime > 0` with a genuine zero-time observation, you silently drop it.

### B7 — SAS XPORT variable name truncation (DATA CORRUPTION)
XPORT v5 truncates variable names to 8 characters. `haven::read_xpt()` silently truncates. If your dataset has variables like `incomplete_surgery_months` and `incomplete_surgery_age`, they both become `incomplet` — identical, corrupt. Always inspect `names()` after reading XPORT files.

### B8 — RHO is a derived parameter, not estimated (OPTIMIZER DIVERGENCE)
The C optimizer works in `(delta, thalf, nu, m)` space and computes `rho` from `thalf` after each step. If you accidentally include `rho` in your optimization vector, you have an over-parameterized model that will not converge.

---

## 4. Suggested Project Structure

```
R/
├── core/
│   ├── shaping_functions.R   # ln_g1_sg1(), ln_g3_sg3(), ln_base_transform()
│   ├── loglik.R              # obs_loglik(), total_loglik()
│   ├── conservation.R        # compute_phase_sumcf(), solve_conservation_factor()
│   ├── params.R              # make_param_spec(), pack_theta(), unpack_theta()
│   └── optimizer.R           # make_objective(), fit_model()
├── selection/
│   ├── stepwise.R            # stepwise_select(), decide_action()
│   ├── wald.R                # wald_test()
│   └── score.R               # score_test()
├── io/
│   ├── read_xport.R          # prepare_observations() from XPORT
│   └── format_output.R       # print/summary/predict S3 methods
├── hazard_fit.R              # User-facing API
tests/
├── test-shaping-functions.R  # testthat tests, one per G1 case
├── test-loglik.R
├── test-conservation.R
├── test-optimizer.R
└── test-stepwise.R
```

---

## 5. Validation Tests

### V1 — G1/SG1 Case-by-Case Numerical Validation

Run before integrating into the likelihood. The reference values come from the C executable.

```r
# tests/test-shaping-functions.R
library(testthat)

test_that("G1 Case 1 (M>0, nu>0) matches C reference", {
  # From AVC example: delta=0, thalf=0.151, nu=1.44, m=1
  rho <- set_rho_from_thalf(thalf = 0.151, nu = 1.44, m = 1)
  result <- ln_g1_sg1(t = 0.5, delta = 0, rho = rho, nu = 1.44, m = 1)
  expect_equal(result$ln_g1,  -0.12345, tolerance = 1e-5)   # replace with C output
  expect_equal(result$ln_sg1, -1.23456, tolerance = 1e-5)
})

test_that("G1 Case 2 (M=0, nu>0) matches C reference", { ... })
test_that("G1 Case 3 (M<0, nu>0) matches C reference", { ... })
test_that("G1 Case 4 (M<0, nu=0) matches C reference", { ... })
test_that("G1 Case 5 (M>0, nu<0) matches C reference", { ... })
test_that("G1 Case 6 (M=0, nu<0) matches C reference", { ... })

test_that("set_rho_from_thalf inverts correctly", {
  # RHO is correct when CF(thalf) = 0.5
  rho  <- set_rho_from_thalf(thalf = 0.151, nu = 1.44, m = 1)
  g1   <- ln_g1_sg1(t = 0.151, delta = 0, rho = rho, nu = 1.44, m = 1)
  expect_equal(exp(g1$ln_g1), 0.5, tolerance = 1e-8)
})
```

### V2 — Log-Likelihood at Known Parameters

Use the AVC dataset with the final C parameters and confirm R produces the same log-likelihood.

```r
test_that("total_loglik matches C reference at AVC final parameters", {
  data   <- haven::read_xpt("tests/AVC.xpt") |> prepare_observations(...)
  params <- list(
    early    = list(log_mu = log(0.236), delta = 0, thalf = 0.151, nu = 1.44, m = 1),
    constant = list(log_mu = log(5.4e-4)),
    late     = list(log_mu = -Inf)           # late phase not active
  )
  ll <- total_loglik(data, params)
  expect_equal(ll, -181.109, tolerance = 1e-3)
})
```

### V3 — Optimizer Convergence

```r
test_that("optimizer converges to AVC reference estimates", {
  data   <- haven::read_xpt("tests/AVC.xpt") |> prepare_observations(...)
  spec   <- make_param_spec(phases = c("early", "constant"), covariates = c("AGE", "STATUS"))
  result <- fit_model(data, spec)

  expect_true(result$converged)
  expect_equal(result$loglik, -181.109, tolerance = 0.01)
  # Key covariate estimates
  expect_equal(result$coef[["early.AGE"]],  -0.032, tolerance = 0.001)
  expect_equal(result$coef[["early.STATUS"]], 0.52, tolerance = 0.01)
})
```

### V4 — Conservation of Events

```r
test_that("events are conserved after fitting", {
  result <- hazard_fit(avc_data, time = follow_up, status = death,
                       early = ~ AGE + STATUS + COM_IV, constant = ~ STATUS)
  # Events conserved must equal total events (within floating point)
  expect_equal(result$conserved, result$n_events, tolerance = 1e-4)
})
```

### V5 — Stepwise Selection Path

```r
test_that("stepwise selection matches C reference path for AVC", {
  result  <- hazard_fit(avc_data, ..., selection = list(method = "stepwise"))
  history <- purrr::map_chr(result$selection_history, "variable")
  # Reference sequence from C .lst file:
  expect_equal(history, c("COM_IV", "MAL", "ORIFICE", "INC_SURG", "OPMOS"))
})
```

### V6 — Output Format Regression

```r
test_that("print.hazard_fit output matches reference .lst format", {
  result  <- hazard_fit(avc_data, ...)
  output  <- capture.output(print(result))
  ref     <- readLines("tests/reference/AVC.lst")

  # Key lines to match:
  expect_true(any(grepl("Log likelihood.*-181\\.1", output)))
  expect_true(any(grepl("Events conserved.*70\\.0", output)))
  expect_true(any(grepl("Convergence attained", output)))
})
```

### V7 — Numerical Stability Edge Cases

```r
test_that("ln_g1_sg1 handles delta near zero without cancellation", {
  # delta = 1e-12 should behave like delta = 0
  r1 <- ln_g1_sg1(t = 1, delta = 0,    rho = 1, nu = 1, m = 1)
  r2 <- ln_g1_sg1(t = 1, delta = 1e-12, rho = 1, nu = 1, m = 1)
  expect_equal(r1$ln_g1, r2$ln_g1, tolerance = 1e-8)
})

test_that("obs_loglik handles single event with no censoring", {
  ll <- obs_loglik(time = 1, c1 = 1, c2 = 0, c3 = 0, weight = 1,
                   ctime = NA, stime = 0,
                   cumhaz_fn = \(t) t,
                   hazard_fn = \(t) 1)
  expect_true(is.finite(ll))
})
```

---

## Quick Reference: Migration Risk Matrix

| Component | R Difficulty | Risk of Silent Error | Key Pitfall |
|---|---|---|---|
| G1/SG1 shaping (6 cases) | Hard | Very High | Wrong case dispatch at M=0, nu=0 boundaries |
| Log-likelihood loop | Medium | High | `log1p`/`expm1` — don't use `log(1+x)` |
| Conservation of events | Medium | High | FIXMU selection ties; B8 (rho in optimizer) |
| Objective + optim() | Easy | Low | Pack/unpack indexing off-by-one |
| Stepwise selection | Medium | Medium | Missing oscillation guard (B5) |
| XPORT I/O | Easy | Medium | 8-char name truncation (B7) |
| Output formatting | Easy | Low | Cosmetic only |

**Recommended start sequence:** V1 tests → Phase 1 (shaping) → V2 test → Phase 2 (loglik) → Phase 3 (conservation) → V3-V4 tests → Phase 4 (optimizer) → Phase 5 (mu/scaling) → Phase 6 (stepwise) → Phase 7 (I/O)

Write the tests before the implementation. The C executable is the oracle — use it to generate reference values at known parameters before you touch the C code.
