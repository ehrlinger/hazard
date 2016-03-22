#include <string.h>
#include <math.h>

#include "common.h"
#include "hzd_ln_G1_and_SG1_m.h"
#include "hzd_ln_G3_and_SG3_m.h"

#include "hzr_set_error.h"
#include "hzr_cum_haz_calc.h"

/*static void SETDLL(void)*/
 
/***********************************************************************
 
   DERIVATIVE MATRICES FOR CONCOMITANT INFORMATION
 
   PURPOSE
     TO CALCULATE FIRST AND SECOND DERIVATIVES OF THE OBJECTIVE
     FUNCTION WITH RESPECT TO THE CONCOMITANT VARIABLES AND TO SET UP
     AND CALCULATE FIRST DERIVATIVES OF THE LIKELIHOOD FUNCTION FOR
     VARIABLES OUT OF THE MODEL.
 
     LET
       CF(T) = CUMMULATIVE HAZARD FUNCTION
 
       HF(T) = HAZARD FUNCTION (TIME DERIVATIVE OF CF)
                                  -CF(T)
       SF(T) = SURVIVAL FUNCTION E
 
       T     = TIME OF EVENT OR CENSORING (FOR INTERVAL CENSORED
               DATA, THE TIME BY WHICH THE EVENT SURELY OCCURRED)
 
       CT    = FOR INTERVAL CENSORED DATA, THE TIME AT WHICH INTERVAL
               CENSORING BEGINS
 
       ST    = FOR PIECEWISE OBSERVATIONS, THE STARTING TIME OF THE
               CURRENT PIECE (REPEATING EVENTS AND TIME-VARYING)
 
     THEN
 
                           _
                          |
                     NOBS |
     LOG LIKELIHOOD = SUM | C1(I)W(I)*LOGE(HF(T(I)))
                      I=1 |
                          |_
 
           _                               _   _                  _
          |                                 | |                    |
        - |C1(I)*W1(I) + C2(I) + C3(I)*W3(I)|*|CF(T(I)) - CF(ST(I))|
          |_                               _| |_                  _|
 
                                                           _
                                    _                    _  |
                                   |                      | |
                                   | CF(T(I)) - CF(CT(I)) | |
                 + C3(I)*W3(I)*LOGE|----------------------| |
                                   |     CT(I) - T(I)     | |
                                   |_                    _| |
                                                           _|
 
 
 
     WHERE
       W(I)  = WEIGHT OF EVENT AT TIME=T (POSITIVE VALUE)
 
       C1(I) = COUNT OF NUMBER OF EVENTS AT TIME=T
 
       C2(I) = COUNT OF CENSORED INDIVIDUALS AT TIME=T
 
       C3(I) = COUNT OF INTERVAL CENSORED INDIVIDUALS AT TIME=1 AND
               AN IDENTICAL CENSORING INTERVAL BEGINNING AT TIME=CT
                    _                             _
                3  |                               |
       CF(T) = SUM |MU(J;B(J),CONCOM)*G(T;THETA(J))|
               J=1 |_                             _|
 
                    _                              _
                3  |                                |
       HF(T) = SUM |MU(J;B(J),CONCOM)*SG(T;THETA(J))|
               J=1 |_                              _|
 
 
       CONCOM =  A VECTOR OF CONCOMMITENT INFORMATION OF LENGTH P
 
       B(J)   =  A PARAMETER VECTOR OF LENGTH P+1
 
       THETA(1) = VECTOR OF EARLY PHASE PARAMETERS:  DELTA,RHO,NU,M
 
       THETA(2) = 1
 
       THETA(3) = VECTOR LATE PHASE PARAMETERS:  TAU,GAMMA,ALPHA,ETA
 
       MU(J;B(J),CONCOM) = SCALING PARAMETRIC FUNCTION
 
       G(T;THETA(J)) = SHAPING FUNCTION FOR CUMULATIVE HAZARD
 
       SG(T;THETA(J)) = SHAPING FUNCTION FOR HAZARD FUNCTION
 
     NOTE:  SET SETG1 AND SETG3 FOR FORM OF SHAPING FUNCTION;
            G(2) = T AND SG(2) = 1.
 
     THE FORM OF MU(J;B(J),CONCOM) FOR EACH PHASE 1-3 IS LOG-LINEAR:
                               _                 _
                              |                   |
                              |NVAR               |
       MU(J;B(J),CONCOM) = EXP| SUM B(J,K)*X(J,K) |
                              | K=1               |
                              |_                 _|
 
     WHERE B(J,K) ARE THE REGRESSION COEFFICIENTS AND X(J,K) ARE THE
     CONCOMITANT VARIABLES.
 
     F I R S T   D E R I V A T I V E S
 
                      _
                     |           _
                     |          |
       D1LL     NOBS |          |
     --------- = SUM | X(L,I) * |C1(I)*W(I)*HF(L,T(I))/HF(T(I))
     D(B(L,I))   I=1 |          |
                     |          |_
                     |_
 
           - (C1(I)*W1(I)+C2(I)+C3(I)*W3(I))*(CF(L,T(I)) - CF(L,ST(I)))
 
                                                                 _
                                                               _  |
                                                                | |
                                    CF(L,T(I)) - CF(L,CT(I))    | |
                     + C3(I)*W3(I)*---------------------------  | |
                                      CF(T(I))-CF(CT(I))        | |
                                                               _| |
                                                                 _|
 
 
     S E C O N D   D E R I V A T I V E S
                             _
                            |
                            |
           D2LL        NOBS |
     ---------------- = SUM | X(L,I)*X(K,I) *
     D(B(L,I),B(K,I))   I=1 |
                            |
                            |_
 
      _
     |
     |                HF(L,T(I))*HF(K,T(I))
     | - C1(I)*W(I) * ---------------------
     |                    HF(T(I))**2
     |_
 
 
 
                                 _                      _
             C3(I)W(I)          |                        |
      - --------------------- * |CF(L,T(I)) - CF(L,CT(I))| *
         _                _ 2   |_                      _|
        |                  |
        |CF(T(I))-CF(CT(I))|
        |_                _|
 
                                          _
                   _                    _  |
                  |                      | |
                  |CF(K,T(I))-CF(K,CT(I))| |
                  |_                    _| |
                                          _|
 
 
     IF L=K, THEN SUM IN ADDITION:
 
 
                      HF(K,T(I))
       + C1(I)*W(I) * ----------
                       HF(T(I))
 
       _                               _   _                      _
      |                                 | |                        |
    - |C1(I)*W1(I) + C2(I) + C3(I)*W3(I)|*|CF(K,T(I)) - CF(K,ST(I))|
      |_                               _| |_                      _|
 
                                  _                      _
              C3(I)*W3(I)        |                        |
       + --------------------- * |CF(K,T(I)) - CF(K,CT(I))|
          _                _     |_                      _|
         |                  |
         |CF(T(I))-CF(CT(I))|
         |_                _|
 
 
 
     N O T E:
       SINCE THE OBJECTIVE FUNCTION IS MINUS THE LOG LIKELIHOOD
       FUNCTION, ALL SIGNS IN THE ABOVE FORMULATION ARE CHANGED.
 
 
   DESCRIPTION OF PARAMETERS
     INPUT
       OBS    - REAL*8  NOBS*(P+7)-VECTOR OF OBSERVATIONS
         OBS(1) - TIME   = FOLLOWUP INTERVAL
         OBS(2) - C1     = NUMBER OF UNCENSORED OBSERVATIONS AT T
         OBS(3) - C2     = NUMBER OF CENSORED OBSERVATIONS AT T
         OBS(4) - C3     = NUMBER OF INTERVAL CENSORED OBS. AT CTIME
         OBS(5) - CTIME  = INTERVAL CENSORED STARTING TIME (<T)
         OBS(6) - STIME  = STARTING TIME (<T) FOR REPEATING EVENT OR
                           TIME-VARYING COVARIABLE
         OBS(7) - WEIGHT = WEIGHT FOR THE EVENT (GE 0)
       THETA  - REAL*8  NVAR-VECTOR OF ALL MODEL PARAMETERS
       LNG1   - REAL*8  NOBS-VECTOR OF LOGE(G1) FIXED SHAPING FUNCTION
       LNG1CT - REAL*8  NOBS-VECTOR OF LOGE(G1) FIXED SHAPING FUNCTION
                        AT CT (START OF INTERVAL CENSORING)
       LNG1ST - REAL*8  NOBS-VECTOR OF LOGE(G1) FIXED SHAPING FUNCTION
                        AT ST (START OF INTERVAL)
       LNSG1  - REAL*8  NOBS-VECTOR OF LOGE(SG1) FIXED SHAPING FUNCTION
       LNG2   - REAL*8  NOBS-VECTOR OF LOGE(T)
       LNG2CT - REAL*8  NOBS-VECTOR OF LOGE(CT) FIXED SHAPING FUNCTION
                        (START OF INTERVAL CENSORING)
       LNG2ST - REAL*8  NOBS-VECTOR OF LOGE(CT) FIXED SHAPING FUNCTION
                        AT ST (START OF INTERVAL)
       LNG3   - REAL*8  NOBS-VECTOR OF LOGE(G3) FIXED SHAPING FUNCTION
       LNG3CT - REAL*8  NOBS-VECTOR OF LOGE(G3) FIXED SHAPING FUNCTION
                        AT CT (START OF INTERVAL CENSORING)
       LNG3ST - REAL*8  NOBS-VECTOR OF LOGE(G3) FIXED SHAPING FUNCTION
                        AT ST (START OF INTERVAL)
       LNSG3  - REAL*8  NOBS-VECTOR OF LOGE(SG3) FIXED SHAPING FUNCTION
       LNMU1  - REAL*8  NOBS-VECTOR OF LOGE(MU1)
       LNMU2  - REAL*8  NOBS-VECTOR OF LOGE(MU2)
       LNMU3  - REAL*8  NOBS-VECTOR OF LOGE(MU3)
 
     WORKING
       INDXX  - INTEGER NVAR-VECTOR FOR STORING INDICES FOR X
       INDXP  - INTEGER NVAR-VECTOR FOR STORING CORRESPONDING PHASES
       INDXT  - INTEGER NVAR-VECTOR FOR STORING CORRESPONDING THETAS
 
     OUTPUT
       D1LL1  - REAL*8  NOBS-VECTOR OF FIRST DERIVATIVE PIECES, PHASE 1
       D1LL2  - REAL*8  NOBS-VECTOR OF FIRST DERIVATIVE PIECES, PHASE 2
       D1LL3  - REAL*8  NOBS-VECTOR OF FIRST DERIVATIVE PIECES, PHASE 3
       D2LL11 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (1,1)
       D2LL12 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (1,2)
       D2LL13 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (1,3)
       D2LL22 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (2,2)
       D2LL23 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (2,3)
       D2LL33 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (3,3)
 
   DESCRIPTION OF PARAMETERS FOR ENTRY DAGRAD (FIRST DERIVATIVES)
     INPUT
       BETA   - REAL*8  NVAR-VECTOR OF ACTIVE PARAMETER ESTIMATES
       NVAR   - INTEGER DIMENTIONALITY OF DERIVATIVE ARRAY
 
     OUTPUT
       D1LL   - REAL*8  NVAR DERIVATIVE ARRAY
 
   DESCRIPTION OF PARAMETERS FOR ENTRY DAGRAD (SECOND DERIVATIVES)
     INPUT
       BETA   - REAL*8  ACTIVE PARAMETER ESTIMATES
       NVAR   - INTEGER DIMENTIONALITY OF DERIVATIVE ARRAY
 
     OUTPUT
       D2LL   - REAL*8  NVAR*NVAR DERIVATIVE ARRAY
 
   DESCRIPTION OF PARAMETERS FOR ENTRY D2LCON
     INPUT
       NONE
 
     OUTPUT
       D2LL   - REAL*8  NVAR*NVAR DERIVATIVE ARRAY
 
   DESCRIPTION OF PARAMETERS FOR ENTRY DLLADD
     INPUT
       INDX   - INTEGER INDEX LOCATION OF VARIABLE
       IPHAS  - INTEGER PHASE OF THE VARIABLE (1, 2, OR 3)
 
     OUTPUT
       D1LLAD - REAL*8  FIRST DERIVATIVE (GRADIENT)
       D2LLAD - REAL*8  NVAR+1 VECTOR OF SECOND DERIVATIVES
 
   DESCRIPTION OF OTHER PARAMETERS USED BY OR CREATED BY SUBROUTINE
     INPUT
       NOBS   - INTEGER NUMBER OF OBSERVATIONS IN DATA SET
       P      - INTEGER NUMBER OF UNIQUE CONCOMITANT VARIABLES
       PHASE  - INTEGER 3-VECTOR, =1 IF PHASE IS PRESENT
       FIXG1  - LOGICAL IF .TRUE. EARLY SHAPING FUNCTION IS FIXED
       FIXG3  - LOGICAL IF .TRUE. LATE SHAPING FUNCTION IS FIXED
       MODE   - INTEGER FLAG FOR CONSERVATION OF EVENTS IF ZERO
 
     OUTPUT
       ERROR  - INTEGER ERROR FLAG
                        0 - NO ERROR
                        1 - ARITHMETIC OVERFLOW (FATAL ERROR)
                        2 - PROCESSING ERROR (EG, MISMATCHES WITH
                            NVAR OR WITH PHASES)
       ERRFLG - CHARACTER*12 ERROR MESSAGE
 
   REMARKS
     1.  INITIALLY, SETDLL IS CALLED TO ESTABLISH THE VARIABLE
         DIMENSIONS.
 
     2.  DAGRAD IS USED TO OBTAIN ANALYTICAL GRADIENTS FOR THE
         OPTIMIZATION PROCESS.  IF CONSERVATION OF EVENTS IS
         USED, THEN ONLY NUMERICAL GRADIENTS ARE TO BE USED.
         ALSO, THE ROUTINE IS TO BE USED ONLY IF ALL SHAPING
         PARAMETERS ARE FIXED.  THIS ENTRY USES PRECALCULATED
         INFORMATION FROM THE COMPUTATIONS FOR THE LIKELIHOOD
         FUNCTION, AS THIS SAVES RECALCULATIONS IN THIS SUBPROGRAM.
 
     3.  DAHESS IS USED TO OBTAIN AN ANALYTICAL HESSIAN FOR THE
         OPTIMIZATION PROCESS. THE SAME LIMITATIONS ARE IN EFFECT
         AS FOR ENTRY DAGRAD, AND THE ENTRY USES PRECALCULATED
         INFORMATION SIMILARLY.
 
     4.  ONCE PARAMETER ESTIMATES FOR CONCOMITANT INFORMATION AND
         INTERCEPTS ARE AVAILABLE IN THETA, ENTRY D2CON MAY BE USED
         TO OBTAIN A SECOND DERIVATIVE MATRIX WITH RESPECT TO THE
         LOG LIKELIHOOD FUNCTION FOR USE IN CALCULATING THE VARIANCE-
         COVARIANCE MATRIX.
 
     5.  TO COMPUTE STATISTICS (SUCH AS Q-STATISTICS) FOR VARIABLES
         OUT OF THE MODEL, BOTH FIRST AND SECOND DERIVATIVES FOR
         THESE, GIVEN THE MODEL, ARE OBTAINED USING ENTRY DLLADD.
         HOWEVER, BECAUSE THE SUBROUTINES TO DO THIS ASSUME THAT A
         MINIMIZATION ALGORITHM HAS BEEN USED, THE SIGNS WILL BE
         INCORRECT IF WE USE THE LOG LIKELIHOOD FUNCTION.  THEREFORE
         WE WILL CREATE MINUS THE FIRST AND SECOND DERIVATIVES.
 
   SUBROUTINES REQUIRED
     DLGAOB - CALCULATE LOGE(A/B)
     DLG1   - CALCULATE LOGE(G1(T)), RETURNS LG1 AND LSG1
     DLG3   - CALCULATE LOGE(G3(T)), RETURNS LG3 AND LSG3
 
   METHOD
     THE ABOVE METHOD IS USED (HERE AND IN THE SUBPROGRAM WHICH
     CALCULATES THE LIKELIHOOD FUNCTION) TO FORM ANALYTICAL
     DERIVATIVES.  IN ADDITION WE PRESTORE INFORMATION THAT CAN BE
     USED IN THE CALCULATION OF Q-STATISTICS.
 
   IMPLEMENTATION
     E.H.BLACKSTONE, MD   JUNE, 1987
 
   REVISIONS
     JULY 1, 1987  - USE INDEX ARRAYS TO AVOID REDUNDANT CALCULATIONS
     JULY 1,1989   - ADD REPEATING EVENTS (LEFT CENSORING) LOGIC
     OCT 25,1991   - ADD WEIGHTED EVENTS
     OCT 25,1991   - ALTERED LOG-LIKELIHOOD TO NELSON-TYPE FORMULATION
     October 1993  - Converted to HZRCOR C-version 4.0(0)
     Mar 18,1994   - Corrected weights (MET)
 
   VERIFICATION
     INITIAL VERSION WAS VERIFIED MARCH 1, 1985 AGAINST REDUCE
     AND MARCH 3, 1985 AGAINST SAS PROGRAM AND DATA SET.
     THE PRESENT VERSION WAS TESTED AGAINST THE ORIGINAL ON
 
     OCTOBER/NOVEMBER 1991:  VERIFICATION OF DERIVATIVES FOR NEW
     FORMULATION OF INTERVAL CENSORING, LEFT CENSORING, AND NEW
     STRUCTURE FOR LOG LIKELIHOOD FUNCTION BY COMPARING WITH OUTPUT
     OF OLD PROGRAM.
 
***********************************************************************/
 

