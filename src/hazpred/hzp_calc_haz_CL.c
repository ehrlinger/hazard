#include <string.h>
#include "hazpred.h"
#include "hzd_Error.h"

void hzp_calc_haz_CL(void){
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
