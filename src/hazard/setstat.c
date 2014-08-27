#include "hazard.h"
#include <hzdinc.h>
#include "isblank.h"
/****************************************************************/
/* SETSTAT called from SETCONC                                  */
void setstat(long i,long idx,long *phasevar,long *itabl,
		    long *ipxc){
  if((memcmp(faze[i],"EARLY   ",8)==0 && C->phase[1]!=1) ||
     (memcmp(faze[i],"CONSTANT",8)==0 && C->phase[2]!=1) ||
     (memcmp(faze[i],"LATE    ",8)==0 && C->phase[3]!=1))
    return;
  *phasevar = 1;
  *itabl = 1;
  H->mxmove[idx] = nomovs[i];
  O->orders[idx] = O->swordr[i];
  if(ISBLANK(inex[i]))
    if(H->sw==0) {
      C->theta[idx] = stval[i];
      C->status[idx] = 1;
    } else
      C->status[idx] = 0;
  else if(memcmp(inex[i],"START   ",8)==0) {
    C->theta[idx] = stval[i];
    C->status[idx] = 1;
  } else if(memcmp(inex[i],"INCLUDE ",8)==0) {
    C->theta[idx] = stval[i];
    C->status[idx] = 1;
    H->mxmove[idx] = 0;
  } else {
    *ipxc = 1;
    C->status[idx] = 0;
    H->mxmove[idx] = 0;
  }
}
