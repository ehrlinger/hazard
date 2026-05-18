#ifndef HZD_LN_EBXMIN1DIVB_H
#define HZD_LN_EBXMIN1DIVB_H

/**
 * @file hzd_ln_eBXmin1divB.h
 * @brief Numerically stable computation of \f$\ln\!\left(\frac{e^{\beta x}-1}{\beta}\right)\f$.
 * @see FORTRAN origin: `math91/dlem1d` (DLEM1D).
 */

/**
 * @brief Compute \f$\ln\!\left(\frac{e^{\beta x} - 1}{\beta}\right)\f$,
 *        \f$\beta > 0,\ x > 0\f$.
 *
 * This expression arises in the early-phase transform as the log of
 * the integrated exponential growth function \f$B(t) = (e^{\delta t}-1)/\delta\f$.
 *
 * For \f$\beta x > 0.2\f$: computed as hzd_ln_eX_min1(\f$\beta x\f$) − ln(\f$|\beta|\f$).
 * For \f$\beta x \leq 0.2\f$: uses the Taylor series
 * \f$\frac{e^{\beta x}-1}{\beta} = x + \frac{\beta x^2}{2!} + \frac{\beta^2 x^3}{3!} + \cdots\f$
 * to avoid 0/0 cancellation as \f$\beta \to 0\f$ (the series → \f$x\f$).
 *
 * @param BETA Scale factor \f$\beta > 0\f$.
 * @param X    Variable \f$x > 0\f$.
 * @return \f$\ln\!\left(\frac{e^{\beta x}-1}{\beta}\right)\f$.
 *
 * @note Error codes: 1 = hzd_ln_eX_min1 range error, 2 = \f$\beta \leq 0\f$
 *       or \f$x \leq 0\f$.
 * @see hzd_ln_eX_min1()
 */
double hzd_ln_eBXmin1divB(double BETA,double X);

#endif /* HZD_LN_EBXMIN1DIVB_H */
