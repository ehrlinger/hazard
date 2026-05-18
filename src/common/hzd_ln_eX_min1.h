#ifndef  HZD_LN_EX_MIN1_H
#define  HZD_LN_EX_MIN1_H

/**
 * @file hzd_ln_eX_min1.h
 * @brief Numerically stable computation of \f$\ln(e^x - 1)\f$.
 *
 * @see hzd_ln_eX_min1.c for implementation details.
 * @see FORTRAN origin: `math91/dlexm1` (E.H. Blackstone, March 1986).
 */

/**
 * @brief Compute \f$\ln(e^x - 1)\f$ with domain check and cancellation avoidance.
 *
 * For \f$x \geq\f$ LnLim.maxOneSig (\f$\approx 74\f$), \f$e^x \gg 1\f$
 * so the result is returned as \f$x\f$ directly.
 * For \f$x \leq\f$ LnLim.absMin, underflow is handled by returning 0.
 * Near \f$x = 0\f$ where \f$e^x \approx 1\f$, a Taylor series
 * \f$e^x - 1 = \sum_{k=1}^{N} x^k / k!\f$ is used to avoid cancellation.
 *
 * @param X Argument; must be \f$> 0\f$.
 * @return \f$\ln(e^x - 1)\f$, or 0 on underflow.
 *
 * @note Overflow (\f$x \geq\f$ LnLim.max): silently returns \f$x\f$.
 *       Underflow (result rounds to zero): silently returns 0.
 *       No error code is set; this function never calls hzd_Error().
 * @see hzd_ln_eBXmin1divB() which calls this function.
 */
double hzd_ln_eX_min1(double X);

#endif  /* HZD_LN_EX_MIN1_H */
