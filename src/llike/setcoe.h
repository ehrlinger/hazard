/**
 * @file setcoe.h
 * @brief Conservation-of-events coefficient setup.
 *
 * Turner's conservation-of-events theorem (M.E. Turner Jr., 1985):
 * at maximum likelihood, \f$\sum_i CF(t_i) = \text{(number of events)}\f$.
 * This allows one scaling parameter to be solved explicitly.
 *
 * @see setcoe.c
 * @see setlik.c (called after setcoe to evaluate the likelihood)
 */
#ifndef SETCOE_H
#define SETCOE_H

#ifdef SETCOE_DEFS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct {
  xtended qsumcz,qsumcj;
  double sc1pc3,c3w;
  double sumcf[4];
  double cumhct;
} Setcoe;

/**
 * @brief Set up conservation-of-events coefficients and solve explicit scale.
 *
 * 1. Identifies which scaling parameter (μ₁, μ₂, or μ₃) can be solved
 *    non-iteratively from the conservation constraint \f$\sum CF(t_i) = \sum E(i)\f$.
 * 2. Precomputes shaping functions for phases with all parameters fixed.
 * 3. Iteratively updates the explicit scale until convergence.
 *
 * **FORTRAN origin:** `llike91/setcoe` (SETCOE subroutine);
 * split into setcoe_setup_indices.c, setcoe_obs_loop.c, setcoe_calc_scaling.c.
 */
void setcoe(void);

#undef EXTERN
#endif /* SETCOE_H */