/* SETDLL entry point not required as of HZRCOR C-version 4.0(0) */

 
void DAGRAD(double *d1ll,int nvar)
     /* ENTRY TO CALCULATE THE GRADIENT OF THE OBJECTIVE FUNCTION */
{
  double xi;
  int i,ii,ip,ix;
 
  /* INITIALIZE DERIVATIVE ARRAY */
  for(i=0; i<nvar; i++)
    d1ll[i] = ZERO;
 
  /* FORM GRADIENT HERE PRESTORED INFORMATION */
  for(G.I=0; G.I<Common.Nobs; G.I++) {
    if(Common.phase[1]==1) G.d1llp[1] = HZRstr.d1ll1[G.I];
    if(Common.phase[2]==1) G.d1llp[2] = HZRstr.d1ll2[G.I];
    if(Common.phase[3]==1) G.d1llp[3] = HZRstr.d1ll3[G.I];
 
    G.Im1p7 = (G.I*HZRstr.pp7)-1;

    for(ii=0; ii<nvar; ii++) {
      ip = HZRstr.indxp[ii];
      ix = HZRstr.indxx[ii];
      /* GET X(I) */
      if(ix==0)
	xi = ONE;
      else
	xi = Common.obs[ix+G.Im1p7];
      /**
	 FORM DERIVATIVE (MINUS SIGN USED SINCE WE'RE INTERESTED IN
	 GRADIENT OF OBJECTIVE FUNCTION)
      **/
      d1ll[ii] -= xi*G.d1llp[ip];
    }
  }
}

