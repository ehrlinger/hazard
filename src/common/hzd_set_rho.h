#ifndef HZD_SET_RHO_H
#define HZD_SET_RHO_H

/**
 * @file hzd_set_rho.h
 * @brief Compute the \f$\rho\f$ scaling coefficient for the early-phase G1 model.
 * @see FORTRAN origin: `model91/dg1rho` (DG1RHO).
 */

/**
 * @brief Set the \f$\rho\f$ scaling term in the Early phase struct.
 *
 * \f$\rho\f$ normalises the B(t) function so that the early-phase
 * shaping function G1 equals 1 at \f$t = t_{1/2}\f$ (the half-hazard time):
 * \f[
 *   B(t) = \begin{cases}
 *     t & \text{if } \delta = 0 \\
 *     (e^{\delta t} - 1)/\delta & \text{otherwise}
 *   \end{cases}
 * \f]
 * Called once during early-phase parameter setup (setg1.c).
 *
 * @param Early Pointer to the early-phase global struct; `Early->rho` is set on return.
 */
void hzd_set_rho(struct early *Early);

#endif /* HZD_SET_RHO_H */
