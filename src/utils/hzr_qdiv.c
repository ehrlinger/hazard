#include "common.h"
#ifdef QEMULATE

xtended hzr_qdiv(xtended a,xtended c){
  xtended x;

  x.hi = a.hi/c.hi;
  x.lo = ZERO;
  return x;
}
#endif
