#include <common.h>
#include <string.h>

#include <hzr_qextd.h>
#include "setcoe_obs_loop.h"
#include <hzr_qtod.h>
#include <hzr_set_error.h>
#include <hzd_ln_A_div_B.h>

#include "setcoe.h"
void consrv(void)
     /*-------------------------------------------------------------
 
       ENTRY TO CALCULATE THE EXPLICITLY DETERMINED SCALING FACTOR
       DURING ITERATIVE OPTIMIZATION OF ADDITIONAL PARAMETERS.
       
       THIS ENTRY SHOULD BE CALLED PRIOR TO CALCULATION OF THE 
       OBJECTIVE FUNCTION.  THUS, TEMPORARY STORAGE OF INTERMEDIATE
       CALCULATIONSOF USE IN EVALUATING THE OBJECTIVE FUNCTION 
	IS PROVIDED.
       ------------------------------------------------------------*/
{
  double sumchz,sumchj,lfactr;
  double devent,jevent;
  long indexp,j;
  /*  long interc; */

  /* INITIALIZE */
  Common.errorno = 0;
  CLEAR(Common.errflg);
  Setcoe.qsumcz = QEXTD(ZERO);
  Setcoe.qsumcj = QEXTD(ZERO);
 
  G.entry = CONSRV;
 
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
 
  /**
     NOW CALCULATE THE SCALING FACTOR FOR THE ENTIRE MODEL WHICH WILL
     CONSERVE EVENTS.
  **/
  sumchz = QTOD(Setcoe.qsumcz);
  sumchj = QTOD(Setcoe.qsumcj);
 
  /* CALCULATE THE DIFFERENCE BETWEEN OBSERVED AND PREDICTED EVENTS */
  devent = Setcoe.sc1pc3-sumchz;
 
  /**
     ADD THIS DIFFERENCE TO THE NUMBER OF DEATHS WHICH MUST BE 
     ABSORBED BY THE SCALE PARAMETER WE ARE TO CALCULATE EXPLICITLY
  **/
  jevent = sumchj+devent;
  if(jevent<=ZERO) {
    hzr_set_error("(SETCOE1200)",2);
    return;
  }
 
  /**
     CALCULATE THE FACTOR BY WHICH THE PRESENT SCALE FACTOR MUST BE
     CHANGED TO CONSERVE EVENTS
  **/
  lfactr = hzd_ln_A_div_B(jevent,sumchj);
  if(Common.errorno) {
    hzr_set_error("(SETCOE1210)",Common.errorno);
    return;
  }
  HZRstr.factor = ePOW(lfactr);
 
  /* NOW UPDATE THE SCALE PARAMETER */
  if(HZRstr.mode==0) {
    indexp = HZRstr.pj1+HZRstr.pp1*HZRstr.fixmu-1;
    Common.theta[indexp] += lfactr;
  }
}
 
