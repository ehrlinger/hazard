#ifndef HZD_LN_1MIN_ENEGX_H
#define HZD_LN_1MIN_ENEGX_H

/**
 * @file hzd_ln_1min_enegX.h
 * @brief Numerically stable computation of \f$\ln(1 - e^{-e^x})\f$.
 * @see FORTRAN origin: `math91/dl1mlx` (DL1MLX).
 */

/**
 * @brief Compute \f$\ln\!\left(1 - e^{-e^x}\right)\f$.
 *
 * Equivalent to evaluating \f$\ln(1 - e^{-u})\f$ (see hzd_ln_1min_e_negeX())
 * at \f$u = e^x\f$, but accepts the argument in log form to preserve
 * precision when \f$u\f$ is very small or very large.
 *
 * Four regions (in terms of \f$x\f$):
 * - \f$x > 5.159\f$: \f$e^x \geq 174\f$, \f$e^{-e^x} \approx 0\f$, return 0.
 * - \f$0.834 < x \leq 5.159\f$: \f$e^x > 2.30\f$; use series expansion.
 * - \f$-2.25 \leq x \leq 0.834\f$: stable, compute directly.
 * - \f$-35.351 < x < -2.25\f$: \f$e^{-e^x} \approx 1\f$; use Taylor series.
 *
 * @param X Log of the effective argument, i.e., \f$\ln(u)\f$ where the
 *           underlying expression is \f$\ln(1 - e^{-u})\f$.
 * @return \f$\ln(1 - e^{-e^x})\f$.
 */
double hzd_ln_1min_enegX(double X);

#endif /* HZD_LN_1MIN_ENEGX_H*/
