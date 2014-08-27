#include "common.h"

void hzr_calc_loge_mu(void){
  long k,ip,it,ix,Im1p7;
  double lmu[4];
  /**
   CALCULATE LOGE(MU(J)), THE LOG OF THE PARAMETRIC SCALING FUNCTION
  **/
  Im1p7=G.Im1p7;
  if(Common.p>0) {
    for(k=0; k<HZRstr.ivar; k++) {
      ip = HZRstr.indxp[k];
      it = HZRstr.indxt[k];
      ix = HZRstr.indxx[k];
      if(ix==0)
	/* INTERCEPT */
	lmu[ip] = Common.theta[it];
      else
	/* CONCOMITANT INFORMATION:  SUM (BETA(IP,IT)*X(I,IX)) */
	lmu[ip] += (Common.obs[ix+Im1p7]*Common.theta[it]);
    }
    G.lmu[1]=lmu[1];
    G.lmu[2]=lmu[2];
    G.lmu[3]=lmu[3];
  }
}
