#include <hzdinc.h>
#include "hazard.h"

#include <setg1.h>
#include <setg3.h>
#include <hzr_parm.h>
#include <dtrsmu.h>
#include <setcoe.h>
#include <hzd_parms_to_theta.h>

void shape(void){
  /* long one,two,j;*/

  CLEAR(Common.errflg);
  CLEAR(HZRstr.chgflg);
  HZRstr.ierr = 0;
  Common.errorno = 0;
  if(Common.phase[1]==1)
    if(!setjmp(Common.errtrap))
      SETG1();
  HZRstr.ierr = Common.errorno;
  if(Common.phase[2]==1){
    if(HZRstr.c.muC<=ZERO) {
      Constant.muC = ONE;
      hzr_parm_changed(HZ_MUC);
    } else{
      Constant.muC = HZRstr.c.muC;
    }
  }
  if(Common.phase[3]==1){
    if(!setjmp(Common.errtrap)){
      SETG3();
    }
  }
  if(hzr_parm_is_fixed(HZ_DELTA) && hzr_parm_is_fixed(HZ_THALF) &&
     hzr_parm_is_fixed(HZ_NU) && hzr_parm_is_fixed(HZ_M))
    HZRstr.fixg1 = TRUE;
  if(hzr_parm_is_fixed(HZ_TAU) && hzr_parm_is_fixed(HZ_GAMMA) &&
     hzr_parm_is_fixed(HZ_ALPHA) && hzr_parm_is_fixed(HZ_ETA))
    HZRstr.fixg3 = TRUE;
  if(HZRstr.mode==0 || !HZRstr.fixg1 || !HZRstr.fixg3) {
    HZRstr.angrad = FALSE;
    HZRstr.anhess = FALSE;
  }
  if(Common.errorno!=0 || HZRstr.ierr!=0) 
    return;
  if(setjmp(Common.errtrap))
    return;
  hzd_parms_to_theta();
  DTRSMU(1);
  setcoe();
  DTRSMU(2);
  /* Calls to setobj, setlik, setdll are no longer needed. */
}