void DAHESS(double *d2ll,int nvar)
     /* ENTRY TO CALCULATE THE HESSIAN OF THE OBJECTIVE FUNCTION */
{
  double xi,d2llij[4][4];
  /* double xj; */
  int i,ii,ip,ix,jj,jp,iijj,jjii,nvtnv,I,Im1p7,pp7,nvarii;
  int phase1,phase2,phase3;


  phase1=Common.phase[1]; 
  phase2=Common.phase[2]; 
  phase3=Common.phase[3]; 
  nvtnv = nvar*nvar;
  for(i=0; i<nvtnv; i++)
    d2ll[i] = ZERO;
  pp7=HZRstr.pp7; 
  /* FORM HESSIAN (SECOND DERIVATIVE) FROM PRESTORED INFORMATION */
  for(I=0; I<Common.Nobs; I++) {
      
    if(phase1==1) {
      d2llij[1][1] = HZRstr.d2ll11[I];
      if(phase2==1)
	d2llij[1][2] =
	  d2llij[2][1] = HZRstr.d2ll12[I];
      if(phase3==1)
	d2llij[1][3] =
	  d2llij[3][1] = HZRstr.d2ll13[I];
    }
    if(phase2==1) {
      d2llij[2][2] = HZRstr.d2ll22[I];
      if(phase3==1)
	d2llij[2][3] =
	  d2llij[3][2] = HZRstr.d2ll23[I];
    }
    if(phase3==1)
      d2llij[3][3] = HZRstr.d2ll33[I];
 
    Im1p7 = (I*pp7)-1;

    for(ii=0; ii<nvar; ii++) {
      ip = HZRstr.indxp[ii];
      ix = HZRstr.indxx[ii];
      nvarii=nvar*ii; 
      if(ix==0){
	/* GET X(J)'S */
	d2ll[ii+nvarii] -= d2llij[ip][ip];
	for(jj=ii+1; jj<nvar; jj++) {
	  iijj=jj+nvarii;
	  jjii = ii+nvar*jj;
	  jp = HZRstr.indxp[jj];
	  ix = HZRstr.indxx[jj];
	  if(ix==0){
	    d2ll[jjii]=
	      d2ll[iijj] -= d2llij[ip][jp];
	  }else{
	    d2ll[jjii]=
	      d2ll[iijj] -= Common.obs[ix+Im1p7]*d2llij[ip][jp];
	  }
	}
      }else{
	xi = Common.obs[ix+Im1p7];
	/* GET X(J)'S */
	d2ll[ii+nvarii] -= xi*xi*d2llij[ip][ip];
	for(jj=ii+1; jj<nvar; jj++) {
	  iijj=jj+nvarii;
	  jjii = ii+nvar*jj;
	  jp = HZRstr.indxp[jj];
	  ix = HZRstr.indxx[jj];
	  if(ix==0){
	    d2ll[jjii]=
	      d2ll[iijj] -= xi*d2llij[ip][jp];
	  }else{
	    d2ll[jjii]=
	      d2ll[iijj] -= xi*Common.obs[ix+Im1p7]*d2llij[ip][jp];
	  }
	}
      }
    }
  }
}

