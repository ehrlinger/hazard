#include "hazpred.h"
#include "xvgetd.h"
#include "hazverr.h"
#include <string.h>

void defvars(void){
  long j;

  if(inhaz_ns[0].ntype!=2 || memcmp(inhaz_ns[0].nname,"_NAME_  ",8))
    hazverr(1);
  if(inhaz_ns[1].ntype!=1 || memcmp(inhaz_ns[1].nname,"_EST_   ",8))
    hazverr(2);
  if(inhaz_ns[2].ntype!=1 || memcmp(inhaz_ns[2].nname,"_STATUS_",8))
    hazverr(3);
  xvgetd(&inhaz_ns[0],in_name);
  xvgetd(&inhaz_ns[1],&in_est);
  xvgetd(&inhaz_ns[2],&in_status);
  if(H->noCL)
    return;
  for(j=0; j<3*C->p+11; j++) {
    if(inhaz_ns[j+3].ntype!=1)
      hazverr(j+4);
    memcpy(namev[j],inhaz_ns[j+3].nname,VAR_NAME_LENGTH);
    xvgetd(&inhaz_ns[j+3],&C->obs[j]);
  }
}
