#include "hazard.h"
#include "isblank.h"
#include "notintg.h"

/****************************************************************/
/* READC3 called from READOBS                                   */
void readc3(void){
  if(!ISBLANK(c3name))
    if(ISMISS(C3)) {
      mc3del = mc3del+1;
      mdel = 1;
    } else if(C3<ZERO) {
      c3del = c3del+1;
      del = 1;
    } else {
      if(C3==ZERO)
	ic30 = 1;
      else if(notintg(C3))
	c3flag = 1;
      c3min = c3min<C3 ? c3min : C3;
      c3max = c3max>C3 ? c3max : C3;
    }
  else
    C3 = CT = ZERO;
}
