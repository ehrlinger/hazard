#include <string.h>
#include <common.h>

#include "setcoe_setup_indices.h"
#include <hzr_qextd.h>
#include "setcoe_obs_loop.h"
#include "setcoe_calc_scaling.h"

#include "setcoe.h"

void constp(void)
     /*--------------------------------------------------------------
      ENTRY TO REINITIALIZE CONSERVATION OF EVENTS ROUTINE IF ONE 
      IS IN A STEPWISE VARIABLE SELECTION MODE.
 
      THIS ENTRY SHOULD BE CALLED JUST PRIOR TO THE 
      OPTIMIZATION STEP.
      -------------------------------------------------------------*/
{
  int j;
 
  /* INITIALIZE */
 
  Common.errorno = 0;
  CLEAR(Common.errflg);
 
  G.entry = CONSTP;
 
  SETCOE_setup_indices();
 
  Setcoe.qsumcz = QEXTD(ZERO);
 
  /* LOOP OVER ALL OBSERVATIONS */
  HZRstr.nfncts++;
 
  /**
     IF THERE IS NO CONCOMMITANT INFORMATION, THE SCALING FACTORS FOR
     EACH INDIVIDUAL REMAIN THE SAME.
  **/
  if(Common.p==0)
    for(j=1; j<=3; j++)
      if(Common.phase[j]==1)
	G.lmu[j] = Common.theta[j+8-1];
 
  for(G.I=0; G.I<Common.Nobs; G.I++)
    if(!SETCOE_obs_loop())
      return;

  SETCOE_calc_scaling();
  return;
}
