#include <string.h>
#include <float.h>
#include "structures.h"

/*------------------------------------------------------------------*/
/* Completed and results compared with DLEXM1, 12/03/89.            */
/* Modifications incorporated from 02Nov94 changes by EHB.          */
/*------------------------------------------------------------------*/

double hzd_ln_eX_min1(double X){
  double sum,prod,denom,LOGRdxX,absX;
  int i,maxCalc;

  ErrorReset;

  absX = fabs(X);

  if(absX>LnLim.onePlsRdx && absX<LnLim.maxOneSig)
    return LOGe(fabs(ePOW(X)-ONE));

  if(X>=LnLim.maxOneSig)
    return X;

  if(X<=LnLim.absMin)
    return ZERO;

  sum = X;
  prod = X;
  denom = ONE;
  LOGRdxX = LOGe(fabs(X))/LnLim.fpRdx;
  maxCalc = (LOGRdxX-DBL_MANT_DIG)/LOGRdxX;
  if(maxCalc>LnLim.bndSeries)
    maxCalc = LnLim.bndSeries;
  for(i=2; i<=maxCalc; i++) {
    denom = denom+ONE;
    prod = prod*(X/denom);
    sum = sum+prod;
  }
  return LOGe(fabs(sum));
}


