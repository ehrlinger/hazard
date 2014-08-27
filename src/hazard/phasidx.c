#include <string.h>

#include "hazard.h"
#include "setidxs.h"

/****************************************************************/
/* PHASEIDX called from hazrd2.c:HZ2LOOP, hazrd4.c:HZ4LOOP      */
void phasidx(long *i,long phasno,long spt,long ept){
  long j;

  if(C->phase[phasno]!=1)
    return;
  if((H->mode==0 && H->fixmu==phasno) || C->status[spt]==1) {
    H->index[*i] = spt;
    memcpy(H->namex[*i],H->names[spt],VAR_NAME_LENGTH);
    H->x[*i] = C->theta[spt];
    (*i)++;
  }
  for(j=spt+1; j<=ept; j++) {
    if(C->status[j]==1)
      setidxs((*i)++,j);
    if(H->x[*i]!=ZERO)
      H->retry = 0;
  }
}
