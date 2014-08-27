#include "hazpred.h"
#include "hzd_Error.h"
/*
  hzp_calc_fn - 

  Used by: hzpo

  Uses: hzd_Error

  Globals:
  Cf[j]
  Hf[j]
  Common.phase
  HZPstr.Sflag
  lnCf[j]
  LnLim.absMax
  LnLim.absMin
  HZPstr.Hflag
  lnHf[j]

  Passed: 

  Returns: 



*/
void hzp_calc_fn(void){
#ifdef DEBUG
     printf("hzp_calc_fn\n");
#endif /*DEBUG*/
  for(j=0; j<3; j++) {
    Cf[j] = ZERO;
    Hf[j] = ZERO;
    if(Common.phase[j+1]==1) {
      if(HZPstr.Sflag) {
	if(lnCf[j]>=LnLim.absMax)
	  hzd_Error("HZPO920",1);
	if(lnCf[j]>LnLim.absMin)
	  Cf[j] = ePOW(lnCf[j]);
      }
      if(HZPstr.Hflag) {
	if(lnHf[j]>=LnLim.absMax)
	  hzd_Error("HZPO930",1);
	if(lnHf[j]>LnLim.absMin)
	  Hf[j] = ePOW(lnHf[j]);
      }
    }
  }
}
