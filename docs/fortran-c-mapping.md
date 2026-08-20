# FORTRAN → C Mapping: HAZARD Codebase

> **Purpose:** This document maps every FORTRAN subroutine/function in
> `/hazard_fortran/` to its C equivalent in `src/`.  It is the primary
> reference for writing Doxygen docblocks — wherever a C function's math
> or algorithm is opaque, the corresponding FORTRAN source has authoritative
> comments.
>
> **FORTRAN source root:** `<path-to>/hazard_fortran/`
> (local copy from CCF network drive; not in this repo)
>
> **C source root:** `src/`

---

## File Classification Summary

All extensionless files in `model91/`, `math91/`, `llike91/`, `optim91/`,
`vars91/`, `vcov91/`, `stat91/`, and `pred91/` are FORTRAN source (`file(1)`
reports "FORTRAN program text, ASCII text").  The `optim91/uncmin91/`
subdirectory is a third-party DOS-era archive; most of its `.for` files are
binary-encoded FORTRAN (reported as "data" by `file`).

| Directory | Contents | Role |
|-----------|----------|------|
| `math91/` | 10 FORTRAN functions | Numerically-stable log-domain primitives |
| `model91/` | 11 FORTRAN subroutines | Phase shaping, parameter transforms, G1/G3 setup |
| `llike91/` | 11 FORTRAN subroutines | Log-likelihood, conservation-of-events, objectives |
| `optim91/` | ~20 FORTRAN subroutines | BFGS optimizer, line search, Hessian estimation |
| `vars91/` | 11 FORTRAN subroutines | Stepwise variable selection |
| `vcov91/` | 7 FORTRAN subroutines | Variance–covariance, Cholesky decomposition |
| `stat91/` | 6 FORTRAN functions | Normal distribution, p-values |
| `pred91/` | 1 FORTRAN subroutine | Prediction / survivorship from estimates |
| `HZF/` | 1 FORTRAN file (monolithic) | Historical FORTRAN 77 HAZARD binary |
| `HZR/` | C + lex + yacc | Legacy C/lex/yacc parser layer |

---

## math91/ — Numerically-Stable Log-Domain Primitives

These functions compute composite expressions in the log domain to avoid
catastrophic cancellation near branch cuts.  They form the computational
bedrock of all shaping function and likelihood evaluations.

| FORTRAN file | Entry point | C file | C function | Expression |
|---|---|---|---|---|
| `math91/dlexm1` | `DLEXM1(X)` | `src/common/hzd_ln_eX_min1.c` | `hzd_ln_eX_min1(x)` | ln(eˣ − 1), x > 0 |
| `math91/dlexm1` (alt) | — | `src/common/hzd_ln_eX_min1.c` | — | same; variant revisions in `dlexm1` only |
| `math91/dlexp1` | `DLEXP1(X)` | `src/common/hzd_ln_eX_plus1.c` | `hzd_ln_eX_plus1(x)` | ln(eˣ + 1) |
| `math91/dlexp191` | `DLEXP1` variant | `src/common/hzd_ln_eX_plus1.c` | same | 1991 revision |
| `math91/dl1mex` | `DL1MEX(X)` | `src/common/hzd_ln_1min_e_negeX.c` | `hzd_ln_1min_e_negeX(x)` | ln(1 − e⁻ˣ), x > 0 |
| `math91/dl1mex91` | `DL1MEX` variant | `src/common/hzd_ln_1min_e_negeX.c` | same | 1991 revision |
| `math91/dl1mlx` | `DL1MLX(LNX)` | `src/common/hzd_ln_1min_enegX.c` | `hzd_ln_1min_enegX(lnx)` | ln(1 − e^(−e^LNX)) |
| `math91/dlem1d` | `DLEM1D(BETA,X)` | `src/common/hzd_ln_eBXmin1divB.c` | `hzd_ln_eBXmin1divB(beta,x)` | ln((e^(βx) − 1)/β), β > 0, x > 0 |
| `math91/dllxm1` | `DLLXM1(LNX)` | *(inlined in shaping fns)* | — | ln(e^(e^LNX) − 1); no standalone C file |
| `math91/dlgaob` | `DLGAOB(A,B)` | `src/common/dlgaob.c` | `dlgaob(a,b)` | ln(a/b); `hzd_ln_A_div_B.c` is the newer C-idiomatic wrapper |
| `math91/machin` | `MACHIN(MACHEP)` | `src/common/hzd_init_Common.c` | `hzd_set_Machn()` | Machine epsilon (unit of rounding error) |

