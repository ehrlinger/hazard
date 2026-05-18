#ifndef HZD_LATE_T2P_H
#define HZD_LATE_T2P_H

/**
 * @file hzd_late_t2p.h
 * @brief Late-phase theta-to-parameter inverse transform.
 * @see hzd_late_t2p.c
 * @see FORTRAN origin: `model91/dtrsfm` IOP=2 (late-phase block).
 */

/**
 * @brief Map unconstrained optimizer theta[] back to late-phase model parameters.
 *
 * Inverts hzd_late_p2t(): recovers (TAU, GAMMA, ALPHA, ETA) from
 * theta[4..7] in `Common.theta[]`. Results written to `TempL` (temporary
 * late struct) for validation before committing to `Late`.
 *
 * The conditional transform depends on g3flag and the GETWO/GAETWO
 * constraint flags (matching the forward transform in hzd_late_p2t()).
 *
 * @note Calls hzd_Error() on overflow (fatal).
 * @see hzd_late_p2t() for the forward transform.
 * @see hzd_early_t2p() for the early-phase equivalent.
 */
void hzd_late_t2p(void);

#endif /* HZD_LATE_T2P_H */
