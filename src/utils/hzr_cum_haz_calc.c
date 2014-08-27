#include "common.h"
#include "hzr_set_error.h"
#include <math.h>

logical hzr_cum_haz_calc_m(double *cum,double *cf,double lcf){
  if(lcf >= LnLim.absMax) {
    hzr_set_error("(SETCOE950)",1);
    return FALSE;
  }
  if(lcf <= LnLim.absMin) return TRUE;
  *cf = ePOW( lcf );
  *cum += *cf;
  return TRUE;
}

logical hzr_cum_haz_calc(double *cum,double *cf,double *lcf){
  long j;
  double temp0;

  for(j=1; j<=3; j++) {
    cf[j] = ZERO;
    if(Common.phase[j]!=1)
      continue;
    temp0 = lcf[j];
    if(temp0>=LnLim.absMax) {
      hzr_set_error("(SETCOE950)",1);
      return FALSE;
    }
    if(temp0>LnLim.absMin)
      cf[j] = ePOW(temp0);
  }
  *cum = cf[1]+cf[2]+cf[3];
  return TRUE;
}
