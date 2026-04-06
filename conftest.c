/* confdefs.h */
#define PACKAGE_NAME "Hazard_Package"
#define PACKAGE_TARNAME "hazard_package"
#define PACKAGE_VERSION "4.3.1"
#define PACKAGE_STRING "Hazard_Package 4.3.1"
#define PACKAGE_BUGREPORT ""
#define PACKAGE_URL ""
#define PACKAGE "hazard_package"
#define VERSION "4.3.1"
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_STRINGS_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define HAVE_WCHAR_H 1
#define STDC_HEADERS 1
#define _ALL_SOURCE 1
#define _DARWIN_C_SOURCE 1
#define _GNU_SOURCE 1
#define _HPUX_ALT_XOPEN_SOCKET_API 1
#define _NETBSD_SOURCE 1
#define _OPENBSD_SOURCE 1
#define _POSIX_PTHREAD_SEMANTICS 1
#define __STDC_WANT_IEC_60559_ATTRIBS_EXT__ 1
#define __STDC_WANT_IEC_60559_BFP_EXT__ 1
#define __STDC_WANT_IEC_60559_DFP_EXT__ 1
#define __STDC_WANT_IEC_60559_EXT__ 1
#define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
#define __STDC_WANT_IEC_60559_TYPES_EXT__ 1
#define __STDC_WANT_LIB_EXT2__ 1
#define __STDC_WANT_MATH_SPEC_FUNCS__ 1
#define _TANDEM_SOURCE 1
#define __EXTENSIONS__ 1
#define HAVE_ALLOCA_H 1
#define HAVE_ALLOCA 1
#define HAVE_FLOAT_H 1
#define HAVE_LIMITS_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_UNISTD_H 1
#define HAVE_MALLOC 1
#define HAVE_REALLOC 1
#define HAVE_FLOOR 1
#define HAVE_MEMSET 1
/* end confdefs.h.  */
/* Define pow to an innocuous variant, in case <limits.h> declares pow.
   For example, HP-UX 11i <limits.h> declares gettimeofday.  */
#define pow innocuous_pow

/* System header to define __stub macros and hopefully few prototypes,
   which can conflict with char pow (void); below.  */

#include <limits.h>
#undef pow

/* Override any GCC internal prototype to avoid an error.
   Use char because int might match the return type of a GCC
   builtin and then its argument prototype would still apply.  */
#ifdef __cplusplus
extern "C"
#endif
char pow (void);
/* The GNU C library defines this for functions which it implements
    to always fail with ENOSYS.  Some functions are actually named
    something starting with __ and the normal name is an alias.  */
#if defined __stub_pow || defined __stub___pow
choke me
#endif

int
main (void)
{
return pow ();
  ;
  return 0;
}
