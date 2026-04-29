/* src/common/hzd_error_marker.h
 *
 * Structured stderr error markers introduced in v4.4.6.
 *
 * Format: "HAZARD_ERROR_<CODE> <human-readable message>\n"
 *
 * Always written to stderr. NOT controlled by HZD_LOG_LEVEL — these
 * are protocol-level markers consumed by SAS macros (v4.4.7+) and
 * external wrappers, not human-readable diagnostics. Suppressing them
 * would break the binary↔caller contract.
 *
 * Adding a code: see hzd_exit_codes.h.
 */
#ifndef HZD_ERROR_MARKER_H
#define HZD_ERROR_MARKER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Emit a structured error marker to stderr.
 *
 *   code:  short uppercase identifier without "HAZARD_ERROR_" prefix,
 *          e.g. "XPORT_V8_REJECTED". Must be non-NULL, non-empty,
 *          ASCII [A-Z0-9_]. Length <= 64.
 *   msg:   human-readable message, may be NULL ("(no message)").
 *
 * Output is unbuffered (stderr is fflush'd before return).
 *
 * If the caller is going to exit with a non-zero code, it should call
 * this function before exit() / xexit() so the marker reaches stderr
 * before the process terminates.
 */
void hzd_emit_error(const char *code, const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* HZD_ERROR_MARKER_H */