void d2lcon(double *d2ll)
     /**
   ENTRY D2LCON FOR CALCULATING SECOND DERIVATIVES OF THE LOG LIKELIHOOD
   FUNCTION, WITH SETUP FOR SUBSEQUENT USE IN BRINGING IN VARIABLES NOT
   PRESENTLY IN THE MODEL.
**/
{
  int i,j,ivtiv;
  double cf[4],lcf[4],hf[4],lhf[4],cfct[4],lcfct[4];
  double cfst[4],lcfst[4];
  double cumhaz,hazard,cumhct,dcf=0;
  /* double lct;*/
  double cumhst,dt;
  double lc1haz=0,lhazrd=0;
  double der1=0,der3=0;
  /* double llike;*/
  double time,c1,c2,c3,ctime,c1c2c3;
  double stime,c1w,c3w,weight;
  int k,l;
  int ip,it,ix,Im1p7;
  double lmu[4];
  int I;
  double lnG1,lnSG1;
  double lnG3,lnSG3;
  double xi;
  /* double xj; */
  int ii,jj,jp,iijj,jjii;
  double dcf2=0;
  double d2llij[4][4],d1llp[4];
  int nvar,nvarii; 

  /* build control flag to determine computed goto's "switch{ case: }" */
  /* INITIALIZE */
  Common.errorno = 0;
  CLEAR(Common.errflg);
 
  /* ZERO OUT SECOND DERIVATIVE ARRAY */
  nvar = HZRstr.ivar;
  ivtiv = nvar*nvar;
  for(i=0; i<ivtiv; i++)
    d2ll[i] = ZERO;
 
  /*-------------------------------------------------------------------
    LOOP TO PRECALCULATE THE CONSTANT SCALING FACTOR FOR DERIVATIVES,
    -------------------------------------------------------------------*/
 
  /* INCREMENT NUMBER OF FUNCTION EVALUATIONS COUNTER. */
  HZRstr.nfncts++;
 
  /**
     IF THERE IS NO CONCOMMITANT INFORMATION, THE SCALING FACTORS FOR
     EACH INDIVIDUAL REMAIN THE SAME.
  **/
  if(Common.p==0)
    for(j=1; j<=3; j++)
      if(Common.phase[j]==1)
	lmu[j] = Common.theta[j+7];
 
  for(I=0; I<Common.Nobs; I++) {
    /* Init variables */ 
    Im1p7 = (I*HZRstr.pp7)-1;
    time = Common.obs[Im1p7+1];
    cfst[1]= cfst[2] = cfst[3] = cf[1]= cf[2]= cf[3]= cumhaz=ZERO;
      
    if(HZRstr.mode<=0 ) {
      if(Common.phase[1]==1) {
	lcf[1] = HZRstr.lnmu1[I]+HZRstr.lng1[I];
	if(!hzr_cum_haz_calc_m(&cumhaz,&cf[1],lcf[1])) return ;
      }
      if(Common.phase[2]==1) {
	lcf[2] = HZRstr.lnmu2[I]+HZRstr.lng2[I];
	if(!hzr_cum_haz_calc_m(&cumhaz,&cf[2],lcf[2])) return ;
      }
      if(Common.phase[3]==1) {
	lcf[3] = HZRstr.lnmu3[I]+HZRstr.lng3[I];
	if(!hzr_cum_haz_calc_m(&cumhaz,&cf[3],lcf[3])) return ;
      }
    } else {
      /** CALCULATE LOGE(MU(J)), THE LOG OF THE PARAMETRIC SCALING FUNCTION **/
      if(Common.p>0) {
	for(k=0; k<nvar; k++) {
	  ip = HZRstr.indxp[k];
	  it = HZRstr.indxt[k];
	  ix = HZRstr.indxx[k];
	  /* INTERCEPT */
	  if(ix==0) lmu[ip] = Common.theta[it];
	  /* CONCOMITANT INFORMATION:  SUM (BETA(IP,IT)*X(I,IX)) */
	  else lmu[ip] += (Common.obs[ix+Im1p7]*Common.theta[it]);
	}
      }
      if(Common.phase[1]) {
	if(HZRstr.fixg1) {
	  lnG1 = HZRstr.lng1[I];
	  lnSG1 = HZRstr.lnsg1[I];
	} else {
	  hzd_ln_G1_and_SG1_m(time,&lnG1,&lnSG1);
	  if(Common.errorno) return ;
	  HZRstr.lng1[I] = lnG1;
	  HZRstr.lnsg1[I] = lnSG1;
	}
	lcf[1] = lmu[1]+lnG1;
	if(!hzr_cum_haz_calc_m(&cumhaz,&cf[1],lcf[1])) return ;
      }
      if(Common.phase[2]==1) {
	lcf[2] = lmu[2]+HZRstr.lng2[I];
	if(!hzr_cum_haz_calc_m(&cumhaz,&cf[2],lcf[2])) return ;
      }
      if(Common.phase[3]) {
	if(HZRstr.fixg3) {
	  lnG3 = HZRstr.lng3[I];
	  lnSG3 = HZRstr.lnsg3[I];
	} else {
	  hzd_ln_G3_and_SG3_m(time,&lnG3,&lnSG3);
	  if(Common.errorno) return ;
	  HZRstr.lng3[I] = lnG3;
	  HZRstr.lnsg3[I] = lnSG3;
	}
	lcf[3] = lmu[3]+lnG3;
	if(!hzr_cum_haz_calc_m(&cumhaz,&cf[3],lcf[3])) return ;
      } 
    }
 
    /* HAZARD FUNCTION (IF NEEDED) AND ITS ASSOCIATED WEIGHT */
    c1 = Common.obs[Im1p7+2];
    weight = Common.obs[Im1p7+7];
    c1w = c1*weight;
   
    if(c1>ZERO) {
      hazard=0; /* this is avoid spurious error D2LCON920 */
      if(HZRstr.mode<=0 ) {
	if(Common.phase[1]==1) {
	  lhf[1] = HZRstr.lnmu1[I]+HZRstr.lnsg1[I];
	  if(!hzr_cum_haz_calc_m(&hazard,&hf[1],lhf[1])) return ;
	}
	if(Common.phase[2]==1) {
	  lhf[2] = HZRstr.lnmu2[I];
	  if(!hzr_cum_haz_calc_m(&hazard,&hf[2],lhf[2])) return ;
	} 
	if(Common.phase[3]==1) {
	  lhf[3] = HZRstr.lnmu3[I]+HZRstr.lnsg3[I];
	  if(!hzr_cum_haz_calc_m(&hazard,&hf[3],lhf[3])) return ;
	}
      }else{
	if(Common.phase[1]) {
	  lhf[1] = lmu[1]+lnSG1;
	  if(!hzr_cum_haz_calc_m(&hazard,&hf[1],lhf[1])) return ;
	}
	if(Common.phase[2]==1) {
	  lhf[2] = lmu[2];
	  if(!hzr_cum_haz_calc_m(&hazard,&hf[2],lhf[2])) return ;
	}
	if(Common.phase[3]) {
	  lhf[3] = lmu[3]+lnSG3;
	  if(!hzr_cum_haz_calc_m(&hazard,&hf[3],lhf[3])) return ;
	}
      }
      if(hazard<=ZERO) { hzr_set_error("(D2LCON920)",2); return ; }
      lhazrd = LOGe(hazard);
      lc1haz = -TWO*lhazrd;
    }

 
    /* CALCULATE LOG CUMULATIVE HAZARD FUNCTION FOR STARTING TIME (STIME) */

    stime = Common.obs[Im1p7+6];
    if(stime>ZERO) {
      if(Common.phase[1]) {
	if(HZRstr.fixg1) {
	  lnG1 = HZRstr.lng1st[I];
	} else {
	  hzd_ln_G1_and_SG1_m(stime,&lnG1,&lnSG1);
	  if(Common.errorno) return ;
	  HZRstr.lng1st[I] = lnG1;
	}
	lcfst[1] = lmu[1]+lnG1;
	if(!hzr_cum_haz_calc_m(&cumhst,&cfst[1],lcfst[1])) return ;
      }

      if(Common.phase[2]==1) {
	lcfst[2] = lmu[2]+HZRstr.lng2st[I];
	if(!hzr_cum_haz_calc_m(&cumhst,&cfst[2],lcfst[2])) return ;
      }

      if(Common.phase[3]) {
	if(HZRstr.fixg3) {
	  lnG3 = HZRstr.lng3st[I];
	} else {
	  hzd_ln_G3_and_SG3_m(stime,&lnG3,&lnSG3);
	  if(Common.errorno) return ;
	  HZRstr.lng3st[I] = lnG3;
	}
	lcfst[3] = lmu[3]+lnG3;
	if(!hzr_cum_haz_calc_m(&cumhst,&cfst[3],lcfst[3])) return ;
      } 
    }
 
    /* CALCULATE INTERVAL CUMULATIVE HAZARD PIECE, IF NEEDED */
    c3 = Common.obs[Im1p7+4];
    c3w = c3*weight;
    cumhct = ZERO;
    if(c3>ZERO) {
      ctime = Common.obs[Im1p7+5];
      if(ctime>ZERO) {
	if(HZRstr.mode<=0) {
	  if(Common.phase[1]==1){
	    lcfct[1] = HZRstr.lnmu1[I]+HZRstr.lng1ct[I];
	    if(!hzr_cum_haz_calc_m(&cumhct,&cfct[1],lcfct[1])) return ;
	  } 
	  if(Common.phase[2]==1){
	    lcfct[2] = HZRstr.lnmu2[I]+HZRstr.lng2ct[I];
	    if(!hzr_cum_haz_calc_m(&cumhct,&cfct[2],lcfct[2])) return ;
	  }
	  if(Common.phase[3]==1){
	    lcfct[3] = HZRstr.lnmu3[I]+HZRstr.lng3ct[I];
	    if(!hzr_cum_haz_calc_m(&cumhct,&cfct[3],lcfct[3])) return ;
	  } 
	} else{ 
	  if(Common.phase[1]) {
	    if(HZRstr.fixg1) {
	      lnG1 = HZRstr.lng1ct[I];
	    } else {
	      hzd_ln_G1_and_SG1_m(ctime,&lnG1,&lnSG1);
	      if(Common.errorno) return ;
	      HZRstr.lng1ct[I] = lnG1;
	    }
	    lcfct[1] = lmu[1]+lnG1;
	    if(!hzr_cum_haz_calc_m(&cumhct,&cfct[1],lcfct[1])) return ;
	  }
	  if(Common.phase[2]==1){ 
	    lcfct[2] = lmu[2]+HZRstr.lng2ct[I];
	    if(!hzr_cum_haz_calc_m(&cumhct,&cfct[2],lcfct[2])) return ;
	  }
	  if(Common.phase[3]) {
	    if(HZRstr.fixg3) {
	      lnG3 = HZRstr.lng3ct[I];
	    } else {
	      hzd_ln_G3_and_SG3_m(ctime,&lnG3,&lnSG3);
	      if(Common.errorno) return ;
	      HZRstr.lng3ct[I] = lnG3;
	    }
	    lcfct[3] = lmu[3]+lnG3;
	    if(!hzr_cum_haz_calc_m(&cumhct,&cfct[3],lcfct[3])) return ;
	  }
	}
      }else{
	cumhct =  cfct[1] = cfct[2] = cfct[3] = ZERO;
      }
 
      /**
         CALCULATE NELSON-TYPE APPROXIMATION TO HAZARD FUNCTION BY DIFFERENCES
      **/
      dcf = cumhaz-cumhct;
      dcf2 = dcf*dcf;
      dt = time-ctime;
      if(dcf<=ZERO || dt<=ZERO) {
	hzr_set_error("(D2LCON)",2);
	return ;
      }
    }
 
    /* NOW FORM THE LOG LIKLIHOOD FUNCTION */
    c2 = Common.obs[Im1p7+3];
    c1c2c3 = c1w+c2+c3w;
      
    /**
       NOW CALCULATE THE SCALING FACTORS FOR EACH DERIVATIVE IF THESE
       ARE NEEDED **/
    for(k=1; k<=3; k++) {
      if(Common.phase[k]!=1)
	continue;
      d1llp[k] = -c1c2c3*(cf[k]-cfst[k]);
      d2llij[k][k] = - c1c2c3*(cf[k]-cfst[k]);
      if(c1>ZERO) {
	d1llp[k] += c1w*ePOW(lhf[k]-lhazrd);
	der1 = c1w*ePOW(lhf[k]+lc1haz);
	d2llij[k][k] += der1*(hazard-hf[k]);
      }
 
      if(c3>ZERO) {
	der3 = c3w*((cf[k]-cfct[k])/(dcf2));
	d1llp[k] += der3 * dcf;
	d2llij[k][k] += (der3*(dcf+(cfct[k]-cf[k])));
      }
 
      /* FOR SECOND DERIVATIVES, WE MUST FORM CROSS-PRODUCT TERMS */
      for(l=k+1; l<=3; l++)
	if(Common.phase[l]==1) {
	  d2llij[k][l] = ZERO;
	  if(c1>ZERO)
	    d2llij[k][l] = -der1*hf[l];
	  if(c3>ZERO)
	    d2llij[k][l] += der3*(cfct[l]-cf[l]);
	}
    }
 
    /* IF DOING FIRST DERIVATIVES, SAVE PIECES. */
    /**
       NOW SAVE THE APPROPRIATE SCALING FACTORS FOR THE DERIVATIVES FOR
       USE IN ENTRY DLLADD.
    **/

    if(Common.phase[1]==1) {
      HZRstr.d1ll1[I] = d1llp[1];
      HZRstr.d2ll11[I] = d2llij[1][1];
      if(Common.phase[2]==1) HZRstr.d2ll12[I] = d2llij[1][2];
      if(Common.phase[3]==1) HZRstr.d2ll13[I] = d2llij[1][3];
    }
    if(Common.phase[2]==1) {
      HZRstr.d1ll2[I] = d1llp[2];
      HZRstr.d2ll22[I] = d2llij[2][2];
      if(Common.phase[3]==1) HZRstr.d2ll23[I] = d2llij[2][3];
    }
    if(Common.phase[3]==1){
      HZRstr.d1ll3[I] = d1llp[3];
      HZRstr.d2ll33[I] = d2llij[3][3];
    }

    /*    SETDLL_form_hessian(d2ll,HZRstr.ivar);   */
    for(ii=0; ii<nvar; ii++) {
      ip = HZRstr.indxp[ii];
      ix = HZRstr.indxx[ii];
      nvarii=nvar*ii;
      if(ix==0){
	/* GET X(J)'S */
	d2ll[ii+nvarii] -= d2llij[ip][ip];
	for(jj=ii+1; jj<nvar; jj++) {
	  iijj=jj+nvarii;
	  jjii = ii+nvar*jj;
	  jp = HZRstr.indxp[jj];
	  ix = HZRstr.indxx[jj];
	  if(ix==0){
	    d2ll[jjii]=
	      d2ll[iijj] -= d2llij[ip][jp];
	  }else{
	    d2ll[jjii]=
	      d2ll[iijj] -= Common.obs[ix+Im1p7]*d2llij[ip][jp];
	  }
	}
      }else{
	xi = Common.obs[ix+Im1p7];
	/* GET X(J)'S */
	d2ll[ii+nvarii] -= xi*xi*d2llij[ip][ip];
	for(jj=ii+1; jj<nvar; jj++) {
	  iijj=jj+nvarii;
	  jjii = ii+nvar*jj;
	  jp = HZRstr.indxp[jj];
	  ix = HZRstr.indxx[jj];
	  if(ix==0){
	    d2ll[jjii]=
	      d2ll[iijj] -= xi*d2llij[ip][jp];
	  }else{
	    d2ll[jjii]=
	      d2ll[iijj] -= xi*Common.obs[ix+Im1p7]*d2llij[ip][jp];
	  }
	}
      }
    }
  }
}

