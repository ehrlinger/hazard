/**
 * @file hzd_ln_eX_min1.c
 * @brief Numerically stable computation of \f$\ln(e^x - 1)\f$, \f$x > 0\f$.
 *
 * Avoids catastrophic cancellation when \f$x\f$ is small (where
 * \f$e^x \approx 1\f$) by switching to the Taylor series
 * \f$e^x - 1 = \sum_{k=1}^{N} x^k / k!\f$, truncated when the
 * next term falls below machine precision.
 *
 * **FORTRAN origin:** `math91/dlexm1` — E.H. Blackstone, March 1986;
 * FORTRAN 77 translation from earlier work by C.R. Katholi, Ph.D.
 * Verified December 3, 1989; November 1994 revisions by E.H. Blackstone.
 *
 * @see hzd_ln_eX_min1.h
 * @see hzd_ln_eBXmin1divB.c
 */
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


