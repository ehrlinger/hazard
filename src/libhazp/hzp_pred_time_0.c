#include "hazpred.h"
#include "hzd_Error.h"
/*
  hzp_pred_time_0

  Used by: hzpo

  Uses:

  Globals: 
  HZPstr.Surviv 
  HZPstr.CL_L_S
  HZPstr.CL_U_S
  HZPstr.earlyS
  HZPstr.constS
  HZPstr.lateS 
  HZPstr.Hflag

  Passed: 

  Returns: 



*/
void hzp_pred_time_0(void){
#ifdef DEBUG
     printf("hzp_pred_time_0\n");
#endif /*DEBUG*/
  HZPstr.Surviv = ONE;
  HZPstr.CL_L_S = ONE;
  HZPstr.CL_U_S = ONE;
  HZPstr.earlyS = ONE;
  HZPstr.constS = ONE;
  HZPstr.lateS  = ONE;
  if(HZPstr.Hflag)
    hzd_Error("HZPO910",3);
}