**Notes:**
- `dlgaob.c` is a direct transliteration of the FORTRAN; `hzd_ln_A_div_B.c`
  is a thin C wrapper providing the same semantics under the project naming
  convention.
- `dllxm1` has no standalone C translation; its computation is inlined in the
  G1/G3 shaping function code paths that need ln(e^(e^ln(t)) − 1).

---

## model91/ — Phase Shaping Functions and Parameter Transforms

| FORTRAN file | Entry point | C file(s) | C function(s) | Role |
|---|---|---|---|---|
| `model91/dlg1` | `DLG1` | `src/common/hzd_ln_G1_and_SG1.c` | `hzd_ln_G1_and_SG1()` | ln(G1) and SG1 — early phase shaping fn and its time derivative |
| `model91/dlg3` | `DLG3` | `src/common/hzd_ln_G3_and_SG3.c` | `hzd_ln_G3_and_SG3()` | ln(G3) and SG3 — late phase shaping fn and its time derivative |
| `model91/dtrsfm` | `DTRSFM(THETA,STATUS,IOP)` | `src/common/hzd_early_p2t.c` | `hzd_early_p2t()` | Early phase: model params → estimation params (IOP=1) |
| `model91/dtrsfm` | (same) | `src/common/hzd_early_t2p.c` | `hzd_early_t2p()` | Early phase: estimation params → model params (IOP=2) |
| `model91/dtrsfm` | (same) | `src/common/hzd_late_p2t.c` | `hzd_late_p2t()` | Late phase: model params → estimation params (IOP=1) |
| `model91/dtrsfm` | (same) | `src/common/hzd_late_t2p.c` | `hzd_late_t2p()` | Late phase: estimation params → model params (IOP=2) |
| `model91/dtrsfm91` | `DTRSFM` 1991 variant | same C files | same | Older revision; the canonical docs are in `dtrsfm` |
| `model91/setg1` | `SETG1(...)` | `src/model/setg1.c` | `setg1()` | Validate early-phase parameters, set G1FLAG |
| `model91/setg191` | `SETG1` 1991 variant | `src/model/setg1.c` | same | Older revision |
| `model91/setg3` | `SETG3(...)` | `src/model/setg3.c` | `setg3()` | Validate late-phase parameters, set G3FLAG |
| `model91/setg394` | `SETG3` 1994 variant | `src/model/setg3.c` | same | 1994 revision |
| `model91/dtrsmu` | `DTRSMU` | `src/model/dtrsmu.c` | `dtrsmu()` | Early-phase μ coefficient computation |
| `model91/restor` | `RESTOR` | `src/model/restor.c` | `restor()` | Restore saved parameter vector after rejected step |
| `model91/dg1rho` | `DG1RHO` | `src/common/hzd_set_rho.c` | `hzd_set_rho()` | Compute ρ scaling coefficient for G1 |

**Key FORTRAN comment lode:**
- `dlg1` lines 1–200: full ASCII-art formula for G1 across all six special-case
  flags (M>0/NU>0, M>0/NU<0, M<0, exponential limits, etc.)
- `dlg3` lines 1–150: G3 formula and SG3 derivative in each flag case
- `dtrsfm` lines 1–120: parameter naming conventions (DELTA, THALF, NU, M,
  TAU, GAMMA, ALPHA, ETA) and transform equations

---

## llike91/ — Log-Likelihood, Conservation of Events, Objective Function

The `91`, `94`, `96` suffixes are successive revisions.  The C code merges
them into single files.

