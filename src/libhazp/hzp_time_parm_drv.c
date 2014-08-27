
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "hazpred.h"
#include "hzd_theta_to_parms.h"
#include "hzd_ln_G1_and_SG1.h"
#include "hzd_ln_G3_and_SG3.h"
/*
  hzp_time_parm_drv
  
  Used by: hzp_calc_parm_drv

  Uses:
  hzd_theta_to_parms()
  hzd_ln_G1_and_SG1()
  hzd_ln_G3_and_SG3()

  Globals:  
  tempj
  Common.theta[j];
  step[j]
  sqrEps
  SaveE
  Early;
  DlnG[j]
  HazG1.lnG1;
  DlnSG[j]
  HazG1.lnSG1
  SaveL
  Late
  HazG3.lnG3;
  HazG3.lnSG3;

  Passed: 

  Returns: 
*/
void hzp_time_parm_drv(void){
#ifdef DEBUG
     printf("hzp_time_parm_drv\n");
#endif /*DEBUG*/

  tempj = Common.theta[j];
  step[j] = sqrEps*tempj;
  Common.theta[j] = step[j]+tempj;
  step[j] = Common.theta[j]-tempj;
  if(j<4) {
    SaveE = Early;
    hzd_theta_to_parms(1,0);
    hzd_ln_G1_and_SG1();
    DlnG[j] = HazG1.lnG1;
    DlnSG[j] = HazG1.lnSG1;
    Early = SaveE;
  } else {
    SaveL = Late;
    hzd_theta_to_parms(0,1);
    hzd_ln_G3_and_SG3();
    DlnG[j] = HazG3.lnG3;
    DlnSG[j] = HazG3.lnSG3;
    Late = SaveL;
  }
  Common.theta[j] = tempj;
}
