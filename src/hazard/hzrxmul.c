#include <common.h>
#include "hazard.h"

xtended hzr_xmul(double a,double c){
  double a1,a2,c1,c2,c21,c22,t;
  xtended x;

  t = a*k_xmul; a1 = (a-t)+t; a2 = a-a1;
  t = c*k_xmul; c1 = (c-t)+t; c2 = c-c1;
  t = c2*k_xmul; c21 = (c2-t)+t; c22 = c2-c21;
  x.hi = a*c;
  x.lo = ((((a1*c1-x.hi)+a1*c2)+c1*a2)+c21*a2)+c22*a2;
  return x;
}
