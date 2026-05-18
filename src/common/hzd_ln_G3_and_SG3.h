/**
 * @file hzd_ln_G3_and_SG3.h
 * @brief Late-phase shaping function G3 and its time derivative SG3.
 *
 * Computes \f$\ln G_3(t)\f$ and \f$\ln SG_3(t)\f$ for the late-phase
 * (chronically increasing) component of the three-phase hazard model.
 *
 * **FORTRAN origin:** `model91/dlg3` (DLG3 subroutine).
 * @see hzd_ln_G3_and_SG3.c
 * @see setg3() for parameter validation and g3flag assignment.
 */
#include "structures.h"

#ifndef HZD_LN_G3_AND_SG3_H
#define HZD_LN_G3_AND_SG3_H

/**
 * @brief Compute \f$\ln G_3(t)\f$ and \f$\ln SG_3(t)\f$ (late-phase shaping functions).
 *
 * Uses global structs: reads `Late` (tau, gamma, alpha, eta, g3flag)
 * and `HazG3.T`; writes `HazG3.lnG3` and `HazG3.lnSG3`.
 *
 * The late-phase shaping function \f$G_3(t)\f$ is a 4-parameter model
 * based on a special case of the hypergeometric series:
 * \f[
 *   G_3(t) = \left[\left(1 + \left(\frac{t}{\tau}\right)^\gamma\right)^{1/\alpha} - 1\right]^\eta
 * \f]
 *
 * Four cases selected by `Late.g3flag`:
 *
 * | g3flag | Condition | Notes |
 * |--------|-----------|-------|
 * | 1 | \f$\alpha \neq 0\f$, \f$\gamma\eta = 2\f$ | Generic, constrained |
 * | 2 | \f$\alpha = 0\f$, \f$\gamma\eta = 2\f$ | Exponential, constrained |
 * | 3 | \f$\alpha \neq 0\f$ | Generic, unconstrained |
 * | 4 | \f$\alpha = 0\f$ | Exponential, unconstrained |
 *
 * When \f$\alpha \to 0\f$ (g3flag 2 or 4), the model reduces to:
 * \f[
 *   G_3(t) = \left[e^{(t/\tau)^\gamma} - 1\right]^\eta
 * \f]
 *
 * @pre `setg3()` must have been called to set `Late.g3flag`.
 * @pre `HazG3.T` must be set to the evaluation time \f$t > 0\f$.
 * @post `HazG3.lnG3` and `HazG3.lnSG3` are set.
 *
 * @note Calls hzd_Error() via ErrorPassBack on domain errors.
 * @see hzd_ln_A_div_B() hzd_ln_eX_plus1() hzd_ln_eX_min1() hzd_ln_eeX_min1()
 */
void hzd_ln_G3_and_SG3(void);
#endif /* HZD_LN_G3_AND_SG3_H */
