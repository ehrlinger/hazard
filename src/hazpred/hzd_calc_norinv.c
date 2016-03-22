#include <string.h>
#include "hazpred.h"

double hzd_calc_norinv(double P){
  double Q,R,Y,numer,denom;

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
