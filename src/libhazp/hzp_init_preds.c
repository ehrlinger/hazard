#include "hazpred.h"
/*
  hzp_init_preds initializer
  
  Used by: hzpo

  Uses:

  Globals: 
  HZPstr.Surviv
  HZPstr.CL_L_S
  HZPstr.CL_U_S
  HZPstr.earlyS
  HZPstr.constS
  HZPstr.lateS 
  HZPstr.Hazard
  HZPstr.CL_L_H
  HZPstr.CL_U_H
  HZPstr.earlyH
  HZPstr.constH
  HZPstr.lateH 

  Passed: 

  Returns: 



*/
void hzp_init_preds(void){
#ifdef DEBUG
     printf("hzp_init_preds\n");
#endif /*DEBUG*/

  HZPstr.Surviv = ZERO;
  HZPstr.CL_L_S = ZERO;
  HZPstr.CL_U_S = ZERO;
  HZPstr.earlyS = ZERO;
  HZPstr.constS = ZERO;
  HZPstr.lateS  = ZERO;
  HZPstr.Hazard = ZERO;
  HZPstr.CL_L_H = ZERO;
  HZPstr.CL_U_H = ZERO;
  HZPstr.earlyH = ZERO;
  HZPstr.constH = ZERO;
  HZPstr.lateH  = ZERO;
}
