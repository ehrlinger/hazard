#ifndef HZD_LN_EX_PLUS1_H
#define HZD_LN_EX_PLUS1_H

/**
 * @file hzd_ln_eX_plus1.h
 * @brief Numerically stable computation of \f$\ln(e^x + 1)\f$.
 * @see FORTRAN origin: `math91/dlexp1`.
 */

/**
 * @brief Compute \f$\ln(e^x + 1)\f$ with series expansion for small \f$x\f$.
 *
 * For \f$x \leq -174\f$, \f$e^x \approx 0\f$ and the result is 0.
 * For \f$-174 < x < 5\f$, uses the alternating series
 * \f$\ln(1+z) = z - z^2/2 + z^3/3 - \cdots\f$ with \f$z = e^x\f$.
 * For \f$x \geq 5\f$, \f$e^x \gg 1\f$ and the result is returned as \f$x\f$.
 *
 * @param X Argument (any real value).
 * @return \f$\ln(e^x + 1)\f$.
 */
double hzd_ln_eX_plus1(double X);

#endif /* HZD_LN_EX_PLUS1_H*/