void DLLADD(int indx,int iphas,double *d1llad,double *d2llad)
     /**
   CALCULATE DERIVATIVES FOR VARIABLES OUTSIDE THE MAXIMUM LIKELIHOOD
   MODEL
**/
{
  double xi,xj,d2llij[4];
  /* double d1llp;*/
  int I,i,ix,ivarp1,jj,jp,Im1p7,pp7;
 
  /* INITIALIZE */
  if(iphas<1 || iphas>3) {
    hzr_set_error("(SETDLL960)",2);
    return;
  }
  ivarp1 = HZRstr.ivar;
  pp7 = HZRstr.pp7;
  *d1llad = ZERO;
  for(i=0; i<=ivarp1; i++)
    d2llad[i] = ZERO;
 
  /* CALCULATION LOOP OVER ALL OBSERVATIONS */
  for(I=0; I<Common.Nobs; I++) {
 
    /* GET THE VARIABLE OF INTEREST */
 
    Im1p7 = (I*pp7)-1;
    xi = Common.obs[indx+Im1p7];
 
    if (iphas == 1) {
      /* CALCULATE FIRST DERIVATIVE */
      *d1llad -= xi*HZRstr.d1ll1[I];
      /* FILL TEMPORARY ARRAY OF SCALING FACTORS FOR DERIVATIVES */
      d2llij[1]=HZRstr.d2ll11[I];
      d2llij[2]=HZRstr.d2ll12[I];
      d2llij[3]=HZRstr.d2ll13[I];
      /**
	 DIAGONAL ELEMENT OF THE SECOND DERIVATIVE VECTOR (FOR THE
	 VARIABLE OF INTEREST WITH ITSELF)
      **/
      d2llad[ivarp1] -= xi*xi*d2llij[1];
    }else if (iphas ==2){
      /* CALCULATE FIRST DERIVATIVE */
      *d1llad -= xi*HZRstr.d1ll2[I];
      /* FILL TEMPORARY ARRAY OF SCALING FACTORS FOR DERIVATIVES */
      d2llij[1]=HZRstr.d2ll12[I];
      d2llij[2]=HZRstr.d2ll22[I];
      d2llij[3]=HZRstr.d2ll23[I];
      /**
	 DIAGONAL ELEMENT OF THE SECOND DERIVATIVE VECTOR (FOR THE
	 VARIABLE OF INTEREST WITH ITSELF)
      **/
      d2llad[ivarp1] -= xi*xi*d2llij[2];
    }else if (iphas ==3){
      /* CALCULATE FIRST DERIVATIVE */
      *d1llad -= xi*HZRstr.d1ll3[I];
      /* FILL TEMPORARY ARRAY OF SCALING FACTORS FOR DERIVATIVES */
      d2llij[1]=HZRstr.d2ll13[I];
      d2llij[2]=HZRstr.d2ll23[I];
      d2llij[3]=HZRstr.d2ll33[I];
      /**
	 DIAGONAL ELEMENT OF THE SECOND DERIVATIVE VECTOR (FOR THE
	 VARIABLE OF INTEREST WITH ITSELF)
      **/
      d2llad[ivarp1] -= xi*xi*d2llij[3];
    } 
    /**
       NOW GET DERIVATIVES OF THE VARIABLE WITH RESPECT TO ALL THE
       VARIABLES IN THE MODEL
    **/

    for(jj=0; jj<ivarp1; jj++) {
      jp = HZRstr.indxp[jj];
      ix = HZRstr.indxx[jj];

      if(ix==0)
	d2llad[jj] -= xi*d2llij[jp];
      else{
	xj = Common.obs[ix+Im1p7];
	d2llad[jj] -= xi*xj*d2llij[jp];
      }

    }
  }
}
 
