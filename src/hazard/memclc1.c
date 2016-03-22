#include <string.h>
#include "hazard.h"
/****************************************************************/
/* MEMCLC1 called from ALOCWRK                                  */
void memclc1(int phasno,int xt1st,long *xt2nd,long *xt3rd,
	     int *xt4th,int *xt5th){
  if(C->phase[phasno]==1) {
    *xt2nd = xt1st+C->Nobs;
    *xt3rd = *xt2nd+C->Nobs;
    if(intcen>0)
      *xt4th = *xt3rd+C->Nobs;
    else
      *xt4th = *xt3rd+1;
    *xt5th = *xt4th+C->Nobs;
  } else {
    *xt2nd = xt1st+1;
    *xt3rd = *xt2nd+1;
    *xt4th = *xt3rd+1;
    *xt5th = *xt4th+1;
  }
}
