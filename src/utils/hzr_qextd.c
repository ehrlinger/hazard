#include <string.h>
#include "common.h"

#ifdef QEMULATE
xtended hzr_qextd(double a){
  xtended x;

  x.hi = a;
  x.lo = ZERO;
  return x;
}
#else

xtended hzr_qextd(double a){
  xtended x;

  x = a;
  return x;
}
#endif 
