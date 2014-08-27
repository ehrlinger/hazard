#include "structures.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <float.h>

/*------------------------------------------------------------------*/
/* Completed and results compared with DL1MEX, 12/04/89.            */
/*------------------------------------------------------------------*/

double hzd_ln_1min_enegX(double X){
#ifdef DEBUG
     printf("hzdln_1min_enegX\n");
#endif /*DEBUG*/

  double sum,prod,denom,empmx,prdvd,LOGRdxX;
  int i,maxCalc;

  ErrorReset;

  if(X>(-LnLim.oneMinRdx) && X<LnLim.fpRdx)
    return LOGe(ONE-ePOW(-X));

  if(X>=(-LnLim.absMin))
    return ZERO;

  if(X<=ZERO) ErrorReturn(2,ZERO);

  if(X>=LnLim.fpRdx) {
    empmx = ePOW(-X);
    sum = empmx;
    prod = empmx;
    denom = ONE;
    LOGRdxX = -X/LnLim.fpRdx;
    maxCalc = (int) ((LOGRdxX-DBL_MANT_DIG)/LOGRdxX);
    for(i=2; i<=maxCalc; i++) {
      denom = denom+ONE;
      prod = prod*empmx;
      prdvd = prod/denom;
      sum = sum+prdvd;
    }
    return -sum;
  }

  sum = X;
  prod = X;
  denom = ONE;
  LOGRdxX = LOGe(X)/LnLim.fpRdx;
  maxCalc = (int) ((LOGRdxX-DBL_MANT_DIG)/LOGRdxX);
  if(maxCalc>LnLim.bndSeries)
    maxCalc = LnLim.bndSeries;
  for(i=2; i<=maxCalc; i++) {
    denom = denom+ONE;
    prod = -prod*(X/denom);
    sum = sum+prod;
  }
  return LOGe(sum);
}
