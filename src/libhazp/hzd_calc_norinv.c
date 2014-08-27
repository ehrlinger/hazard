#include "hazpred.h"
/*
  hzd_calc_norinv

  Used by: hzpp

  Uses:

  Globals: Machn.half
  Machn.two
  Machn.norinv.C0
  Machn.norinv.C1
  Machn.norinv.C2
  Machn.norinv.D1
  Machn.norinv.D2
  Machn.norinv.D3

  Passed: P

  Returns: Y
*/
double hzd_calc_norinv(double P){
  double Q,R,Y,numer,denom;

#ifdef DEBUG
     printf("hzd_calc_norinv\n");
#endif /*DEBUG*/
  if(P<=ZERO || P>=ONE)
    return ZERO;
  if(P<=Machn.half)
    Q = P;
  else
    Q = ONE-P;
  R = sqrt(-Machn.two*LOGe(Q));
  numer = Machn.norinv.C0+R*(Machn.norinv.C1+Machn.norinv.C2*R);
  denom = ONE+R*(Machn.norinv.D1+
		 R*(Machn.norinv.D2+Machn.norinv.D3*R));
  Y = R-numer/denom;
  if(P<=Machn.half)
    return Y;
  else
    return -Y;
}
