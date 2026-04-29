/* src/common/hzd_telemetry.h
 *
 * Append-only JSONL invocation telemetry — v4.4.6+.
 * See docs/superpowers/specs/2026-04-28-hazard-v5-design.md §7.7
 * and Amendment 2026-04-29 §A.3.
 *
 * Lifecycle:
 *   1. main() calls hzd_telemetry_begin() ONCE at startup
 *   2. main() captures input format / size info as it learns them
 *      (via hzd_telemetry_set_*)
 *   3. main() calls hzd_telemetry_end() before exit (any path)
 *
 * Always-safe: failures to write the telemetry log are non-fatal.
 * The binary's exit code reflects the analysis result, not telemetry I/O.
 *
 * Opt-out:
 *   - HAZARD_NO_TELEMETRY=1 env var disables all writes
 *   - Constructor argument no_telemetry=1 (set by --no-telemetry CLI)
 *
 * PHI invariants (NEVER logged):
 *   - Variable names, values, labels, formats, informats
 *   - Control file content
 *   - Any contents of input data
 *
 * Schema version: 1 (see spec §7.7 Amendment A.3.1).
 */
#ifndef HZD_TELEMETRY_H
#define HZD_TELEMETRY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Input format enum — values match strings emitted in JSON. */
enum hzd_telemetry_input_format {
    HZD_TELEMETRY_INPUT_NONE     = 0,
    HZD_TELEMETRY_INPUT_XPORT_V5 = 1,
    HZD_TELEMETRY_INPUT_XPORT_V8 = 2,
    HZD_TELEMETRY_INPUT_PARQUET  = 3,  /* reserved for v5.0-alpha */
    HZD_TELEMETRY_INPUT_UNKNOWN  = 4
};

/* Output format enum. */
enum hzd_telemetry_output_format {
    HZD_TELEMETRY_OUTPUT_NONE     = 0,
    HZD_TELEMETRY_OUTPUT_XPORT_V5 = 1,
    HZD_TELEMETRY_OUTPUT_PARQUET  = 2   /* reserved for v5.0-beta */
};

/* Initialize the telemetry subsystem. Call ONCE at main() entry.
 * If no_telemetry is non-zero (--no-telemetry CLI), or HAZARD_NO_TELEMETRY=1
 * is set in the environment, telemetry is disabled (subsequent set_* and
 * end() are no-ops).
 *
 * Captures: ts (now), host, user, hazard_version, parent_job_id (from
 * HAZARD_BOOTSTRAP_JOB_ID env if set, NULL otherwise).
 *
 * Resolves the log path via fallback chain (see docs §7.7 / A.3.3).
 * Disables silently with one HZD_LOG_WARN if no path is writable.
 */
void hzd_telemetry_begin(int no_telemetry);

/* Setters — call as info becomes available during the run. */
void hzd_telemetry_set_input_format(enum hzd_telemetry_input_format f);
void hzd_telemetry_set_input_size_bytes(long long n);
void hzd_telemetry_set_output_format(enum hzd_telemetry_output_format f);
void hzd_telemetry_set_n_obs(long long n);
void hzd_telemetry_set_n_vars(long long n);

/* Finalize: compute duration, set exit_code, write the JSON event,
 * rotate if needed. Call ONCE before exit. After this returns,
 * subsequent set_* / end() calls are no-ops.
 */
void hzd_telemetry_end(int exit_code);

/* Test-only reset — resets all process-wide state. Not for production use. */
void hzd_telemetry_test_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* HZD_TELEMETRY_H */
