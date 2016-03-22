#include <string.h>
#include "hzdinc.h"

void xvgetd(struct namestr *xvnsparm,void *xvarparm){
  xvgetptr[xvgetcnt].xvns = xvnsparm;
  xvgetptr[xvgetcnt].xvar = xvarparm;
  xvgetcnt++;
}
