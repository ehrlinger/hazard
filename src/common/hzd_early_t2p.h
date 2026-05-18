#ifndef HZD_EARLY_T2P_H
#define HZD_EARLY_T2P_H

/**
 * @file hzd_early_t2p.h
 * @brief Early-phase theta-to-parameter inverse transform.
 * @see hzd_early_t2p.c
 * @see FORTRAN origin: `model91/dtrsfm` IOP=2.
 */

/**
 * @brief Map unconstrained optimizer theta[] back to early-phase model parameters.
 *
 * Inverts hzd_early_p2t(): converts theta[0..3] from `Common.theta[]`
 * to early-phase model parameters written into the `TempE` (temporary early)
 * global struct:
 *
 * | theta index | Target | Inverse transform |
 * |---|---|---|
 * | 0 | `TempE.delta` | \f$-e^{-e^{\theta_0}}\f$ (with sign from `Early.deltaN`) |
 * | 1 | `TempE.tHalf` | \f$e^{\theta_1}\f$ |
 * | 2 | `TempE.nu` | \f$e^{\theta_2}\f$ (negated if g1flag ≥ 5) |
 * | 3 | `TempE.m` | \f$e^{\theta_3}\f$ (negated if g1flag ≥ 3) |
 *
 * Only indices where `Common.status[i] == 1` are updated.
 *
 * @note Calls hzd_Error() on overflow (fatal, error code 1).
 * @see hzd_early_p2t() for the forward transform.
 */
void hzd_early_t2p(void);

#endif /* HZD_EARLY_T2P_H */
