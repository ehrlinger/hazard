#ifndef HZD_LN_1MIN_E_NEGEX_H
#define HZD_LN_1MIN_E_NEGEX_H

/**
 * @file hzd_ln_1min_e_negeX.h
 * @brief Numerically stable computation of \f$\ln(1 - e^{-x})\f$, \f$x > 0\f$.
 * @see FORTRAN origin: `math91/dl1mex` (DL1MEX).
 */

/**
 * @brief Compute \f$\ln(1 - e^{-x})\f$ with domain check and series expansion.
 *
 * Four regions:
 * - \f$x \geq 174\f$: \f$e^{-x} \approx 0\f$, return 0.
 * - \f$2.30 \leq x < 174\f$: use series \f$\sum_{k=1}^N (-z)^k/k\f$,
 *   \f$z = e^{-x}\f$.
 * - \f$0.1 \leq x < 2.30\f$: stable; compute directly.
 * - \f$0 < x < 0.1\f$: \f$e^{-x} \approx 1\f$, cancellation risk;
 *   use Taylor expansion of \f$e^{-x}\f$.
 *
 * @param lnX Argument; must be \f$> 0\f$.
 * @return \f$\ln(1 - e^{-x})\f$, or 0 on overflow.
 *
 * @note Error code 2 = \f$x\f$ not positive.
 */
double hzd_ln_1min_e_negeX(double lnX);

#endif /* HZD_LN_1MIN_E_NEGEX_H */
