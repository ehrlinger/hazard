#include "hazpred.h"
#include "hzd_ln_G1_and_SG1.h"
#include "hzd_ln_G3_and_SG3.h"
/*
  hzp_calc_log_fn

  Used by: hzpo

  Uses:
  hzd_ln_G1_and_SG1()
  hzd_ln_G3_and_SG3()

  Globals: 
  Common.phase[]
  HazG1.T
  Time
  lnCf[]
  lnMu[]
  HazG1.lnG1
  lnHf[]
  HazG1.lnSG1
  lnTime
  HazG3.T
  HazG3.lnG3;
  HazG3.lnSG3;

  Passed: 

  Returns: 



*/
void hzp_calc_log_fn(void){
#ifdef DEBUG
     printf("hzp_calc_log_fn\n");
#endif /*DEBUG*/

  if(Common.phase[1]==1) {
    HazG1.T = Time;
    hzd_ln_G1_and_SG1();
    lnCf[0] = lnMu[0]+HazG1.lnG1;
    lnHf[0] = lnMu[0]+HazG1.lnSG1;
  }

  if(Common.phase[2]==1) {
    lnCf[1] = lnMu[1]+lnTime;
    lnHf[1] = lnMu[1];
  }

  if(Common.phase[3]==1) {
    HazG3.T = Time;
    hzd_ln_G3_and_SG3();
    lnCf[2] = lnMu[2]+HazG3.lnG3;
    lnHf[2] = lnMu[2]+HazG3.lnSG3;
  }
}
