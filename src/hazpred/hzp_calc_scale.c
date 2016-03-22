#include <string.h>
#include "hazpred.h"

void hzp_calc_scale(void){
  double sum0;

  pp1 = Common.p+1;
  mp = -Common.p;
  pj1 = 8+mp;
  for(j=1; j<=3; j++) {
    if(Common.phase[j]==1) {
      j1p0 = pj1+pp1*j;
      sum0 = Common.theta[j1p0-1];
      if(Common.p>0) {
	j1p1 = j1p0+1;
	j1pp = j1p0+Common.p;
	jind = 1-j1p0;
	for(ji=j1p1; ji<=j1pp; ji++) {
	  indexp = ji+jind;
	  sum0 += Common.theta[ji-1]*Common.obs[indexp-1];
	}
      }
      lnMu[j-1] = sum0;
    }
  }
}
