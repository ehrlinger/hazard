#ifndef  HZD_LATE_P2T_H
#define  HZD_LATE_P2T_H

/**
 * @file hzd_late_p2t.h
 * @brief Late-phase parameter-to-theta transform.
 * @see hzd_late_p2t.c
 * @see FORTRAN origin: `model91/dtrsfm` IOP=1 (late-phase block).
 */

/**
 * @brief Map late-phase model parameters to unconstrained optimizer theta[].
 *
 * Converts (TAU, GAMMA, ALPHA, ETA) from the `Late` global struct to
 * unconstrained computational parameters:
 *
 * | theta index | Source | Transform | Notes |
 * |---|---|---|---|
 * | 4 | `Late.tau` | \f$\ln(\tau)\f$ | |
 * | 5 | `Late.gamma` | \f$\ln(\gamma)\f$ or \f$\ln(\gamma\eta - 2)\f$ | see Note 1 |
 * | 6 | `Late.alpha` | \f$\ln(\gamma\eta/\alpha - 2)\f$ or \f$\ln(\alpha)\f$ | see Note 2 |
 * | 7 | `Late.eta` | \f$\ln(\gamma\eta - 2)\f$ or \f$\ln(\eta)\f$ | see Note 3 |
 *
 * Notes (from FORTRAN DTRSFM):
 * - Note 1: Use \f$\ln(\gamma\eta-2)\f$ if \f$\gamma\eta \neq 2\f$ and g3flag<3 and ETA is fixed.
 * - Note 2: Use \f$\ln(\alpha)\f$ if \f$\gamma\eta/\alpha=2\f$ and \f$\gamma\eta\neq2\f$, or g3flag>2.
 * - Note 3: Use \f$\ln(\eta)\f$ if \f$\gamma\eta\f$ is fixed at 2 or g3flag>2.
 *
 * Only indices where `Common.status[i] == 1` are updated.
 *
 * @param sTheta  Pointer into the theta vector (theta[4..7] for late phase).
 * @see hzd_late_t2p() for the inverse.
 * @see hzd_early_p2t() for the early-phase equivalent.
 */
void hzd_late_p2t(double *sTheta);

#endif /* HZD_LATE_P2T_H */
