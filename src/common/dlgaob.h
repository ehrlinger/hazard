#ifndef  DLGAOB_H
#define  DLGAOB_H

/**
 * @file dlgaob.h
 * @brief Numerically stable computation of \f$\ln(A/B)\f$ via Mercator series.
 * @see FORTRAN origin: `math91/dlgaob` (DLGAOB) — E.H. Blackstone, September 1985.
 */

/**
 * @brief Compute \f$\ln(A/B)\f$ with high accuracy near \f$A/B = 1\f$.
 *
 * When \f$e^{-0.1} \leq A/B \leq e^{0.1}\f$ (ratio near 1, log near 0),
 * uses the Mercator series with \f$y = (A-B)/(A+B)\f$:
 * \f[
 *   \ln(A/B) = 2\left(\frac{y}{1} + \frac{y^3}{3} + \frac{y^5}{5} + \cdots\right)
 * \f]
 * to avoid precision loss. Otherwise returns \f$\ln(A/B)\f$ directly.
 *
 * @param A Numerator; must be > 0.
 * @param B Denominator; must be > 0.
 * @return \f$\ln(A/B)\f$, or 0 on domain error.
 *
 * @note Error codes: 1 = arithmetic overflow, 2 = A <= 0 or B <= 0 or A/B <= 0.
 * @see hzd_ln_A_div_B() for a simpler wrapper without series correction.
 */
double dlgaob(double A, double B);

#endif  /* DLGAOB_H */
