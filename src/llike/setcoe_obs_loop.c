#include <common.h>
#include <hzr_set_error.h>
#include <hzr_calc_loge_mu.h>
#include <hzr_cum_haz_func.h>
#include <hzr_cum_haz_calc.h>
#include <hzr_qextd.h>
#include "setcoe.h"

logical SETCOE_obs_loop(void){
  double c1,c2,c3,c1c2c3;
  double weight,c1w,stime;
  double cf[4],lcf[4],lcfct[4];
  double cfst[4],lcfst[4];
  double time,ctime;
  double cumhaz,sumchz,sumchj;
  /* double cftj; */
  double cumhst;
  long j;
  /* long  k; */
 
  G.Im1p7 = (G.I*HZRstr.pp7)-1;
 
  /**
     _                       _
     NOBS|                         |
     I=1 |_                       _|
  **/
 
  weight = Common.obs[G.Im1p7+7];
  c1 = Common.obs[G.Im1p7+2];
  c1w = c1*weight;
  c2 = Common.obs[G.Im1p7+3];
  c3 = Common.obs[G.Im1p7+4];
  Setcoe.c3w = c3*weight;
  stime = Common.obs[G.Im1p7+6];
  if(G.entry==SETCOE) {
    if(c1<ZERO || c2<ZERO || c3<ZERO || weight<ZERO) {
      hzr_set_error("SETCOE920",2);
      return FALSE;
    }
    Setcoe.sc1pc3 += (c1w+Setcoe.c3w);
  }
  c1c2c3 = c1w+c2+Setcoe.c3w;
 
  hzr_calc_loge_mu();
 
  /**
     CALCULATE LOG CUMULATIVE HAZARD
 
     NOTE:  IF SHAPING FUNCTIONS ARE TO BE PRESTORED (AS THEY ALWAYS
     WILL BE FOR PHASE 2), THEN STORE THE LOGARITHM OF THE
     CUMULATIVE HAZARD FUNCTION AND THE HAZARD FUNCTION.
  **/
 
  time = Common.obs[G.Im1p7+1];
  if(time<=ZERO) {
    hzr_set_error("(SETCOE930)",2);
    return FALSE;
  }

  if(!hzr_cum_haz_func(time,lcf,&HZRstr.lng1,NULL,&HZRstr.lnsg1,TRUE))
    return FALSE;
 
  /**
     NOW CALCULATE THE CUMULATIVE HAZARD PIECES INDIVIDUALLY FOR EACH
     PHASE.
 
     IN CONVERTING LOGE CUMULATIVE HAZARD, TAKE CARE TO AVOID MACHINE
     MAXIMUMS.
  **/

  if(!hzr_cum_haz_calc(&cumhaz,cf,lcf))
    return FALSE;
 
  /**
     FOR SETCOE, CALCULATE THE PIECE NEEDED FOR CONSERVATION OF EVENTS
     WITHIN EACH PHASE.
     _                                                 _
     NOBS|                                                   |
     SUMCF(J) = SUM |(C1(I)*W1(I) + C2(I) + C3(I)*W3(I))*(CFJ(T)-CFT(ST)|
     I=1 |_                                                 _|
  **/
  if(G.entry==SETCOE) {
    for(j=1; j<=3; j++)
      Setcoe.sumcf[j] += c1c2c3*cf[j];
  }
 
  /* CALCULATE INTERVAL CUMULATIVE HAZARD FUNCTION FOR STARTING TIME */
 
  cumhst = ZERO;
  cfst[HZRstr.fixmu] = ZERO;
  if(stime>ZERO) {
    if(stime>=time) {
      hzr_set_error("(SETCOE960)",2);
      return FALSE;
    }
    if(!hzr_cum_haz_func(stime,lcfst,&HZRstr.lng1st,NULL,NULL,FALSE))
      return FALSE;
    if(!hzr_cum_haz_calc(&cumhst,cfst,lcfst))
      return FALSE;
    for(j=1; j<=3; j++)
      Setcoe.sumcf[j] -= c1c2c3*cfst[j];
  }
 
  /**
     PUT TOGETHER THE PIECES.
     _                                                  _
     |                                                    |
     SUMCHZ = SUM |(C1(I)*W1(I) + C2(I) + C3(I)*W3(I))*(CF(T) - CF(ST))|
     I=1 |_                                                  _|
  **/
  sumchz = c1c2c3*(cumhaz-cumhst);
  Setcoe.qsumcz = QSUM(Setcoe.qsumcz,QEXTD(sumchz));
  if(G.entry==CONSRV) {
    sumchj = c1c2c3*(cf[HZRstr.fixmu]-cfst[HZRstr.fixmu]);
    Setcoe.qsumcj = QSUM(Setcoe.qsumcj,QEXTD(sumchj));
  }
 
  /**
     If there is no C3, go ahead and finish up now, rather than wade
     through IF/ENDIF.  This originally also checked for being called
     from SETCOE, but this test was deleted for 4.0.18 after discussions
     with EHB regarding his changes to Retrofit SETCOE.
  **/
  if(c3==ZERO)
    return TRUE;
 
  /* GET CUMULATIVE HAZARD FUNCTION FOR THE EARLIER TIME POINT (CT) */
  ctime = Common.obs[G.Im1p7+5];
  if(ctime==ZERO)
    return TRUE;
  else if(ctime<ZERO || ctime>=time) {
    hzr_set_error("(SETCOE1000)",2);
    return FALSE;
  }
 
  if(!hzr_cum_haz_func(ctime,lcfct,&HZRstr.lng1ct,NULL,NULL,FALSE))
    return FALSE;
  else
    return TRUE;
}
 
