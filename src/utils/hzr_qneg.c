#include <string.h>
#include "common.h"
#ifdef QEMULATE 
xtended hzr_qneg(xtended a){
  xtended x;

  x.hi = -a.hi;
  x.lo = -a.lo;
  return x;
}
#endif
