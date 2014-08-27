#include "hazard.h"

/****************************************************************/
/* MEMCLC2 called from ALOCWRK                                  */
void memclc2(long phasno,long xt1st,long *xt2nd){
  if(C->phase[phasno]==1)
    *xt2nd = xt1st+C->Nobs;
  else
    *xt2nd = xt1st+1;
}
