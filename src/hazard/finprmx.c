#include <string.h>
#include "hazard.h"
/****************************************************************/
/* FINPRMX called from THETA_X                                  */
void finprmx(int *i,int phasno,long spt,long ept){
  int j;

  if(C->phase[phasno]!=1)
    return;
  if(H->mode==0 && H->fixmu==phasno)
    H->x[(*i)++] = C->theta[spt];
  for(j=spt; j<=ept; j++)
    if(C->status[j]==1)
      H->x[(*i)++] = C->theta[j];
}
