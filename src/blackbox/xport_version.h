/**
 * @file xport_version.h
 * @brief SAS XPORT format version detection (V5 vs V8).
 *
 * Pure function: classify the SAS XPORT "header #2" record (80 bytes)
 * as V5, V8, or unknown. No I/O, no global state — testable.
 * Added in v4.4.6.
 */
#ifndef XPORT_VERSION_H
#define XPORT_VERSION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SAS XPORT transport file format version.
 */
enum xport_version {
    XPORT_VERSION_UNKNOWN = 0,  /**< Header not recognized. */
    XPORT_VERSION_V5      = 1,  /**< SAS XPORT V5 ("LIBRARY " tag) — supported. */
    XPORT_VERSION_V8      = 2   /**< SAS XPORT V8 ("LIBV8   " tag) — rejected. */
};

/**
 * @brief Classify the second 80-byte header record of a SAS XPORT file.
 *
 * Checks the 8-byte format identifier at bytes 20–27:
 * - `"LIBRARY "` → V5 (classic SAS XPORT format)
 * - `"LIBV8   "` → V8 (SAS XPORT V8 — rejected by hazard, exits with
 *   HAZARD_EXIT_XPORT_V8_REJECTED)
 *
 * @param buf  Pointer to header bytes (need not be null-terminated).
 * @param len  Number of valid bytes in buf; must be ≥ 80.
 * @return     XPORT_VERSION_V5, XPORT_VERSION_V8, or XPORT_VERSION_UNKNOWN.
 */
enum xport_version xport_detect_version(const char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* XPORT_VERSION_H */
