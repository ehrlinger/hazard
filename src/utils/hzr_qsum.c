#include <string.h>
#include "common.h"

#ifdef QEMULATE
xtended hzr_qsum(xtended a,xtended c){
  double z,q;
  register double zz;
  xtended x;

  z = a.hi+c.hi;
  q = a.hi-z;
  zz = (((q+c.hi)+(a.hi-(q+z)))+a.lo)+c.lo;
  x.hi = z+zz;
  x.lo = (z-x.hi)+zz;
  return x;
}
#endif
