#include <string.h>
#include "hzd_set_Machn.h"
#include "hzd_set_LnLim.h"
#include "hzd_init_Common.h"
#include "hazpred.h"

struct common *hzpi(void){
  hzd_set_Machn();
  hzd_set_LnLim();
  hzd_init_Common();
  Common.hzpstr = &HZPstr;
  Version.core_ver = HZPVER;
#ifdef HZDPOSTI
  HZDPOSTI();
#endif
  return &Common;
}
