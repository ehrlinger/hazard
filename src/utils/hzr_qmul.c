#include "common.h"
#include "hzr_xmul.h"
#ifdef QEMULATE

xtended hzr_qmul(xtended a,xtended c){
  xtended x,z;
  register double zz;

  z = hzr_xmul(a.hi,c.hi);
  zz = ((a.hi+a.lo)*c.lo+a.lo*c.hi)+z.lo;
  x.hi = z.hi+zz;
  x.lo = (z.hi-x.hi)+zz;
  return x;
}
#endif
