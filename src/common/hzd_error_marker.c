/* src/common/hzd_error_marker.c
 *
 * Implementation of hzd_emit_error() — see hzd_error_marker.h for contract.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "hzd_error_marker.h"

/* Validate that `code` is non-NULL, non-empty, ASCII [A-Z0-9_], length <= 64.
   Returns 1 if valid, 0 otherwise. */
static int code_is_valid(const char *code)
{
    if (!code) return 0;
    size_t n = 0;
    for (const char *p = code; *p; ++p, ++n) {
        if (n >= 64) return 0;
        unsigned char c = (unsigned char)*p;
        int ok = (c >= 'A' && c <= 'Z')
              || (c >= '0' && c <= '9')
              || c == '_';
        if (!ok) return 0;
    }
    return n > 0;
}

void hzd_emit_error(const char *code, const char *msg)
{
    if (!code_is_valid(code)) {
        /* Soft-reject: emit a single diagnostic to stderr, don't crash.
           This catches programmer errors (bad code) without breaking
           the binary's exit path. */
        fprintf(stderr,
                "[hazard internal] hzd_emit_error: invalid code (rejected)\n");
        fflush(stderr);
        return;
    }
    const char *safe_msg = msg ? msg : "(no message)";
    fprintf(stderr, "HAZARD_ERROR_%s %s\n", code, safe_msg);
    fflush(stderr);
}
