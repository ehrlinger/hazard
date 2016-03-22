#include <string.h>
#include <math.h>

#include <common.h>

#include <consrv.h>

#include "setlik.h"
#include "setcoe.h"
#include <hzr_set_error.h>
#include <hzr_qextd.h>
#include <hzr_qtod.h>
#include <hzr_cum_haz_calc.h>
#include <hzr_qsum.h>
#include <hzd_theta_to_parms.h>
#include <dlgaob.h>

#define SETOBJ_DEFS
#include "setobj.h"

void DCEOBJ_main_logic(double *fx);
logical DCEOBJ_obs_loop(void);
/*static void SETOBJ(void)*/

/***********************************************************************

   OBJECTIVE FUNCTION EVALUATION

   PURPOSE
     TO EVALUATE THE OBJECTIVE FUNCTION FOR THE OPTIMIZATION ROUTINE.
     THE OBJECTIVE FUNCTION HAS THE FORM:

       FX = -LNLIKE

     WHERE LNLIKE IS THE LOG LIKELIHOOD FUNCTION.

     THE LOG LIKELIHOOD FUNCTION HAS THE FORM:

     LET
       CF(T) = CUMMULATIVE HAZARD FUNCTION

       HF(T) = HAZARD FUNCTION (TIME DERIVATIVE OF CF)
                                  -CF(T)
       SF(T) = SURVIVAL FUNCTION E

       T     = TIME OF EVENT OR CENSORING (FOR INTERVAL CENSORED
               DATA, THE TIME BY WHICH THE EVENT SURLY OCCURRED)

       CT    = FOR INTERVAL CENSORED DATA, THE TIME AT WHICH INTERVAL
               CENSORING BEGINS

       ST    = FOR PIECEWISE OBSERVATIONS, THE STARTING TIME OF THE
               CURRENT PIECE (REPEATING EVENTS AND TIME-VARYING 
               COVARIABLES)

     THEN
                     _
                    |             _             _
              NDATA |            |               |
       LLIKE = SUM  | C1(I)*W(I)*|LOGE(HF(T(I))) |
               I=1  |            |_             _|
                    |_


                     _                   _   _                  _
                    |                     | |                    |
                  - |C1(I) + C2(I) + C3(I)|*|CF(T(I)) - CF(ST(I))|
                    |_                   _| |_                  _|

                                                              _
                                    _                       _  |
                                   |                         | |
                                   | CF(T(I)) - CF(CT(I))    | |
                  + C3(I)*W(I)*LOGE|E                    - 1 | |
                                   |_                       _| |
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

   DESCRIPTION OF PARAMETERS FOR INITIALIZATION
     INPUT
       OBS    - REAL*8  NOBS*P-VECTOR OF OBSERVATIONS
       THETA  - REAL*8  NTHETA-VECTOR OF ALL MODEL PARAMETERS
       INDEX  - INTEGER NTHETA-VECTOR POINTER TO ACTIVE PARAMETER
                        LOCATIONS IN THETA
       LNG1   - REAL*8  NOBS-VECTOR OF LOGE(G1) FIXED SHAPING FUNCTION
       LNG1CT - REAL*8  NOBS-VECTOR OF LOGE(G1) FIXED SHAPING FUNCTION
                        AT CT (START OF INTERVAL CENSORING)
       LNG1ST - REAL*8  NOBS-VECTOR OF LOGE(G1) FIXED SHAPING FUNCTION
                        AT ST (START OF INTERVAL)
       LNSG1  - REAL*8  NOBS-VECTOR OF LOGE(SG1) FIXED SHAPING FUNCTION
       LNG2   - REAL*8  NOBS-VECTOR OF LOGE(T)
       LNG2CT - REAL*8  NOBS-VECTOR OF LOGE(CT) FIXED SHAPING FUNCTION
                        (START OF INTERVAL CENSORING)
       LNG2ST - REAL*8  NOBS-VECTOR OF LOGE(ST) FIXED SHAPING FUNCTION
                        (START OF INTERVAL)
       LNG3   - REAL*8  NOBS-VECTOR OF LOGE(G3) FIXED SHAPING FUNCTION
       LNG3CT - REAL*8  NOBS-VECTOR OF LOGE(G3) FIXED SHAPING FUNCTION
                        AT CT (START OF INTERVAL CENSORING)
       LNG3ST - REAL*8  NOBS-VECTOR OF LOGE(G3) FIXED SHAPING FUNCTION
                        AT ST (START OF INTERVAL)
       LNSG3  - REAL*8  NOBS-VECTOR OF LOGE(SG3) FIXED SHAPING FUNCTION
       LNMU1  - REAL*8  NOBS-VECTOR OF LOGE(MU1) FOR PHASE 1
       LNMU2  - REAL*8  NOBS-VECTOR OF LOGE(MU2) FOR PHASE 2
       LNMU3  - REAL*8  NOBS-VECTOR OF LOGE(MU3) FOR PHASE 3

     OUTPUT
       NONE

   DESCRIPTION OF PARAMETERS FOR COMPUTATIONAL ENTRY DHAZRD
     INPUT
       BETA   - REAL*8  NVAR-VECTOR OF CURRENT PARAMETER ESTIMATES
                        OF THE ACTIVE PARAMETERS
       NVAR   - INTEGER NUMBER OF ACTIVE PARAMETERS

     OUTPUT
       FX     - REAL*8  VALUE OF THE OBJECTIVE FUNCTION AT BETA

   DESCRIPTION OF PARAMETERS FOR COMPUTATIONAL ENTRY DCEOBJ
     INPUT
       NONE

     OUTPUT
       FX     - REAL*8  VALUE OF THE OBJECTIVE FUNCTION AT BETA

   DESCRIPTION OF PARAMETERS FROM COMMON BLOCKS
     INPUT
       NOBS   - INTEGER NUMBER OF OBSERVATIONS IN DATA SET
       P      - INTEGER NUMBER OF UNIQUE CONCOMITANT VARIABLES
       PHASE  - INTEGER 3-VECTOR, =1 IF PHASE IS PRESENT
       FACTOR - REAL*8  VALUE FROM CONSERVATION OF EVENTS
       FIXMU  - INTEGER PHASE NUMBER FOR APPLYING FACTOR
       FIXG1  - LOGICAL ALL SHAPING PARAMETERS FIXED IN EARLY PHASE
       FIXG3  - LOGICAL ALL SHAPING PARAMETERS FIXED IN LATE PHASE

     OUTPUT
       NFNCTS - INTEGER NUMBER OF FUNCTION EVALUATIONS PERFORMED
       ERROR  - INTEGER ERROR FLAG
                        -1 - UNDERFLOW (MANAGED BY THE SYSTEM)
                         0 - NO ERROR DETECTED
                         1 - ARITHMETIC OVERFLOW (FATAL ERROR)
                         2 - VARIABLE OUTSIDE LEGAL RANGE (EG. SIGN)
       ERRFLG - CHARACTER*12 ERROR MESSAGE

   REMARKS
     1.  PRIOR TO THE FIRST CALL TO DHAZRD, AN INITIALIZATION CALL
         TO SETOBJ MUST BE MADE.  THE PURPOSE OF THIS CALL IS TO
         ESTABLISH THE VARIABLE DIMENSION ARRAYS USED IN THE
         PROCESSING.  THIS SHOULD HAVE TO BE DONE ONLY ONCE FOR
         A GIVEN PROBLEM, SINCE THE ARRAYS RELATE TO THE SIZE OF
         THE DATA SET BEING PROCESSED AND NOT AT ALL TO THE
         DIMENSIONALITY OF THE OPTIMIZATION BEING PERFORMED.

     2.  FOR CALCULATION PURPOSES THE MODEL PARAMETERS ARE TRANSFORMED
         TO A NEW SET OF ESTIMATION PARAMETERS.  THE FORM OF THESE
         NEW PARAMETERS IS GIVEN IN SUBROUTINE DTRSFM.

     3.  SUBROUTINE MUST NOT ALTER THE VALUES OF BETA.

     4.  THE FIRST FIVE (7) OBSERVATIONS IN VECTOR OBS ARE:
         OBS(1) - TIME   = FOLLOWUP INTERVAL
         OBS(2) - C1     = NUMBER OF UNCENSORED OBSERVATIONS AT T
         OBS(3) - C2     = NUMBER OF CENSORED OBSERVATIONS AT T
         OBS(4) - C3     = NUMBER OF INTERVAL CENSORED OBS. AT CTIME
         OBS(5) - CTIME  = INTERVAL CENSORED STARTING TIME (<T)
         OBS(6) - STIME  = STARTING TIME (<T) FOR REPEATING EVENT OR
                           TIME-VARYING COVARIABLE
         OBS(7) - WEIGHT = WEIGHT FOR THE EVENT (GE 0)

         NOTE:  THE ROUTINE ASSUMES THAT PRIOR TO CALLING DHAZRD
                THE VALUES FOR TIME, C1, C2, C3, CTIME, STIME AND
                WEIGHT HAVE BEEN CHECKED AND THEY ARE LEGITIMATE:
                TIME>0, 0 =< CTIME < TIME, C1 >= 0, C2 >= 0, C3 >= 0;
                STIME < TIME; WEIGHT >=0.  THESE WILL NOT BE RECHECKED.

     5.  CERTAIN CALLS ARE MADE TO THE CONSERVATION OF EVENTS ROUTINE
         FOR CALCULATION OF THE SCALE PARAMETER SOLVED FOR
         EXPLICITLY.  THIS MUST BE DONE PRIOR TO CALCULATION OF THE
         VALUE OF THE OBJECTIVE FUNCTION SINCE WE ARE OPTIMIZING
         WITH RESPECT TO THAT FUNCTION, NOT SIMPLY THE LOG LIKELIHOOD
         FUNCTION.  THUS, IF NUMERICAL DERIVATIVES ARE BEING TAKEN
         AND CONSERVATION OF EVENTS IS CALLED FOR, THEN WE MUST
         ALWAYS UPDATE THE SCALING PARAMETER DURING THE PROCESS.

     6.  IF CONSERVATION OF EVENTS IS INVOKED, IT MAY BE DESIRABLE
         TO OBTAIN AN INITIAL ESTIMATE OF THE MAGNITUDE OF THE
         OBJECTIVE FUNCTION.  A CALL TO ENTRY DCEOBJ WILL GIVE
         THE INITIAL VALUE OF THE OBJECTIVE FUNCTION WITH MINIMAL
         RECALCULATION SINCE IT USES VALUES SET IN SETCOE OR
         CONSTP.

   SUBROUTINES REQUIRED
     DLGAOD - LOGE(A/B)
     DTRSFM - INTERCONVERT ESTIMATION AND MODEL PARAMETERS
     CONSRV - CALCULATE SCALE PARAMETER EXPLICITLY
     DLLIKE - CALCULATE LIKELIHOOD FUNCTION WITHOUT CONSERVATION
              OF EVENTS

   METHOD
     SINCE THE OPTIMIZATION ROUTINE ATTEMPTS TO MINIMIZE THE OBJECTIVE
     FUNCTION, FX IS EXPRESSED AS MINUS THE LOG LIKELIHOOD FUNCTION.

     WHEREEVER NECESSARY, ROUTINES ARE CALLED WHICH IMPROVE THE
     ACCURACY OF THE COMPUTATIONS.  IN ADDITION, THE FINAL SUMMATION
     TO FORM THE LOG LIKELIHOOD FUNCTION IS PERFORMED IN XTENDED
     PRECISION.

     NOTE THAT IN THE PARTICULAR FORMULATION OF THE LOG LIKELIHOOD
     FUNCTION GIVEN ABOVE, PARTIAL OBSERVATIONS MAY BE INCORPORATED
     BY SEGMENTATION.  SPECIFICALLY, AN INDIVIDUAL'S FOLLOWUP CAN BE
     SEGMENTED INTO PORTIONS EXTENDING FROM 0 TO ST AND A PORTION
     FROM ST TO T.  THE IMPORTANCE OF THIS OBSERVATION IS THAT IF THE
     EVENT IS A NONTERMINATING (REPEATING) ONE, (ST - T) REPRESENTS
     THE INTERVAL BETWEEN EVENTS OF A NONHOMOGENEOUS POISSON
     PROCESS.  OR IF TIME VARYING COVARIABLES ARE INTRODUCED THAT
     MERELY SHIFT THE HAZARD FUNCTION UP OR DOWN FROM ITS CURRENT
     POSITION (A HIGHLY RESTRICTIVE ASSUMPTION, BUT QUITE ANALOGOUS
     TO THE COX MODEL), THEN (ST - T) REPRESENTS AN INTERVAL OVER
     WHICH THE TIME-VARYING COVARIABLE IS (PIECEWISE) UNCHANGING.

     YET A FURTHER CONSIDERATION IS THE WEIGHTED CASE WHEREBY THE
     EVENT TAKES ON A VALUE (POSITIVE, BUT POSSIBLY CONTINUOUS)
     OTHER THAN 0 OR 1.  "HAZARD" PIECES REQUIRE WEIGHTING.  THIS
     CAN BE SHOWN TO CONSERVE EVENT, AND IN THE CONSTANT HAZARD
     CASE, FOR EXAMPLE, THE MLE IS THE SUM OF ALL WEIGHTS DIVIDED BY
     THE SUM OF ALL TIME.

   IMPLEMENTATION
     E.H. BLACKSTONE, MD     OCTOBER, 1985
     FORTRAN77 TRANSLATION IN PART OF SUBROUTINE DHAZRD BY
     C.R. KATHOLI, PH.D.

   REVISIONS
     MARCH 17,1986 - USE XTENDED ARITHMATIC FOR MAIN SUMMATION
     MARCH 26,1986 - INITIALIZATION BY SETOBJ IMPLEMENTED
     APRIL 7,1986  - PRECEDE OBJECTIVE FUNCTION EVALUATION BY
                     CONSERVATION OF EVENTS CALCULATIONS
     JUNE 25,1987  - ELIMINATE REDUNDANT CALLS TO DTRSFM
     JULY 1, 1987  - AVOID REDUNDANT CALCULATIONS BY USING INDEX ARRAYS
     JUNE 1,1989   - ADD REPEATING EVENTS (LEFT CENSORING) LOGIC
     APR 13,1991   - ADD WEIGHTED EVENTS
     October 1993  - Converted to HZRCOR C-version 4.0(0)
     Mar 18,1994   - Corrected weighted events (M.E. Turner)

   VERIFICATION
     NOVEMBER 29, 1985
     COMBINATIONS OF RIGHT AND INTERVAL CENSORED DATA IN ALL PHASES
     TESTED AGAINST DATA SETS RUN USING ORIGINAL KATHOLI PROGRAM
     AND AGAINST CALCULATIONS MADE BY SAS.

     OCTOBER, 1991:  TEST OF REVISIONS AGAINST ORIGINAL PROGRAM.

***********************************************************************/


