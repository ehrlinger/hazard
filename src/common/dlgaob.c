/**
 * @file dlgaob.c
 * @brief Numerically stable computation of \f$\ln(A/B)\f$ via Mercator series.
 *
 * When the ratio A/B is near 1.0 and the log is near 0, direct evaluation
 * of ln(A/B) loses significant digits. The Mercator series with
 * \f$y = (A-B)/(A+B)\f$ provides full precision in this region.
 *
 * **FORTRAN origin:** `math91/dlgaob` (DLGAOB) — E.H. Blackstone, September 1985;
 * FORTRAN 77 translation from C.R. Katholi, Ph.D.
 * Verified September 30, 1985 against Katholi's original.
 *
 * @see dlgaob.h
 * @see hzd_ln_A_div_B.c for the simpler wrapper.
 */
#include <string.h>
#include "structures.h"

double dlgaob(double A,double B){
  double ratio,y,sum,term,ix,denom;
  int i;

  ErrorReset;

  if(A<=ZERO || B<=ZERO) ErrorReturn(2,ZERO);

  ratio = A/B;
  if(ratio<=ZERO) ErrorReturn(2,ZERO);

  if(ratio<=Machn.absMin || ratio>=Machn.absMax)
    ErrorReturn(1,ZERO);

  if(ratio<LnLim.divBtm || ratio>LnLim.divTop)
    return LOGe(ratio);

  y = (A-B)/(A+B);
  if(y==ZERO)
    return ZERO;

  sum = y;
  term = y;
  y = y*y;
  ix = ZERO;
  for(i=1; i<=13; i++) {
    term = term*y;
    ix += ONE;
    denom = (TWO*ix)+ONE;
    sum += term/denom;
    if(fabs(term)<(Machn.Eps*fabs(sum)))
      return TWO*sum;
  }
  return TWO*sum;
}
