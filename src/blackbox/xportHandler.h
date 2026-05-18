#ifndef XPORT_HANDLER_H
#define XPORT_HANDLER_H

/**
 * @file xportHandler.h
 * @brief SAS XPORT transport file reader — public interface.
 *
 * Reads a SAS V5 XPORT-format file and loads observation data into
 * the `Common` global struct (`Common.obs`, `Common.Nobs`, etc.).
 * Rejects V8 XPORT format with a structured exit code.
 *
 * @see xportHandler.c for implementation.
 * @see xport_version.h for V5/V8 detection.
 */

/**
 * @brief Read a SAS XPORT file and populate global observation data.
 *
 * Opens `filename`, classifies the format (V5 vs V8 via xport_detect_version()),
 * reads the namestr records (variable definitions), then reads all observation
 * records into `Common.obs`. IBM-to-IEEE float conversion is applied via ibmieee.c.
 *
 * **Exit behavior on error:**
 * - V8 format detected → exits with HAZARD_EXIT_XPORT_V8_REJECTED.
 * - Invalid/corrupt header → exits with HAZARD_EXIT_XPORT_INVALID.
 *
 * @param filename  Path to the XPORT file (typically from `$TMPDIR/hzr.<ID>.dta`).
 * @param C         Pointer to the Common struct; `*C` is written and `Common = *C` set.
 *
 * @note TMPDIR protocol: the hazard binary constructs the path as
 *       `$TMPDIR/hzr.<JOBID>.<JOBIX>.dta` via opnfils.c.
 * @see xport_detect_version() xexit.h hzd_exit_codes.h
 */
void hzpxprt(char *filename, struct common *C);

#endif /* XPORT_HANDLER_H */
