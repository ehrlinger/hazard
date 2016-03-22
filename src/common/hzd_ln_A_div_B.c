#include <string.h>
#include "structures.h"

double hzd_ln_A_div_B(double A,double B){
  double ratio;

  if(A<=ZERO || B<=ZERO) ErrorReturn(2,ZERO);

  ratio = A/B;
  if(ratio<=ZERO) ErrorReturn(2,ZERO);

  if(ratio<=Machn.absMin || ratio>=Machn.absMax)
    ErrorReturn(1,ZERO);

  return LOGe(ratio);
}