/* SETOBJ entry point not required as of HZRCOR C-version 4.0(0) */



void DHAZRD(double *beta,double *fx,int nvar)
     /* MAIN COMPUTATIONAL ROUTINE */
{
  double llike;
  int i,j;


  Common.errorno = 0;
  CLEAR(Common.errflg);

  /* RETRIEVE THETAS FROM CURRENT PARAMETER VALUE VECTOR */
  for(i=0; i<nvar; i++) {
    j = HZRstr.nm1dx[i];
    Common.theta[j] = beta[i];
  }

  /**
     IF CONSERVATION OF EVENTS IS NOT BEING INVOKED, GET LIKELIHOOD
     FUNCTION
  **/
  if(HZRstr.mode>0) {
    dllike(&llike);
    if(Common.errorno)
      return;
    *fx = -llike;
    return;
  }

  /*-------------------------------------------------------------------
    IF CONSERVATION OF EVENTS IS BEING INVOKED, FIRST DETERMINE THE
    VALUE FOR THE PARAMETER BEING EXPLICITLY EXPRESSED AS A FUNCTION
    OF THE REMAINING PARAMETERS AND THE NUMBER OF EVENTS (SUM OF WEIGHTS)
    -------------------------------------------------------------------*/

  /* CONVERT ESTIMATION PARAMETERS TO EQUATION PARAMETERS */
  if((Common.phase[1]==1 && !HZRstr.fixg1) ||
     (Common.phase[3]==1 && !HZRstr.fixg3)) {
    hzd_theta_to_parms((Common.phase[1]==1 && !HZRstr.fixg1),
		       (Common.phase[3]==1 && !HZRstr.fixg3));
    if(Common.errorno)
      return;
  }
  /* NOW ESTIMATE THE REMAINING SCALING THETA FOR PHASE FIXMU */
  consrv();
	
  if(Common.errorno)
    return;
  /**
     NOW USE THE INFORMATION TEMPORARILY PRESTORED BY CONSERVATION OF
     EVENT SUBPROGRAM TO EXPIDITE THE CALCULATION OF THE OBJECTIVE
     FUNCTION
  **/
  Setobj.facall = FALSE;
  DCEOBJ_main_logic(fx);
}

