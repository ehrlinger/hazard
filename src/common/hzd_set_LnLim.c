#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "structures.h"
#include <float.h>

void hzd_set_LnLim(void){
#ifdef DEBUG
     printf("hzd_set_LnLim\n");
#endif /*DEBUG*/

  double d;
  int i;

  
  LnLim.two = LOGe(Machn.two);
  LnLim.twotwo = LOGe(LnLim.two);
  LnLim.absMin = LOGe(Machn.absMin);
  LnLim.absMax = LOGe(Machn.absMax);
  LnLim.divBtm = ePOW(-0.1e0);
  LnLim.divTop = ePOW(0.1e0);
  LnLim.fpRdx = LOGe((double)FLT_RADIX);
  LnLim.onePlsRdx = LOGe(ONE+ONE/FLT_RADIX);
  LnLim.oneMinRdx = LOGe(ONE-ONE/FLT_RADIX);

  for(i=0,d=ZERO; i<DBL_MANT_DIG; i++)
    d = d*FLT_RADIX+(FLT_RADIX-1);

  LnLim.maxOneSig = LOGe(d+ONE);

  for(d=(double)DBL_MANT_DIG,i=1; d>ZERO; i++)
    d = (d-ONE)-(LOGe((double)i)/LnLim.fpRdx);
  LnLim.bndSeries = i;
}
