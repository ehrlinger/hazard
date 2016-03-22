#include <string.h>
#include "hazard.h"
#include <isblanck.h>
#include <notintg.h>

/****************************************************************/
/* READC2 called from READOBS                                   */
void readc2(void){
  if(!ISBLANCK(c2name))
    if(ISMISS(C2)) {
      mc2del = mc2del+1;
      mdel = 1;
    } else if(C2<ZERO) {
      c2del = c2del+1;
      del = 1;
    } else {
      if(C2==ZERO)
	ic20 = 1;
      else if(notintg(C2))
	c2flag = 1;
      c2min = c2min<C2 ? c2min : C2;
      c2max = c2max>C2 ? c2max : C2;
    }
  else
    if((ISBLANCK(c1name) && C3==ZERO) || (ISBLANCK(c3name) && C1==ZERO) ||
       (!ISBLANCK(c1name) && !ISBLANCK(c3name) && C1==ZERO && C3==ZERO)) {
      C2 = ONE;
      c2chg = c2chg+1;
      c2min = c2min<C2 ? c2min : C2;
      c2max = c2max>C2 ? c2max : C2;
    } else
      C2 = ZERO;
}
