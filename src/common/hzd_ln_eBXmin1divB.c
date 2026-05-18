/**
 * @file hzd_ln_eBXmin1divB.c
 * @brief Numerically stable computation of \f$\ln\!\left(\frac{e^{\beta x}-1}{\beta}\right)\f$.
 *
 * **FORTRAN origin:** `math91/dlem1d` (DLEM1D) — E.H. Blackstone.
 * Verified December 3, 1989; November 1994 revisions by E.H. Blackstone.
 *
 * @see hzd_ln_eBXmin1divB.h
 * @see hzd_ln_eX_min1.c (called as subroutine for \f$\beta x > 0.2\f$)
 */
#include <string.h>
#include "structures.h"
#include <float.h>
#include "hzd_Error.h"
#include "hzd_ln_eX_min1.h"

/*-----------------------------------------------------------------*/
/* Completed and results compared with DLEM1D, 12/03/89.           */
/* Modifications incorporated from 02Nov94 changes by EHB.         */
/*-----------------------------------------------------------------*/

double hzd_ln_eBXmin1divB(double BETA,double X){
  double BX,sum,prod,denom,LOGRdxBX,LOGeBX,absBETA;
  int i,maxCalc;

  ErrorReset;

  if(X<=ZERO) ErrorReturn(2,ZERO);

  BX = BETA*X;
  absBETA = fabs(BETA);

  LOGeBX = LOGe(absBETA)+LOGe(X);
  if(LOGeBX<=LnLim.absMin || LOGeBX>=LnLim.absMax)
    hzd_Error("DLEM1D50",2);

  if(BX>LnLim.onePlsRdx || BX<LnLim.oneMinRdx)
    return hzd_ln_eX_min1(BX)-LOGe(absBETA);

  sum = ONE;
  prod = ONE;
  denom = ONE;
  LOGRdxBX = LOGeBX/LnLim.fpRdx;
  maxCalc = (int) ((LOGRdxBX-(DBL_MANT_DIG-1))/LOGRdxBX);
  if(maxCalc>LnLim.bndSeries)
    maxCalc = LnLim.bndSeries;
  for(i=2; i<=maxCalc; i++) {
    denom = denom+ONE;
    prod = prod*(BX/denom);
    sum = sum+prod;
  }
  return LOGe(sum)+LOGe(X);
}
