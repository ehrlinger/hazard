#include <string.h>
#include "hazpred.h"
void hzp_calc_hazard(void){
  HZPstr.Hazard = Hf[0]+Hf[1]+Hf[2];
  HZPstr.earlyH = Hf[0];
  HZPstr.constH = Hf[1];
  HZPstr.lateH = Hf[2];
}