| FORTRAN file | Entry point | C file(s) | C function(s) | Role |
|---|---|---|---|---|
| `llike91/setlik` | `SETLIK(OBS,THETA,STATUS,...)` | `src/llike/setlik.c` | `setlik()` | Evaluate log-likelihood L(θ) and precompute derivative quantities |
| `llike91/setlik91` | variant | same | same | 1991 revision |
| `llike91/setlik94` | variant | same | same | 1994 revision |
| `llike91/setlik96` | variant | same | same | 1996 revision (canonical) |
| `llike91/setcoe` | `SETCOE(OBS,THETA,STATUS,...)` | `src/llike/setcoe.c` + `setcoe_calc_scaling.c` + `setcoe_obs_loop.c` + `setcoe_setup_indices.c` | `setcoe()` (+ helpers) | Conservation of events: determine scale, iterative update |
| `llike91/setcoe91` | variant | same | same | 1991 revision |
| `llike91/setcoe94` | variant | same | same | 1994 revision |
| `llike91/setcoe96` | variant | same | same | 1996 revision |
| `llike91/setobj` | `SETOBJ(OBS,INDEX,THETA,...)` | `src/llike/setobj.c` | `setobj()` | Objective function: FX = −LNLIKE (for optimizer) |
| `llike91/setobj91` | variant | same | same | 1991 revision |
| `llike91/setobj94` | variant | same | same | 1994 revision |

**Key FORTRAN comment lode:**
- `setlik` lines 1–100: formal definition of the log-likelihood sum, including
  handling of exact events (C1), right-censored (C2), and interval-censored (C3)
  observation types — the most important mathematical documentation in the repo.
- `setcoe` lines 30–120: Turner's conservation-of-events theorem (proof available
  upon request per the comment), Σ CF(tᵢ) = # events, and the iterative scheme
  for updating the scale parameter implicitly.

---

## optim91/ — BFGS Quasi-Newton Optimizer

Adapted from the UNCMIN library (Schnabel, Koontz, Weiss, 1985).  The `uncmin91/`
subdirectory contains the original UNCMIN distribution archive (DOS-era `.for`
files, many binary-encoded by `file`).

| FORTRAN file | Entry point | C file | C function | Role |
|---|---|---|---|---|
| `optim91/bfgsfa` | `BFGSFA` | `src/optim/bfgsfa.c` | `bfgsfa()` | BFGS quasi-Newton main driver |
| `optim91/bfgsfa91` | variant | same | same | 1991 revision |
| `optim91/bfgsfack` | variant | same | same | Checkpoint variant |
| `optim91/cndest` | `CNDEST` | `src/optim/cndest.c` | `cndest()` | Condition number estimator |
| `optim91/ddot91` | `DDOT` | `src/optim/dqdot.c` | `dqdot()` | BLAS-style dot product |
| `optim91/dqdot` | `DQDOT` | `src/optim/dqdot.c` | `dqdot()` | Quadratic dot product |
| `optim91/dnmr2` | `DNMR2` | `src/optim/dnmr2.c` | `dnmr2()` | Vector 2-norm |
| `optim91/fdgrad` | `FDGRAD` | `src/optim/fdgrad.c` | `fdgrad()` | Finite-difference gradient |
| `optim91/fdgrad96` | variant | same | same | 1996 revision |
| `optim91/fdhess` | `FDHESS` | `src/optim/fdhess.c` | `fdhess()` | Finite-difference Hessian |
| `optim91/fdhess96` | variant | same | same | 1996 revision |
| `optim91/ihesfa` | `IHESFA` | `src/optim/ihesfa.c` | `ihesfa()` | Initial Hessian approximation |
| `optim91/jacrot` | `JACROT` | `src/optim/jacrot.c` | `jacrot()` | Jacobi rotation for QR |
| `optim91/linesr` | `LINESR` | `src/optim/linesr.c` | `linesr()` | Line search (Armijo/Wolfe) |
| `optim91/linesr91` | variant | same | same | 1991 revision |
| `optim91/linesrck` | variant | same | same | Checkpoint variant |
| `optim91/qrupdt` | `QRUPDT` | `src/optim/qrupdt.c` | `qrupdt()` | Rank-1 QR update |
| `optim91/rquad` | `RQUAD` | `src/optim/rquad.c` | `rquad()` | Reverse quadratic interpolation |
| `optim91/setopt` | `SETOPT` | `src/optim/setopt.c` | `setopt()` | Optimizer parameter/tolerance setup |
| `optim91/setopt91` | variant | same | same | 1991 revision |
| `optim91/umstop` | `UMSTOP` | `src/optim/umstop.c` | `umstop()` | Optimizer stopping criteria |
| `optim91/umstp0` | `UMSTP0` | `src/optim/umstp0.c` | `umstp0()` | Extended stopping criteria |
| `optim91/uminck` | *(text file)* | `src/optim/uminck.c` | `uminck()` | UNCMIN configuration/sanity check |

