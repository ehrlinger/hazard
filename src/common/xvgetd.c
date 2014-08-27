#include "hzdinc.h"

void xvgetd(struct namestr *xvnsparm,void *xvarparm){
#ifdef DEBUG
     printf("xvgetd\n");
#endif /*DEBUG*/

  xvgetptr[xvgetcnt].xvns = xvnsparm;
  xvgetptr[xvgetcnt].xvar = xvarparm;
  xvgetcnt++;
}
