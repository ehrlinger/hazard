#ifndef SWAB_COMPAT_H
#define SWAB_COMPAT_H

/* Pull in WORDS_BIGENDIAN when built via autotools (AC_C_BIGENDIAN). */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/*
 * Cross-platform swab(3) compatibility.
 * POSIX provides swab() in <unistd.h>; MinGW/UCRT exposes _swab() in
 * <stdlib.h>.  Consumers include this header and call swab() uniformly.
 */
#ifdef _WIN32
#  include <stdlib.h>
#  define swab(from, to, n) _swab((char *)(from), (char *)(to), (int)(n))
#else
#  ifndef _XOPEN_SOURCE
#    define _XOPEN_SOURCE 600
#  endif
#  include <unistd.h>
#endif

/*
 * SAS transport format is big-endian.  Byte-swap namestr fields on
 * little-endian hosts; on big-endian hosts the fields are already in
 * the correct byte order.
 *
 * AC_C_BIGENDIAN in configure.ac writes WORDS_BIGENDIAN into config.h
 * for big-endian builds.  When that symbol is absent the host is
 * little-endian and swapping is required.
 */
#ifndef WORDS_BIGENDIAN
#  define SAS_TRANSPORT_BYTESWAP
#endif

/*
 * Portable in-place byte-swap helpers.
 *
 * Use these instead of swab(src, src, n) which is undefined behaviour
 * when source and destination overlap (_swab on UCRT may silently
 * mishandle overlapping buffers).
 *
 * uint16_t/uint32_t are used internally to make the fixed-width
 * assumption explicit.  _Static_assert below verifies that short and
 * int match those widths on the current platform.
 */
#include <stdint.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(short) == 2,
    "short must be 16 bits for SAS namestr byte-swap helpers");
_Static_assert(sizeof(int) == 4,
    "int must be 32 bits for SAS namestr byte-swap helpers");
#endif

static inline short hzd_bswap_short(short x) {
    uint16_t u = (uint16_t)x;
    u = (uint16_t)((u << 8) | (u >> 8));
    return (short)u;
}

static inline int hzd_bswap_int(int x) {
    uint32_t u = (uint32_t)x;
    u = ((u & 0xFF000000U) >> 24) |
        ((u & 0x00FF0000U) >>  8) |
        ((u & 0x0000FF00U) <<  8) |
        ((u & 0x000000FFU) << 24);
    return (int)u;
}

#endif /* SWAB_COMPAT_H */
