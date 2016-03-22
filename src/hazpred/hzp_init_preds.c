#include <string.h>
#include "hazpred.h"

void hzp_init_preds(void){
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
