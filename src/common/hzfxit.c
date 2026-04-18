#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "xexit.h"
#include "hzd_log.h"

/* Defined in hzfxit.h — callers use the hzfxit() macro which forwards
   __FILE__ / __LINE__ / __func__ into this implementation. */
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

  xexit(16);
}
