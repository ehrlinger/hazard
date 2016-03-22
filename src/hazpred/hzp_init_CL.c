#include <string.h>
#include "hazpred.h"
#include "hzp_refmt_cov.h"
void hzp_init_CL(void){
  sqrEps = sqrt(Machn.Eps);
  hzp_refmt_cov();
  if(HZPstr.Sflag)
    for(i=0; i<Common.Ntheta; i++)
      dCf[i] = ZERO;
  if(HZPstr.Hflag)
    for(i=0; i<Common.Ntheta; i++)
      dHf[i] = ZERO;
}
