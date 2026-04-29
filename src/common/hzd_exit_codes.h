/* src/common/hzd_exit_codes.h
 *
 * Canonical exit codes for the hazard binary. The convention follows
 * the existing hzfxit() history: 0 = success, 1 = generic legacy
 * failure (preserved for backward compat), 12-20 = structured errors
 * introduced in v4.4.6+.
 *
 * Adding a code requires:
 *   1. New HAZARD_EXIT_* constant here
 *   2. Corresponding HAZARD_ERROR_* code string (see hzd_error_marker.h)
 *   3. Entry in docs/BINARY-SAS-PROTOCOL.md (when that file exists in v4.4.7)
 *   4. Entry in hzd_error_describe() table
 */
#ifndef HZD_EXIT_CODES_H
#define HZD_EXIT_CODES_H

#define HAZARD_EXIT_SUCCESS              0
#define HAZARD_EXIT_LEGACY_FAILURE       1   /* preserved — pre-v4.4.6 generic */
#define HAZARD_EXIT_OPTIMIZER_DIVERGED  12   /* existing hzfxit("ERROR") class */
#define HAZARD_EXIT_MEMORY_EXHAUSTED    14   /* existing */
#define HAZARD_EXIT_XPORT_V8_REJECTED   16   /* NEW v4.4.6 */
#define HAZARD_EXIT_XPORT_INVALID       17   /* NEW v4.4.6 */
/* Note: VAR_NOT_FOUND and VAR_TYPE_MISMATCH intentionally share exit code 18.
   They represent the same failure class (variable resolution failure) and are
   distinguished by their HAZARD_ERROR_<CODE> stderr marker, not the exit code. */
#define HAZARD_EXIT_VAR_NOT_FOUND       18   /* reserved for v5.0-alpha */
#define HAZARD_EXIT_VAR_TYPE_MISMATCH   18   /* alias of VAR_NOT_FOUND — same exit class */
#define HAZARD_EXIT_LICENSE_INVALID     19   /* reserved */
#define HAZARD_EXIT_INTERNAL_ERROR      20   /* catch-all */

#endif /* HZD_EXIT_CODES_H */
