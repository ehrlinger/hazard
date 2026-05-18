#ifndef HZD_EARLY_P2T_H
#define HZD_EARLY_P2T_H

/**
 * @file hzd_early_p2t.h
 * @brief Early-phase parameter-to-theta transform.
 * @see hzd_early_p2t.c
 * @see FORTRAN origin: `model91/dtrsfm` IOP=1 (E.H. Blackstone, November 1985).
 */

/**
 * @brief Map early-phase model parameters to unconstrained optimizer theta[].
 *
 * Converts the early-phase model parameters (DELTA, THALF, NU, M) stored
 * in the `Early` global struct to unconstrained computational parameters:
 *
 * | theta index | Source | Transform |
 * |---|---|---|
 * | 0 | `Early.delta` | \f$\ln(-\ln|\delta|)\f$ |
 * | 1 | `Early.tHalf` | \f$\ln(t_{1/2})\f$ |
 * | 2 | `Early.nu` | \f$\ln(|\nu|)\f$ |
 * | 3 | `Early.m` | \f$\ln(|M|)\f$ |
 *
 * Only indices where `Common.status[i] == 1` (actively estimated) are updated.
 *
 * @param sTheta  Pointer into the theta vector (theta[0..3] for early phase).
 *
 * @note Calls hzd_Error() on domain errors (e.g., THALF ≤ 0, |DELTA| ≥ 1).
 * @see hzd_early_t2p() for the inverse transform.
 * @see hzd_late_p2t() for the late-phase equivalent.
 */
void hzd_early_p2t(double *sTheta);

#endif /* HZD_EARLY_P2T_H */
