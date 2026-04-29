/* src/blackbox/xport_version.c
 *
 * Pure-C banner classifier. No allocations, no globals.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "xport_version.h"

enum xport_version xport_detect_version(const char *buf, size_t len)
{
    if (!buf || len < 80) return XPORT_VERSION_UNKNOWN;

    /* Per the SAS XPORT V5 spec, header #2 is:
       "HEADER RECORD*******LIBRARY HEADER RECORD!!!!!!!..."
       V8 replaces "LIBRARY " with "LIBV8   " (8 chars, padded). */

    /* Guard: must start with the standard "HEADER RECORD*******" prefix. */
    if (memcmp(buf, "HEADER RECORD*******", 20) != 0) {
        return XPORT_VERSION_UNKNOWN;
    }

    /* Classifier: bytes 20..27 are the format identifier (8 chars). */
    if (memcmp(buf + 20, "LIBRARY ", 8) == 0) return XPORT_VERSION_V5;
    if (memcmp(buf + 20, "LIBV8   ", 8) == 0) return XPORT_VERSION_V8;

    return XPORT_VERSION_UNKNOWN;
}
