#include <common.h>
#include <hzr_qtod.h>
#include <hzd_ln_A_div_B.h>
#include <hzr_set_error.h>

#include "setcoe.h"

void SETCOE_calc_scaling(void){
  double sumchz,lfactr;
  long j,indexp;
 
  /**
     NOW CALCULATE THE SCALING FACTOR FOR THE ENTIRE MODEL WHICH WILL
     CONSERVE EVENTS.
  **/
  sumchz = QTOD(Setcoe.qsumcz);
  lfactr = hzd_ln_A_div_B(Setcoe.sc1pc3,sumchz);
  if(Common.errorno) {
    hzr_set_error("(SETCOE1020)",Common.errorno);
    return;
  }
  HZRstr.factor = ePOW(lfactr);
 
  /* APPLY SCALE FACTOR TO EACH PHASE INTERCEPT */
  if(HZRstr.mode<2)
    for(j=1; j<=3; j++)
      if(Common.phase[j]==1) {
	indexp = HZRstr.pj1+HZRstr.pp1*j-1;
	Common.theta[indexp] += lfactr;
      }
}
