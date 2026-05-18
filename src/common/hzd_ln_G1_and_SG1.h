#ifndef HZD_LN_G1_AND_SG1_H
#define HZD_LN_G1_AND_SG1_H

/**
 * @file hzd_ln_G1_and_SG1.h
 * @brief Early-phase shaping function G1 and its time derivative SG1.
 *
 * Computes \f$\ln G_1(t)\f$ and \f$\ln SG_1(t)\f$ for the
 * early-phase component of the three-phase parametric hazard model
 * (Blackstone, Naftel, Turner 1986).
 *
 * **FORTRAN origin:** `model91/dlg1` (DLG1 subroutine).
 * @see hzd_ln_G1_and_SG1.c for implementation and flag-case details.
 * @see setg1() for parameter validation and g1flag assignment.
 */

/**
 * @brief Compute \f$\ln G_1(t)\f$ and \f$\ln SG_1(t)\f$ (early-phase shaping functions).
 *
 * Uses global structs: reads `Early` (delta, delta0, rho, nu, m, g1flag)
 * and `HazG1.T` (time); writes `HazG1.lnG1` and `HazG1.lnSG1`.
 *
 * The early-phase cumulative hazard shaping function \f$G_1(t)\f$ is
 * parameterised by four shape parameters \f$(\delta, \nu, M, \rho)\f$
 * with six special cases selected by `Early.g1flag`:
 *
 * | g1flag | Condition | Model name |
 * |--------|-----------|------------|
 * | 1 | \f$M>0,\,\nu>0\f$ | Negative Generic |
 * | 2 | \f$M=0,\,\nu>0\f$ | Limiting Case 1 (Gompertz-like) |
 * | 3 | \f$M<0,\,\nu>0\f$ | Positive Generic |
 * | 4 | \f$M<0,\,\nu=0\f$ | Limiting Case 2 |
 * | 5 | \f$M>0,\,\nu<0\f$ | Mixed Generic |
 * | 6 | \f$M=0,\,\nu<0\f$ | Limiting Case 3 |
 *
 * The time variable is first transformed via
 * \f$B(t) = (e^{\delta t}-1)/\delta\f$ (or \f$B(t)=t\f$ when \f$\delta=0\f$).
 * All flag-case computations are done in the log domain to avoid overflow.
 *
 * @pre `setg1()` must have been called to set `Early.g1flag` and validate parameters.
 * @pre `HazG1.T` must be set to the evaluation time \f$t > 0\f$.
 * @post `HazG1.lnG1` and `HazG1.lnSG1` are set.
 *
 * @note On domain error, calls hzd_Error() which longjmps — never returns.
 *       Uses ErrorPassBack() for errors in called subroutines.
 * @see dlgaob() hzd_ln_eBXmin1divB() hzd_ln_eX_plus1() hzd_ln_1min_e_negeX()
 *      hzd_ln_1min_enegX()
 */
void hzd_ln_G1_and_SG1(void);

#endif /* HZD_LN_G1_AND_SG1_H */
