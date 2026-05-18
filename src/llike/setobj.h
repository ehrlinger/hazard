/**
 * @file setobj.h
 * @brief Objective function for the BFGS optimizer.
 *
 * The optimizer minimises FX = −LNLIKE (negative log-likelihood).
 * Multiple entry points for different call contexts (full optimization
 * vs. conservation-of-events update).
 *
 * **FORTRAN origin:** `llike91/setobj` (SETOBJ subroutine).
 * @see setobj.c
 */
#ifndef SETOBJ_H
#define SETOBJ_H
#include <structures.h> /* Defines the logical data type */

#ifdef SETOBJ_DEFS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct {
  xtended lnlike;
  double lfactr;
  logical facall;
} Setobj;

/**
 * @brief Optimizer entry point: evaluate −LNLIKE at beta.
 *
 * Called by the BFGS optimizer (bfgsfa.c) at each function evaluation.
 * Transforms beta to model parameters, calls setcoe() then setlik(),
 * and returns FX = −LNLIKE.
 *
 * @param beta  Current parameter vector (optimizer theta[]).
 * @param fx    Output: objective function value (−LNLIKE).
 * @param nvar  Number of active parameters.
 */
void DHAZRD(double *beta,double *fx,int nvar);

/**
 * @brief Conservation-of-events objective entry point.
 *
 * Evaluates the objective for the conservation-of-events update step
 * (used between optimizer iterations to update the explicit scale).
 *
 * @param fx  Output: objective function value.
 */
void DCEOBJ(double *fx);

/**
 * @brief Main logic for DCEOBJ (no theta transform).
 * @param fx  Output: objective function value.
 */
void DCEOBJ_main_logic(double *fx);

/**
 * @brief Per-observation loop for DCEOBJ.
 * @return TRUE on success, FALSE on domain error.
 */
logical DCEOBJ_obs_loop(void);

#undef EXTERN
#endif /* SETOBJ_H */
