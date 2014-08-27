#include <structures.h>
#include <hzd_set_Machn.h>
#include <hzd_set_LnLim.h>
#include <hzd_init_Common.h>
#include "hazard.h"

struct common *hzri(void){
  hzd_set_Machn();
  hzd_set_LnLim();
  hzd_init_Common();
  Common.hzrstr = &HZRstr;
  Version.core_ver = HZRVER;
  HZRstr.Mu[1] = &Early.muE;
  HZRstr.Mu[2] = &Constant.muC;
  HZRstr.Mu[3] = &Late.muL;

  return &Common;
}
