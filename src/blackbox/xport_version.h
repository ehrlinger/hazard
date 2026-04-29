/* src/blackbox/xport_version.h
 *
 * XPORT format version detection — added in v4.4.6.
 *
 * Pure function: classify the SAS XPORT "header #2" record (80 bytes)
 * as V5, V8, or unknown. No I/O, no global state — testable.
 */
#ifndef XPORT_VERSION_H
#define XPORT_VERSION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum xport_version {
    XPORT_VERSION_UNKNOWN = 0,
    XPORT_VERSION_V5      = 1,
    XPORT_VERSION_V8      = 2
};

/* Classify the second 80-byte header record from a SAS XPORT file.
 *
 *   buf    — pointer to header bytes (does not need to be null-terminated)
 *   len    — number of valid bytes in buf
 *
 * Returns XPORT_VERSION_V5 if buf contains "LIBRARY", V8 if "LIBV8",
 * UNKNOWN otherwise (NULL buf, len < 80, or no match).
 */
enum xport_version xport_detect_version(const char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* XPORT_VERSION_H */