---

## vars91/ — Stepwise Variable Selection

| FORTRAN file | Entry point | C file | C function | Role |
|---|---|---|---|---|
| `vars91/stepw` | `STEPW(STATUS,PVALUE,MOVES,...)` | `src/vars/stepw.c` | `stepw()` | Forward stepwise concomitant variable selection |
| `vars91/stepw91` | variant | same | same | 1991 revision |
| `vars91/backw` | `BACKW(STATUS,PVALUE,MOVES,...)` | `src/vars/backw.c` | `backw()` | Backward elimination |
| `vars91/backw91` | variant | same | same | 1991 revision |
| `vars91/dfast` | `DFAST(BETA,NVAR,COV,IVAR,...)` | `src/vars/dfast.c` | `dfast()` | Fast backward screening via coefficient update approximation |
| `vars91/dqstat` | `DQSTAT(CHOL,NVAR,INDX,...)` | `src/vars/dqstat.c` | `dqstat()` | Q-statistics for variables not in model (driver) |
| `vars91/dqstat91` | variant | same | same | 1991 revision |
| `vars91/q1` | `Q1` | `src/vars/q1.c` | `q1()` | Q-statistic inner-loop computation, subroutine 1 |
| `vars91/q2` | `Q2` | *(inlined in dqstat.c or q1.c)* | — | Q-statistic subroutine 2; no standalone C file found |
| `vars91/swvari91` | `SWVARI` | `src/vars/swvari.c` | `swvari()` | Stepwise variable indicator tracking |
| `vars91/swvarq91` | `SWVARQ` | `src/vars/swvarq.c` | `swvarq()` | Q-stat bookkeeping for stepwise |
| `vars91/swvarx91` | `SWVARX` | `src/vars/swvarx.c` | `swvarx()` | Extra bookkeeping for stepwise |

---

## vcov91/ — Variance-Covariance and Cholesky

| FORTRAN file | Entry point | C file | C function | Role |
|---|---|---|---|---|
| `vcov91/chlskm` | `CHLSKM(A,WK1,WK2,SX,MACHEP,N,IOP)` | `src/vcov/chlskm.c` | `chlskm()` | Modified Cholesky decomposition (Gill & Murray 1974) |
| `vcov91/choslv` | `CHOSLV(L,G,S,N)` | `src/vcov/choslv.c` | `choslv()` | Solve L·L'·S = −G (Cholesky back-substitution) |
| `vcov91/dcovar` | `DCOVAR(BETA,NVAR,...)` | `src/vcov/dcovar.c` | `dcovar()` | Compute variance-covariance, correlation, SE, p-values |
| `vcov91/setdll` | `SETDLL(OBS,INDEX,THETA,...)` | `src/vcov/setdll.c` | `setdll()` | First and second derivatives of log-likelihood (for concomitants) |
| `vcov91/setdll91` | variant | same | same | 1991 revision |
| `vcov91/setdll94` | variant | same | same | 1994 revision |
| `vcov91/setdll96` | variant | same | same | 1996 revision |

---

## stat91/ — Statistical Distribution Functions

| FORTRAN file | Entry point | C file | C function | Role |
|---|---|---|---|---|
| `stat91/normal` | `NORMAL(Z)` | `src/stat/normal.c` | `NORMAL(z)` | Single-tail area of standard normal (5-term polynomial approximation) |
| `stat91/normal91` | variant | same | same | 1991 revision |
| `stat91/norinv` | `NORINV(P)` | `src/hazpred/hzd_calc_norinv.c` | `hzd_calc_norinv()` | Inverse normal — function form |
| `stat91/snorin` | `SNORIN(P,Z)` | `src/hazpred/hzd_calc_norinv.c` | same | Inverse normal — subroutine form; merged in C |
| `stat91/pdlike` | `PDLIKE(LLIKE1,LLIKE2,DF)` | `src/stat/pdlike.c` | `pdlike()` | p-value for difference of two log-likelihoods (χ² test) |

---

## pred91/ — Prediction from Parameter Estimates

