#include <float.h>
#include <math.h>
#include "hzdinc.h"
#include "common.h"

#ifdef QEMULATE
void hzr_qinit(void){
  double beta,p,xp;

  beta = FLT_RADIX;
  p = DBL_MANT_DIG;
  xp = p-floor(p/TWO);
  k_xmul = pow(beta,xp)+ONE;
}
#endif 
