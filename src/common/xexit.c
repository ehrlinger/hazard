#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "hzd_telemetry.h"

void xexit(int rc){
  if(rc!=0)
    printf("!%d\n",rc);
  /* v4.4.6: telemetry hook covers every exit path. */
  hzd_telemetry_end(rc);
  exit(rc);
}
