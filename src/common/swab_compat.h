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

#endif /* SWAB_COMPAT_H */