| FORTRAN file | Entry point | C file(s) | C function(s) | Role |
|---|---|---|---|---|
| `pred91/dpred` | `DPRED(OBS,THETA,STATUS,COV,...)` | `src/hazpred/hzp_calc_fn.c`, `hzp_calc_hazard.c`, `hzp_calc_survival.c`, `hzp_calc_haz_CL.c`, `hzp_calc_srv_CL.c`, `hzp_calc_scale.c`, `hzp_calc_intcp_drv.c`, `hzp_calc_intcp_subr.c`, `hzp_calc_parm_drv.c`, `hzp_calc_log_fn.c` | split across hazpred subsystem | Generate survivorship S(t), hazard H(t), and confidence limits from parameter estimates |

---

## HZF/ — Historical FORTRAN 77 Monolith

| File | Type | Notes |
|---|---|---|
| `HZF/hazf77.f` | FORTRAN 77 | Monolithic HAZARD binary.  The earliest surviving complete implementation; precedes the modular `model91/`/`llike91/` split. Useful for whole-program reading. |
| `HZF/params` | FORTRAN (COMMON block) | COMMON block and PARAMETER definitions referenced by `hazf77.f` |
| `HZF/lstsrc.c` | C | Source-listing utility; not part of core algorithm |
| `HZF/Makefile` | make | AIX-era build (IBM RS/6000) |

---

## HZR/ — C/Lex/Yacc Parser Layer

This is the original C front end that parses the PROC HAZARD statement syntax
and calls the FORTRAN computation layer (later the C-translated equivalents).

| File | Type | Notes |
|---|---|---|
| `HZR/hazard.c` | C | Entry point / HAZARD main dispatcher |
| `HZR/hazard.l` | Lex | Lexer for PROC HAZARD statement syntax |
| `HZR/hazard.y` | Yacc | Grammar for PROC HAZARD |
| `HZR/lex.yy.c` | C (generated) | `lex` output |
| `HZR/y.tab.c` | C (generated) | `yacc` output |
| `HZR/y.tab.h` | C (generated) | `yacc` symbol table header |

The modern C repo (`src/hazard/hazard_l.l`, `src/hazard/hazard_y.y`) is a
direct descendant of this layer.

---

## Non-FORTRAN / Infrastructure Files

| File | Classification | Notes |
|---|---|---|
| `build` | Shell script | Top-level build script (AIX-era) |
| `optim91/uminck` | Text | UNCMIN check/configuration parameters |
| `optim91/uminck91` | Text | 1991 revision |
| `optim91/umstop` | Text | Optimizer stopping criteria narrative |
| `optim91/uncminck` | Text | UNCMIN library configuration |
| `optim91/uncmin91/` | Directory | Third-party UNCMIN library archive (DOS-era, `.for` files; most are binary-encoded Lahey FORTRAN) |
| `optim91/uncmin91/asof11-1.txt` | Text | UNCMIN release notes |
| `optim91/uncmin91/linesr.f77` | FORTRAN | Pure F77 line search (CRLF — from DOS distribution) |
| `optim91/uncmin91/linesr.for` | FORTRAN | Same with Lahey extensions |
| `optim91/uncmin91/umstp0.for` | FORTRAN | Stopping criteria |
| `optim91/uncmin91/extadd.for`, `extmul.for`, `lngmul.for` | FORTRAN | Extended arithmetic helpers |
| `optim91/uncmin91/*.lib` | Binary | Microsoft Visual C libraries (historical) |
| `vars91/dqstat.diff` | Diff | Patch record between `dqstat` revisions |
| `.DS_Store` | macOS metadata | Ignore |

---

## Quick-Reference: FORTRAN Entry Point → C Function

