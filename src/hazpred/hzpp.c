#include <string.h>
#include "hazpred.h"
#include "hzd_calc_norinv.h"
void hzpp(void){
  if(HZPstr.noCL)
    HZPstr.Znormal = ZERO;
  else
    if(HZPstr.CLimit<=ZERO || HZPstr.CLimit>=ONE)
      HZPstr.Znormal = ONE;
    else {
      HZPstr.Pvalue = (ONE-HZPstr.CLimit)/Machn.two;
      HZPstr.Znormal = fabs(hzd_calc_norinv(HZPstr.Pvalue));
    }
}
