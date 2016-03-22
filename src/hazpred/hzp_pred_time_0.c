#include <string.h>
#include "hazpred.h"
#include "hzd_Error.h"

void hzp_pred_time_0(void){
  HZPstr.Surviv = ONE;
  HZPstr.CL_L_S = ONE;
  HZPstr.CL_U_S = ONE;
  HZPstr.earlyS = ONE;
  HZPstr.constS = ONE;
  HZPstr.lateS  = ONE;
  if(HZPstr.Hflag)
    hzd_Error("HZPO910",3);
}
