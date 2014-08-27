#include "hazpred.h"
#include "hzd_ln_1min_enegX.h"
#include "hzd_Error.h"
#include "hzd_ln_eX_min1.h"
#include "hzd_ln_eX_plus1.h"

/*
  hzp_calc_srv_C

  Used by: hzpo

  Uses: 
  hzd_ln_1min_enegX
  hzd_ln_eX_min1
  hzd_ln_eX_plus1
  hzd_Error

  Globals: 
  cumHaz
  LnLim.absMax
  LnLim.absMin
  seZ
  CXsX
  Z
  Zl
  HZPstr.Znormal
  Zu
  HZPstr.CL_L_S
  HZPstr.CL_U_S

  Passed: 

  Returns: 



*/
void hzp_calc_srv_CL(void){
#ifdef DEBUG
     printf("hzp_calc_srv_CL\n");
#endif /*DEBUG*/

  double Slogit,logitS,temp0;

  logitS = -hzd_ln_1min_enegX(cumHaz);
  if(logitS>LnLim.absMax)
    hzd_Error("HZPO970",1);
  if(logitS<LnLim.absMin)
    Slogit = ZERO;
  else
    Slogit = ePOW(logitS);
  seZ = sqrt(CXsX)*Slogit;
  Z = hzd_ln_eX_min1(cumHaz);
  Zl = Z-HZPstr.Znormal*seZ;
  Zu = Z+HZPstr.Znormal*seZ;
  temp0 = hzd_ln_eX_plus1(Zu);
  if(temp0>LnLim.absMax)
    HZPstr.CL_L_S = ZERO;
  else
    if(-temp0<LnLim.absMin)
      HZPstr.CL_L_S = ONE;
    else
      HZPstr.CL_L_S = ePOW(-temp0);
  temp0 = hzd_ln_eX_plus1(Zl);
  if(temp0>LnLim.absMax)
    HZPstr.CL_U_S = ZERO;
  else
    if(-temp0<LnLim.absMin)
      HZPstr.CL_U_S = ONE;
    else
      HZPstr.CL_U_S = ePOW(-temp0);
}
