#ifndef  HZD_LN_A_DIV_B_H
#define  HZD_LN_A_DIV_B_H

/**
 * @file hzd_ln_A_div_B.h
 * @brief Compute \f$\ln(A/B)\f$ with domain error gating.
 * @see FORTRAN origin: `math91/dlgaob` (DLGAOB) — simplified wrapper variant.
 */

/**
 * @brief Compute \f$\ln(A/B)\f$ with domain error gating.
 *
 * A simplified wrapper that checks A > 0, B > 0, A/B > 0, and that the
 * ratio is within representable range, then returns ln(A/B) directly.
 * For A/B near 1.0, use dlgaob() which applies a Mercator series for
 * higher accuracy.
 *
 * @param A Numerator; must be > 0.
 * @param B Denominator; must be > 0.
 * @return \f$\ln(A/B)\f$, or 0 on domain error.
 *
 * @note Error codes: 1 = overflow, 2 = A <= 0 or B <= 0 or A/B <= 0.
 * @see dlgaob() for the series-accurate variant.
 */
double hzd_ln_A_div_B(double A,double B);

#endif  /* HZD_LN_A_DIV_B_H */
