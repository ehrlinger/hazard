#include <string.h>
#include "hazard.h"
#include <hzdinc.h>
#include <notintg.h>
#include <isblanck.h>

/****************************************************************/
/* READC1 called from READOBS                                   */
void readc1(void){
  if(!ISBLANCK(c1name))
    if(ISMISS(C1)) {
      mc1del = mc1del+1;
      mdel = 1;
    } else if(C1<ZERO) {
      c1del = c1del+1;
      del = 1;
    } else {
      if(C1==ZERO)
	ic10 = 1;
      else if(notintg(C1))
	c1flag = 1;
      c1min = c1min<C1 ? c1min : C1;
      c1max = c1max>C1 ? c1max : C1;
    }
  else
    C1 = ZERO;
}
