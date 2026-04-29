#include <stdio.h>
#include <string.h>

#include "structures.h"
#include "hzd_log.h"
#include "hzd_error_marker.h"

/* Defined in hzd_Error.h — callers use the hzd_Error() macro which forwards
   __FILE__ / __LINE__ / __func__ into this implementation. */
void hzd_Error_at(const char *file, int line, const char *func,
                  const char *msgtext, int errnum){
  const char *safe_msg = msgtext ? msgtext : "(null)";

  /* Preserve the legacy "(MSG)" form in Common.errflg for consumers that
     still read it (e.g. hzrbomb, the top-level catch handler fallback). */
  snprintf(Common.errflg, sizeof(Common.errflg), "(%s)", safe_msg);

  Common.errfile = file;
  Common.errline = line;
  Common.errfunc = func;
  if(errnum)
    Common.errorno = errnum;

  /* Emit an ERROR-level log line. At the default threshold this surfaces
     every failure with full location context, regardless of whether the
     caller installed an errtrap. */
  HZD_LOG_ERROR("%s (errno=%d) raised at %s:%d in %s()",
                safe_msg, (int)Common.errorno,
                file ? file : "?", line, func ? func : "?");

  /* NEW v4.4.6: structured stderr marker so SAS macros / external
     wrappers can detect failures without parsing the .lst output.
     The msgtext IS the legacy error code (e.g. "DLG1935"); we pass
     it through directly. Future codes (XPORT_V8_REJECTED, etc.)
     follow the same convention. */
  hzd_emit_error(safe_msg, "internal error in numerical core");

  longjmp(Common.errtrap,(int)Common.errorno);
}
