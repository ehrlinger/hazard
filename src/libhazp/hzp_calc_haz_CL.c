#include "hazpred.h"
#include "hzd_Error.h"
/*  
    hzp_calc_haz_CL
    
    Used by: hzpo
    
    Uses:
    hzd_Error

    Globals:  
    seZ
    HXsX
    HZPstr.Hazard
    Z
    Zl
    HZPstr.Znormal
    Zu
    LnLim.absMax
    LnLim.absMin
    HZPstr.CL_L_H
    HZPstr.CL_U_H 

    Passed: 

    Returns: 



*/
void hzp_calc_haz_CL(void){
#ifdef DEBUG
     printf("hzp_calc_haz_CL\n");
#endif /*DEBUG*/
  seZ = sqrt(HXsX)/HZPstr.Hazard;
  Z = LOGe(HZPstr.Hazard);
  Zl = Z-HZPstr.Znormal*seZ;
  Zu = Z+HZPstr.Znormal*seZ;
  if(Zl>LnLim.absMax)
    hzd_Error("HZPO990",1);
  else
    if(Zl<LnLim.absMin)
      HZPstr.CL_L_H = ZERO;
    else
      HZPstr.CL_L_H = ePOW(Zl);
  if(Zu>LnLim.absMax)
    hzd_Error("HZPO1000",1);
  else
    if(Zu<LnLim.absMin)
      HZPstr.CL_U_H = ZERO;
    else
      HZPstr.CL_U_H = ePOW(Zu);
}