| FORTRAN entry point | C function | C file |
|---|---|---|
| `DLEXM1(X)` | `hzd_ln_eX_min1(x)` | `src/common/hzd_ln_eX_min1.c` |
| `DLEXP1(X)` | `hzd_ln_eX_plus1(x)` | `src/common/hzd_ln_eX_plus1.c` |
| `DL1MEX(X)` | `hzd_ln_1min_e_negeX(x)` | `src/common/hzd_ln_1min_e_negeX.c` |
| `DL1MLX(LNX)` | `hzd_ln_1min_enegX(lnx)` | `src/common/hzd_ln_1min_enegX.c` |
| `DLEM1D(BETA,X)` | `hzd_ln_eBXmin1divB(beta,x)` | `src/common/hzd_ln_eBXmin1divB.c` |
| `DLGAOB(A,B)` | `dlgaob(a,b)` / `hzd_ln_A_div_B(a,b)` | `src/common/dlgaob.c` / `hzd_ln_A_div_B.c` |
| `MACHIN(MACHEP)` | `hzd_set_Machn()` | `src/common/hzd_init_Common.c` |
| `DLG1` | `hzd_ln_G1_and_SG1()` | `src/common/hzd_ln_G1_and_SG1.c` |
| `DLG3` | `hzd_ln_G3_and_SG3()` | `src/common/hzd_ln_G3_and_SG3.c` |
| `DTRSFM(THETA,STATUS,1)` | `hzd_early_p2t()` | `src/common/hzd_early_p2t.c` |
| `DTRSFM(THETA,STATUS,2)` | `hzd_early_t2p()` | `src/common/hzd_early_t2p.c` |
| `DTRSFM` (late,1) | `hzd_late_p2t()` | `src/common/hzd_late_p2t.c` |
| `DTRSFM` (late,2) | `hzd_late_t2p()` | `src/common/hzd_late_t2p.c` |
| `SETG1(...)` | `setg1()` | `src/model/setg1.c` |
| `SETG3(...)` | `setg3()` | `src/model/setg3.c` |
| `DTRSMU` | `dtrsmu()` | `src/model/dtrsmu.c` |
| `RESTOR` | `restor()` | `src/model/restor.c` |
| `DG1RHO` | `hzd_set_rho()` | `src/common/hzd_set_rho.c` |
| `SETLIK(OBS,THETA,STATUS,...)` | `setlik()` | `src/llike/setlik.c` |
| `SETCOE(OBS,THETA,STATUS,...)` | `setcoe()` | `src/llike/setcoe.c` |
| `SETOBJ(OBS,INDEX,THETA,...)` | `setobj()` | `src/llike/setobj.c` |
| `BFGSFA` | `bfgsfa()` | `src/optim/bfgsfa.c` |
| `LINESR` | `linesr()` | `src/optim/linesr.c` |
| `FDHESS` | `fdhess()` | `src/optim/fdhess.c` |
| `FDGRAD` | `fdgrad()` | `src/optim/fdgrad.c` |
| `CHLSKM(A,...,IOP)` | `chlskm()` | `src/vcov/chlskm.c` |
| `CHOSLV(L,G,S,N)` | `choslv()` | `src/vcov/choslv.c` |
| `DCOVAR(BETA,NVAR,...)` | `dcovar()` | `src/vcov/dcovar.c` |
| `SETDLL(OBS,INDEX,THETA,...)` | `setdll()` | `src/vcov/setdll.c` |
| `STEPW(STATUS,PVALUE,...)` | `stepw()` | `src/vars/stepw.c` |
| `BACKW(STATUS,PVALUE,...)` | `backw()` | `src/vars/backw.c` |
| `DFAST(BETA,NVAR,COV,IVAR,...)` | `dfast()` | `src/vars/dfast.c` |
| `DQSTAT(CHOL,NVAR,...)` | `dqstat()` | `src/vars/dqstat.c` |
| `NORMAL(Z)` | `NORMAL(z)` | `src/stat/normal.c` |
| `NORINV(P)` / `SNORIN(P,Z)` | `hzd_calc_norinv()` | `src/hazpred/hzd_calc_norinv.c` |
| `PDLIKE(LLIKE1,LLIKE2,DF)` | `pdlike()` | `src/stat/pdlike.c` |
| `DPRED(OBS,THETA,STATUS,COV,...)` | *(split across hazpred subsystem)* | `src/hazpred/hzp_calc_*.c` |

---

## Revision Suffix Key

Many FORTRAN files appear with numeric suffixes (`91`, `94`, `96`) representing
year-of-revision.  The baseline (no suffix) is typically the earliest version;
higher years are later revisions that were merged into the C translation.

| Suffix | Meaning |
|--------|---------|
| *(none)* | Baseline / earliest captured revision |
| `91` | 1991 revision |
| `94` | 1994 revision |
| `96` | 1996 revision (usually most recent) |
| `.mar` | VAX/VMS MACRO-format variant of the same source |
