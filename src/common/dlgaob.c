#include "structures.h"

double dlgaob(double A,double B){
#ifdef DEBUG
     printf("dlgoab\n");
#endif /*DEBUG*/

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
