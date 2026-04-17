#ifndef SWAB_COMPAT_H
#define SWAB_COMPAT_H

/*
 * Cross-platform swab(3) compatibility.
 * POSIX provides swab() in <unistd.h>, while MinGW exposes _swab()
 * in <stdlib.h>. Consumers include this header and call swab().
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
 * Portable in-place byte-swap helpers.
 * Use these instead of swab(src, src, n) which has undefined behaviour
 * when source and destination overlap (which _swab on UCRT may silently
 * mishandle).
 */
static inline short hzd_bswap_short(short x) {
    unsigned short u = (unsigned short)x;
    return (short)((u << 8) | (u >> 8));
}

static inline int hzd_bswap_int(int x) {
    unsigned int u = (unsigned int)x;
    return (int)(((u & 0xFF000000U) >> 24) |
                 ((u & 0x00FF0000U) >>  8) |
                 ((u & 0x0000FF00U) <<  8) |
                 ((u & 0x000000FFU) << 24));
}

#endif /* SWAB_COMPAT_H */
