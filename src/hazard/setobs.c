#include <string.h>
#include "hazard.h"

/****************************************************************/
/* SETOBS called from READOBS                                   */
void setobs(void){
  int i,j;

  for(i=0; i<C->p; i++) {
    contal[i] += concom[i];
    conmin[i] = conmin[i]<concom[i] ? conmin[i] : concom[i];
    conmax[i] = conmax[i]>concom[i] ? conmax[i] : concom[i];
  }
  events = events+C1;
  rtcen = rtcen+C2;
  intcen = intcen+C3;
  nosubs = nosubs+(C1+C2+C3);
  tally = tally+C1+C3;
  C->obs[obsix] = T;
  C->obs[obsix+1] = C1;
  C->obs[obsix+2] = C2;
  C->obs[obsix+3] = C3;
  C->obs[obsix+4] = CT;
  C->obs[obsix+5] = LC;
  C->obs[obsix+6] = WT;
  for(j=0; j<C->p; j++)
    C->obs[obsix+7+j] = concom[j];
  obsix = obsix+pp7;
}