void DCEOBJ(double *fx){
  Setobj.facall = TRUE;
  DCEOBJ_main_logic(fx);
}

void DCEOBJ_main_logic(double *fx){
  /*xtended lnlike;*/

  G.lnlike = QEXTD(ZERO);
  if(HZRstr.factor<=ZERO) {
    hzr_set_error("(SETOBJ900)",2);
    return;
  }
  Setobj.lfactr = LOGe(HZRstr.factor);

  for(G.I=0; G.I<Common.Nobs; G.I++)
    if(!DCEOBJ_obs_loop())
      return;

   /* NOW RETURN THE OBJECTIVE FUNCTION */
  *fx = -QTOD(G.lnlike);
}

logical DCEOBJ_obs_loop(void)
     /**
   LOOP TO CALCULATE LOG LIKELIHOOD USING PRESTORED VALUES FROM
   CONSERVATION OF EVENT ROUTINE (IN ESSENCE THIS HAS FORCED A DOUBLE
   SUMMATION).
**/
{
  double cf[4],lcf[4],hf[4],lhf[4],cfct[4],lcfct[4];
  double cfst[4],lcfst[4];
  double cumhaz,hazard,cumhct,dcf,lct,llike;
  double cumhst,dt,lhazrd=0;
  double c1,c2,c3,ctime,time;
  double c1w,c3w,weight,stime;

  G.Im1p7 = (G.I*HZRstr.pp7)-1;

  /* CALCULATE LOG CUMULATIVE HAZARD AND HAZARD FUNCTIONS */

  if(Common.phase[1]==1) {
    if(HZRstr.fixmu==1 || Setobj.facall)
      HZRstr.lnmu1[G.I] += Setobj.lfactr;
    lcf[1] = HZRstr.lnmu1[G.I]+HZRstr.lng1[G.I];
    lhf[1] = HZRstr.lnmu1[G.I]+HZRstr.lnsg1[G.I];
  }

  if(Common.phase[2]==1) {
    if(HZRstr.fixmu==2 || Setobj.facall)
      HZRstr.lnmu2[G.I] += Setobj.lfactr;
    lcf[2] = HZRstr.lnmu2[G.I]+HZRstr.lng2[G.I];
    lhf[2] = HZRstr.lnmu2[G.I];
  }

  if(Common.phase[3]==1) {
    if(HZRstr.fixmu==3 || Setobj.facall)
      HZRstr.lnmu3[G.I] += Setobj.lfactr;
    lcf[3] = HZRstr.lnmu3[G.I]+HZRstr.lng3[G.I];
    lhf[3] = HZRstr.lnmu3[G.I]+HZRstr.lnsg3[G.I];
  }

  if(!hzr_cum_haz_calc(&cumhaz,cf,lcf))
    return FALSE;

  /* CALCULATE CUMULATIVE HAZARD FUNCTION FOR STARTING TIME, IF NEEDED */
  cumhst = ZERO;
  stime = Common.obs[G.Im1p7+6];
  if(stime>ZERO) {
    if(Common.phase[1]==1)
      lcfst[1] = HZRstr.lnmu1[G.I]+HZRstr.lng1st[G.I];
    if(Common.phase[2]==1)
      lcfst[2] = HZRstr.lnmu2[G.I]+HZRstr.lng2st[G.I];
    if(Common.phase[3]==1)
      lcfst[3] = HZRstr.lnmu3[G.I]+HZRstr.lng3st[G.I];
    hzr_cum_haz_calc(&cumhst,cfst,lcfst);
  }

  /* CALCULATE HAZARD FUNCTION (IF NEEDED) AND ITS ASSOCIATED WEIGHT */
  hazard = ZERO;
  c1 = Common.obs[G.Im1p7+2];
  weight = Common.obs[G.Im1p7+7];
  c1w = c1*weight;
  if(c1>ZERO) {
    if(!hzr_cum_haz_calc(&hazard,hf,lhf))
      return FALSE;

    /* WE ACTUALLY NEED THE LOG OF THE HAZARD */
    if(hazard<=ZERO) {
      hzr_set_error("(SETOBJ930)",2);
      return FALSE;
    }
    lhazrd = LOGe(hazard);
  }

  /* CALCULATE INTERVAL CUMULATIVE HAZARD PIECE, IF NEEDED */
  c3 = Common.obs[G.Im1p7+4];
  c3w = c3*weight;
  lct = ZERO;
  cumhct = ZERO;
  if(c3>ZERO) {
    time = Common.obs[G.Im1p7+1];
    ctime = Common.obs[G.Im1p7+5];
    if(ctime>ZERO) {
      if(Common.phase[1]==1)
	lcfct[1] = HZRstr.lnmu1[G.I]+HZRstr.lng1ct[G.I];
      if(Common.phase[2]==1)
	lcfct[2] = HZRstr.lnmu2[G.I]+HZRstr.lng2ct[G.I];
      if(Common.phase[3]==1)
	lcfct[3] = HZRstr.lnmu3[G.I]+HZRstr.lng3ct[G.I];
      hzr_cum_haz_calc(&cumhct,cfct,lcfct);
    }
    /**
       CALCULATE NELSON-TYPE APPROXIMATION TO HAZARD FUNCTION BY DIFFERENCES
    **/
    dcf = cumhaz-cumhct;
    dt = time-ctime;
    if(dcf<=ZERO || dt<=ZERO) {
      hzr_set_error("(SETOBJ950)",2);
      return FALSE;
    }
    /*  lct = hzd_ln_A_div_B(dcf,dt);  */
    lct = dlgaob(dcf,dt); 
    if(Common.errorno) {
      hzr_set_error("(SETOBJ960)",Common.errorno);
      return FALSE;
    }
  }

  /* NOW FORM THE LOG LIKLIHOOD FUNCTION FOR THIS OBSERVATION */
  c2 = Common.obs[G.Im1p7+3];
  llike = -(c1w+c2+c3w)*(cumhaz-cumhst);
  if(c1>ZERO)
    llike += c1w*lhazrd;
  if(c3>ZERO)
    llike += c3w*lct;

  /**
     FORM THE GLOBAL LOGLIKELIHOOD FUNCTION FOR ALL SUBJECTS, USING
     HIGH PRECISION TO SKIRT THE PROBLEM OF ADDING SMALL VALUES TO A
     LARGE CUMULATIVE SUM.
  **/
  G.lnlike = QSUM(G.lnlike,QEXTD(llike));

  return TRUE;
}
