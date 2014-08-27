#include "hazpred.h"

#include "hzd_Error.h"
#include "hzp_init_preds.h"
#include "hzp_pred_time_0.h"
#include "hzp_calc_scale.h"
#include "hzp_calc_log_fn.h"
#include "hzp_calc_fn.h"
#include "hzp_calc_survival.h"
#include "hzp_calc_hazard.h"
#include "hzp_calc_parm_drv.h"
#include "hzp_calc_intcp_drv.h"
#include "hzp_calc_XsX.h"
#include "hzp_calc_srv_CL.h"
#include "hzp_calc_haz_CL.h"
/*
  hzpo - do the actual time step.
 
  Used by: Called by main program

  Uses: hzp_init_preds
  hzp_pred_time_0
  hzp_calc_scale();
  hzp_calc_log_fn();
  hzp_calc_fn();
  hzp_calc_survival();
  hzp_calc_hazard();
  hzp_calc_parm_drv(0|2);
  hzp_calc_intcp_drv();
  hzp_calc_srv_CL();
  hzp_calc_haz_CL();
  hzd_Error

  Globals: optmz
  Time
  Common.obs[0];
  lnTime
  Chgd_Obs
  HZPstr.Sflag
  HZPstr.Hflag
  
  Passed: 
  int optmz_parm
  int do_CL (actually, I think it's an enum)
  HZPstr.Znormal
  Common.phase[1|3]
  Common.status[]

  Returns: 

*/
void hzpo(int optmz_parm,int do_CL){
#ifdef DEBUG
     printf("hzpo\n");
#endif /*DEBUG*/

  optmz = optmz_parm;

  Time = Common.obs[0];
  if(Time<0)
    hzd_Error("HZPO900",2);

  /* Initialize the answers to zero */
  hzp_init_preds();

  /* The zero step. */ 
  if(Time==ZERO) {
#ifdef DEBUG
    printf("hzpo zero Step... I didn't think we'd ever get here.\n");
#endif /* DEBUG */
    hzp_pred_time_0();
    return;
  }

  /* Scale the time  */
  lnTime = LOGe(Time);
  if(optmz&Chgd_Obs)
    hzp_calc_scale();
  hzp_calc_log_fn();
  hzp_calc_fn();
  if(HZPstr.Sflag)
    hzp_calc_survival();
  if(HZPstr.Hflag)
    hzp_calc_hazard();
  if(!do_CL)
    return;
  if(HZPstr.Znormal<=ZERO)
    return;

  if(Common.phase[1]==1)
    for(j=0; j<4; j++)
      if(Common.status[j]==1)
	hzp_calc_parm_drv(0);
  if(Common.phase[3]==1)
    for(j=4; j<8; j++)
      if(Common.status[j]==1)
	hzp_calc_parm_drv(2);
  hzp_calc_intcp_drv();
  if(HZPstr.Sflag)
    CXsX = hzp_calc_XsX(dCf);
  if(HZPstr.Hflag)
    HXsX = hzp_calc_XsX(dHf);
  if(HZPstr.Sflag)
    hzp_calc_srv_CL();
  if(HZPstr.Hflag)
    hzp_calc_haz_CL();
}
