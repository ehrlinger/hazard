#include "hazpred.h"
#include "hzp_refmt_cov.h"
/*
  hzp_init_CL - Initialize confidence limits (?)
  
  Used by: hzpe

  Uses: hzp_refmt_cov();  

  Globals: 
  sqrEps
  Machn.Eps
  HZPstr.Sflag
  dCf[i]
  HZPstr.Hflag
  Common.Ntheta
  dHf

  Passed: 

  Returns: 

*/

void hzp_init_CL(void){
#ifdef DEBUG
     printf("hzp_iit_CL\n");
#endif /*DEBUG*/

  sqrEps = sqrt(Machn.Eps);
  hzp_refmt_cov();
  if(HZPstr.Sflag)
    for(i=0; i<Common.Ntheta; i++)
      dCf[i] = ZERO;
  if(HZPstr.Hflag)
    for(i=0; i<Common.Ntheta; i++)
      dHf[i] = ZERO;
}
