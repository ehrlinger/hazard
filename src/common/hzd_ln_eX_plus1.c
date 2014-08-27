#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "structures.h"
#include <float.h>
/*------------------------------------------------------------------*/

double hzd_ln_eX_plus1(double X){
#ifdef DEBUG
     printf("hzd_ln_eX_plus1\n");
#endif /*DEBUG*/

  double sum,prod,denom,LOGRdxX,eX,Z,index;
  int i,maxCalc;

  if(X>LnLim.oneMinRdx && X<LnLim.maxOneSig)
    return LOGe(ePOW(X)+ONE);

  if(X>=LnLim.absMax || X>=(LnLim.two-LnLim.absMin))
    return X;

  if(X<=LnLim.absMin)
    return ZERO;

  if(X>LnLim.absMin && X<LnLim.oneMinRdx) {
    eX = ePOW(X);
    sum = eX;
    prod = eX;
    denom = ONE;
    LOGRdxX = X/LnLim.fpRdx;
    maxCalc = (int) ((LOGRdxX-DBL_MANT_DIG)/LOGRdxX);
    for(i=2; i<=maxCalc; i++) {
      denom = denom+ONE;
      prod = -prod*eX;
      sum = sum+(prod/denom);
    }
    return sum;
  }

  Z = ONE/(Machn.two*ePOW(X)+ONE);
  prod = Machn.two*Z;
  sum = X+prod;
  LOGRdxX = LOGe(Z)/LnLim.fpRdx;
  maxCalc = (int) ((LOGRdxX-DBL_MANT_DIG)/(LOGRdxX*2));
  if(maxCalc<2)
    return sum;
  Z = Z*Z;
  index = ONE;
  for(i=2; i<=maxCalc; i++) {
    prod = prod*Z;
    index = index+ONE;
    denom = (Machn.two*index)-ONE;
    sum = sum+(prod/denom);
  }
  return sum;
}
