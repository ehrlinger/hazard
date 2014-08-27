#include "hazpred.h"
/*
  hzp_calc_scale

  Used by: hzpo

  Uses: 

  Globals:   
  pp1
  Common.p
  mp
  pj1
  Common.phase[j]
  j1p0
  j1p1
  j1pp
  jind
  Common.theta[]
  Common.obs[];
  lnMu[j-1]

  Passed: 

  Returns: 



*/
void hzp_calc_scale(void){
#ifdef DEBUG
  printf("hzp_calc_scale\n");
#endif
  double sum0;

  /* p is a long */
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
