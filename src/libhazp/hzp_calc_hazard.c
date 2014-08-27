#include "hazpred.h"
/*
  hzp_calc_hazard

  Used by: hzpo

  Uses: 

  Globals:   
  HZPstr.Hazard 
  Hf[]
  HZPstr.earlyH
  HZPstr.constH
  HZPstr.lateH 

  Passed: 

  Returns: 


*/
void hzp_calc_hazard(void){
#ifdef DEBUG
     printf("hzp_calc_hazard\n");
#endif /*DEBUG*/

  HZPstr.Hazard = Hf[0]+Hf[1]+Hf[2];
  HZPstr.earlyH = Hf[0];
  HZPstr.constH = Hf[1];
  HZPstr.lateH = Hf[2];
}
