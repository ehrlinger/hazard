#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "hzdinc.h"
#include "xexit.h"
#include "hzd_log.h"
#include "hzd_error_marker.h"
#include "hzd_exit_codes.h"

/* Legacy entry point — preserved bit-exact for v4.4.5 compatibility.
   All existing callers continue to exit with code 16. */
void hzfxit_at(const char *file, int line, const char *func, const char *type){
  const char *safe_type = type ? type : "(unspecified)";

  if(curr_xlmode)
    printf("$Note: EXITING DUE TO %s ERROR (at %s:%d %s)\n",
           safe_type,
           file ? file : "?", line,
           func ? func : "?");

  HZD_LOG_ERROR("fatal exit: %s at %s:%d in %s()",
                safe_type,
                file ? file : "?", line,
                func ? func : "?");

  /* Emit with a stable code; safe_type goes in the message since legacy
     callers pass arbitrary strings that may not satisfy code_is_valid(). */
  hzd_emit_error("LEGACY_HZFXIT", safe_type);

  xexit(HAZARD_EXIT_XPORT_V8_REJECTED);  /* = 16, preserved value */
}

/* v4.4.6+ entry point with explicit exit code. */
void hzfxit_with_code_at(const char *file, int line, const char *func,
                         const char *code, const char *msg, int exit_code) {
  const char *safe_code = code ? code : "INTERNAL_ERROR";
  const char *safe_msg  = msg  ? msg  : "(no message)";

  if(curr_xlmode)
    printf("$Note: EXITING DUE TO %s ERROR (at %s:%d %s)\n",
           safe_code,
           file ? file : "?", line,
           func ? func : "?");

  HZD_LOG_ERROR("fatal exit: %s (%s) at %s:%d in %s()",
                safe_code, safe_msg,
                file ? file : "?", line, func ? func : "?");

  hzd_emit_error(safe_code, safe_msg);

  xexit(exit_code);
}