/* 
   static void SETDLL_fill_d2llij(void){
   if(Common.phase[1]==1) {
   G.d2llij[1][1] = HZRstr.d2ll11[G.I];
   if(Common.phase[2]==1)
   G.d2llij[1][2] =
   G.d2llij[2][1] = HZRstr.d2ll12[G.I];
   if(Common.phase[3]==1)
   G.d2llij[1][3] =
   G.d2llij[3][1] = HZRstr.d2ll13[G.I];
   }
   if(Common.phase[2]==1) {
   G.d2llij[2][2] = HZRstr.d2ll22[G.I];
   if(Common.phase[3]==1)
   G.d2llij[2][3] =
   G.d2llij[3][2] = HZRstr.d2ll23[G.I];
   }
   if(Common.phase[3]==1)
   G.d2llij[3][3] = HZRstr.d2ll33[G.I];
   }
*/
/* 
   static void SETDLL_form_hessian(double *d2ll,int nvar){
   double xi,xj;
   int ii,ip,ix,jj,jp,iijj,jjii;
 
   for(ii=0; ii<nvar; ii++) {
   ip = HZRstr.indxp[ii];
   ix = HZRstr.indxx[ii];

   / * GET AN X(I) * /
   if(ix==0)
   xi = ONE;
   else
   xi = Common.obs[ix+G.Im1p7];

   / * GET X(J)'S * /
   for(jj=ii; jj<nvar; jj++) {
   iijj = jj+nvar*ii;
   jjii = ii+nvar*jj;
   if(jj==ii)
   d2ll[iijj] -= xi*xi*G.d2llij[ip][ip];
   else {
   jp = HZRstr.indxp[jj];
   ix = HZRstr.indxx[jj];
   if(ix==0)
   xj = ONE;
   else
   xj = Common.obs[ix+G.Im1p7];
   d2ll[iijj] =
   d2ll[jjii] -= xi*xj*G.d2llij[ip][jp];
   }
   }
   }
   }
*/
