#include <math.h>
#include <common.h>
#include <string.h>

#include <hzr_set_error.h>
#include <hzr_qextd.h>
#include <hzr_qtod.h>
#include <hzr_calc_loge_mu.h>
#include <hzr_cum_haz_func.h>
#include <hzr_cum_haz_calc.h>
#include <hzr_qsum.h>
#include <hzd_theta_to_parms.h>
#include <hzd_ln_A_div_B.h>

logical SETLIK_obs_loop(void);

/* static void SETLIK(void) */
 
/***********************************************************************
 
   DETERMINATION OF THE LOG LIKELIHOOD FUNCTION
 
   PURPOSE
     TO EVALUATE THE LOG LIKELIHOOD FUNCTION AND, SECONDARILY, TO
     PRECOMPUTE IF NEEDED CERTAIN QUANTITIES FOR ANALYTICAL
     DERIVATIVES.
 
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
                    |              _             _
              NDATA |             |               |
       LLIKE = SUM  | C1(I)*W1(I)*|LOGE(HF(T(I))) |
               I=1  |             |_             _|
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
 
   DESCRIPTION OF PARAMETERS FOR INITIALIZATION
     INPUT
       OBS    - REAL*8  NOBS*P-VECTOR OF OBSERVATIONS
       THETA  - REAL*8  N-VECTOR OF ALL MODEL PARAMETERS
       STATUS - INTEGER N-VECTOR OF STATUS FLAGS FOR MODEL PARAMETERS
                        0 - FIXED PARAMETER
                        1 - ESTIMATED PARAMETER
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
       GRAD   - LOGICAL IF .TRUE. ANALYTIC GRADIENT TO BE CALCULATED
       HESS   - LOGICAL IF .TRUE. ANALYTIC HESSIAN TO BE CALCULATED
 
     OUTPUT
       NONE
 
   DESCRIPTION OF PARAMETERS FOR COMPUTATIONAL ENTRY DLLIKE
     INPUT
       NONE
 
     OUTPUT
       LLIKE  - REAL*8  LOG LIKELIHOOD
       D1LL1  - REAL*8  NOBS-VECTOR OF FIRST DERIVATIVE PIECES, PHASE 1
       D1LL2  - REAL*8  NOBS-VECTOR OF FIRST DERIVATIVE PIECES, PHASE 2
       D1LL3  - REAL*8  NOBS-VECTOR OF FIRST DERIVATIVE PIECES, PHASE 3
       D2LL11 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (1,1)
       D2LL12 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (1,2)
       D2LL13 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (1,3)
       D2LL22 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (2,2)
       D2LL23 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (2,3)
       D2LL33 - REAL*8  NOBS-VECTOR OF SECOND DERIVATIVE PIECES (3,3)
 
     WORKING
       INDXI  - INTEGER IVAR-VECTOR FOR STORING INDICES FOR X
       INDXP  - INTEGER IVAR-VECTOR FOR STORING CORRESPONDING PHASES
       INDXT  - INTEGER IVAR-VECTOR FOR STORING CORRESPONDING THETAS
 
   DESCRIPTION OF PARAMETERS FROM COMMON BLOCKS
     INPUT
       NOBS   - INTEGER NUMBER OF OBSERVATIONS IN DATA SET
       P      - INTEGER NUMBER OF UNIQUE CONCOMITANT VARIABLES
       PHASE  - INTEGER 3-VECTOR, =1 IF PHASE IS PRESENT
       NTHETA - INTEGER NUMBER OF ALL MODEL PARAMETERS
       FIXG1  - LOGICAL IF .TRUE. EARLY SHAPING FUNCTION IS FIXED
       FIXG3  - LOGICAL IF .TRUE. LATE SHAPING FUNCTION IS FIXED
       IVAR   - INTEGER NUMBER OF SCALING FACTOR PARAMETERS
 
     OUTPUT
       NFNCTS - INTEGER NUMBER OF FUNCTION EVALUATIONS PERFORMED
       ERROR  - INTEGER ERROR FLAG
                        -1 - UNDERFLOW (MANAGED BY THE SYSTEM)
                         0 - NO ERROR DETECTED
                         1 - ARITHMETIC OVERFLOW (FATAL ERROR)
                         2 - VARIABLE OUTSIDE LEGAL RANGE (EG. SIGN)
       ERRFLG - CHARACTER*8 ERROR MESSAGE
 
   REMARKS
     1.  PRIOR TO THE FIRST CALL TO DLLIKE, AN INITIALIZATION CALL
         TO SETLIK MUST BE MADE.  THE PURPOSE OF THIS CALL IS TO
         ESTABLISH THE VARIABLE DIMENSION ARRAYS USED IN THE
         PROCESSING.  THIS SHOULD HAVE TO BE DONE ONLY ONCE FOR
         A GIVEN PROBLEM, SINCE THE ARRAYS RELATE TO THE SIZE OF
         THE DATA SET BEING PROCESSED AND NOT AT ALL TO THE
         DIMENSIONALITY OF THE OPTIMIZATION BEING PERFORMED.
 
     2.  FOR CALCULATION PURPOSES THE MODEL PARAMETERS ARE TRANSFORMED
         TO A NEW SET OF ESTIMATION PARAMETERS.  THE FORM OF THESE
         NEW PARAMETERS IS GIVEN IN SUBROUTINE DTRSFM.
 
     3.  THE FIRST SEVEN (7) OBSERVATIONS IN VECTOR OBS ARE:
         OBS(1) - TIME   = FOLLOWUP INTERVAL
         OBS(2) - C1     = NUMBER OF UNCENSORED OBSERVATIONS AT T
         OBS(3) - C2     = NUMBER OF CENSORED OBSERVATIONS AT T
         OBS(4) - C3     = NUMBER OF INTERVAL CENSORED OBS. AT CTIME
         OBS(5) - CTIME  = INTERVAL CENSORED STARTING TIME (<T)
         OBS(6) - STIME  = STARTING TIME (<T) FOR REPEATING EVENT OR
                           TIME-VARYING COVARIABLE
         OBS(7) - WEIGHT = WEIGHT FOR THE EVENT (GE 0)
 
         NOTE:  THE ROUTINE ASSUMES THAT PRIOR TO CALLING DLLIKE
                THE VALUES FOR TIME, C1, C2, C3, CTIME, STIME, AND
                WEIGHT HAVE BEEN CHECKED AND THEY ARE LEGITIMATE:
                TIME>0, 0 =< CTIME < TIME, C1 >= 0, C2 >= 0, C3 >= 0;
                STIME < TIME; WEIGHT >= 0.  THESE WILL NOT BE RECHECKED.
 
 
     4.  WE ASSUME CURRENT VALUES FOR ALL PARAMETERS ARE IN THE
         VECTOR THETA.
 
     5.  ON EXITING ROUTINE, CURRENT VALUES FOR ALL SHAPING MODEL
         PARAMETERS ARE AVAILABLE.
 
     6.  TO AVOID ADDITIONAL PASSES THROUGH THE DATA WHEN ANALYTIC
         DERIVATIVES ARE TO BE CALCULATED, THE NEEDED PIECES FOR
         THESE ARE PRECALCULATED AND STORED FOR SUBROUTINE SETDLL.
         PLEASE REFER TO THAT SUBROUTINE FOR MATHEMATICAL DETAILS.
 
   SUBROUTINES REQUIRED
     DLGAOB - CALCULATE LOGE(A/B)
     DLG1   - CALCULATE LOGE(G1(T)), RETURNS LG1 AND LSG1
     DLG3   - CALCULATE LOGE(G3(T)), RETURNS LG3 AND LSG3
     DTRSFM - INTERCONVERT ESTIMATION AND MODEL PARAMETERS
 
   METHOD
     THE FORM OF THE LOG LIKELIHOOD FUNCTION USES AN ALTERNATIVE
     NELSON-TYPE STRATEGY FOR INTERVAL CENSORED EVENTS.  NAMELY, THE
     DENSITY FUNCTION IS FACTORED, BRINGING OUT THE CUMULATIVE HAZARD
     PIECE.  THE REMAINING SURVIVORSHIP DIFFERENCES ARE THEN
     REPLACED BY AN APPROXIMATION TO THE HAZARD FUNCTION.  THIS
     STRATEGY PROVIDES FOR AN IMPORTANT SIMPLIFICATION OF THE
     COMPUTATIONS, INCLUDING REMOVING THE APPROXIMATIONS IN THE
     CONSERVATION OF EVENTS SCHEME.  IN THEORY THE DIVISION BY
     CT(I)-T(I) IS UNNECESSARY EXCEPT TO SCALE THE LIKELIHOOD.  WE
     HAVE ELECTED TO DO THAT SO THAT THE LIKELIHOOD VALUE IS SIMILAR
     WHETHER OR NOT INTERVAL CENSORING IS USED (EG, IF PRELIMINARY
     ANALYSES ARE DONE USING THE MIDPOINT OF THE INTERVAL).
 
     WHEREEVER NECESSARY, ROUTINES ARE CALLED THAT IMPROVE THE
     ACCURACY OF THE COMPUTATIONS.
 
     NOTE THAT IN THE PARTICULAR FORMULATION OF THE LOG LIKELIHOOD
     FUNCTION GIVEN ABOVE, PARTIAL OBSERVATIONS MAY BE INCORPORATED
     BY SEGMENTATION.  SPECIFICALLY, AN INDIVIDUAL'S FOLLOWUP CAN BE
     SEGMENTED INTO PORTIONS EXTENDING FROM 0 TO ST AND A PORTION
     FROM ST TO T.  THE IMPORTANCE OF THIS OBSERVATION IS THAT IF THE
     EVENT IS A NONTERMINATING (REPEATING) ONE, ST-T REPRESENTS THE
     INTERVAL BETWEEN EVENTS OF A NONHOMOGENEOUS POISSON PROCESS.
     ALTERNATIVELY, IF TIME VARYING COVARIABLES ARE INTRODUCED THAT
     MERELY SHIFT THE HAZARD FUNCTION UP OR DOWN FROM ITS CURRENT
     POSITION (A HIGHLY RESTRICTIVE ASSUMPTION, BUT QUITE ANALOGOUS
     TO THE COX MODEL), THEN ST-T REPRESENTS AN INTERVAL OVER WHICH
     THE TIME-VARYING COVARIABLE IS (PIECEWISE) UNCHANGING.
 
     YET A FURTHER CONSIDERATION IS THE WEIGHTED CASE WHEREBY THE
     EVENT TAKES ON A VALUE (POSITIVE, BUT POSSIBLY CONTINUOUS)
     OTHER THAN 0 OR 1.  "HAZARD" PIECES REQUIRE WEIGHTING.  THIS
     CAN BE SHOWN TO CONSERVE EVENTS, AND IN THE CONSTANT HAZARD
     CASE, FOR EXAMPLE, THE MLE IS THE SUM OF ALL WEIGHTS DIVIDED BY
     THE SUM OF ALL TIME.
 
   IMPLEMENTATION
     E.H. BLACKSTONE, MD     OCTOBER, 1985
     FORTRAN77 TRANSLATION IN PART OF SUBROUTINE DHAZRD BY
     C.R. KATHOLI, PH.D.
 
   REVISIONS
     MARCH 25,1986 - USE EXTENDED ARITHMATIC FOR MAIN SUMMATIONS
     MARCH 26,1986 - INITIALIZATION BY SETLIK IMPLEMENTED
     JUNE 25,1987  - ADD PRECALCULATED INFORMATION FOR FIRST AND
                     SECOND (ANALYTIC) DERIVATIVES IF NEEDED
     JULY 1,1987   - AVOID REDUNDANT CALCULATIONS BY USING INDEX ARRAYS
     JULY 1,1989   - ADD REPEATING EVENTS (LEFT CENSORING) LOGIC
     APR 12,1991   - ADD WEIGHTED EVENTS
     APR 24,1991   - ALTERED LOG-LIKELIHOOD TO NELSON-TYPE FORMULATION
     December 1993 - Converted to HZRCOR C-version 4.0(0)
     Feb 22,1994   - Corrected weighted event (M. E. Turner)
 
   VERIFICATION
     NOVEMBER 29, 1985
     COMBINATIONS OF RIGHT AND INTERVAL CENSORED DATA IN ALL PHASES
     TESTED AGAINST DATA SETS RUN USING ORIGINAL KATHOLI PROGRAM
     AND AGAINST CALCULATIONS MADE BY SAS.
 
     OCTOBER, 1991:  TESTING OF NEW FORMULATION FOR LIKELIHOOD, FOR
     INTERVAL CENSORED REFORMULATION, AND LEFT CENSORING TESTED
     AGAINST ORIGINAL PROGRAM.
 
***********************************************************************/
 

/* SETLIK entry point not required as of HZRCOR C-version 4.0(0) */

 
void dllike(double *llike)
     /* MAIN COMPUTATIONAL ROUTINE */
{
  long j;
 
  G.entry = DLLIKE;
 
  /* INCREMENT NUMBER OF FUNCTION EVALUATIONS COUNTER. */
  HZRstr.nfncts++;
 
  Common.errorno = 0;
  CLEAR(Common.errflg);
 
  /* CONVERT ESTIMATION PARAMETERS TO EQUATION PARAMETERS */
  if((Common.phase[1]==1 && !HZRstr.fixg1) ||
     (Common.phase[3]==1 && !HZRstr.fixg3)) {
    hzd_theta_to_parms((Common.phase[1]==1 && !HZRstr.fixg1),
		       (Common.phase[3]==1 && !HZRstr.fixg3));
    if(Common.errorno)
      return;
  }
 
  /**
     IF THERE IS NO CONCOMMITANT INFORMATION, THE SCALING FACTORS FOR
     EACH INDIVIDUAL REMAIN THE SAME.
  **/
  if(Common.p==0)
    for(j=1; j<=3; j++)
      if(Common.phase[j]==1)
	G.lmu[j] = Common.theta[j+7];
 
  /* INITIALIZE SUMMATIONS */
  G.lnlike = QEXTD(ZERO);
 
  G.dograd = HZRstr.angrad;
  G.dohess = HZRstr.anhess;
 
  /**
     LOOP TO CALCULATE LOG LIKELIHOOD
  **/
  for(G.I=0; G.I<Common.Nobs; G.I++)
    if(!SETLIK_obs_loop())
      return;
 
  *llike = QTOD(G.lnlike);
}
 
logical SETLIK_obs_loop(void){
  double cf[4],lcf[4],hf[4],lhf[4],cfct[4],lcfct[4];
  double cfst[4],lcfst[4];
  double cumhaz,hazard,cumhct,dcf=0,lct=0;
  double cumhst,dt;
  double lc1haz=0,lhazrd=0;
  double der1=0,der3=0,llike;
  double time,c1,c2,c3,ctime,c1c2c3;
  double stime,c1w,c3w,weight;
  long k,l;
 
  G.Im1p7 = (G.I*HZRstr.pp7)-1;
 
  time = Common.obs[G.Im1p7+1];
  if(HZRstr.mode<=0 && G.entry==D2LCON) {
    if(Common.phase[1]==1) {
      lcf[1] = HZRstr.lnmu1[G.I]+HZRstr.lng1[G.I];
      lhf[1] = HZRstr.lnmu1[G.I]+HZRstr.lnsg1[G.I];
    }
    if(Common.phase[2]==1) {
      lcf[2] = HZRstr.lnmu2[G.I]+HZRstr.lng2[G.I];
      lhf[2] = HZRstr.lnmu2[G.I];
    }
    if(Common.phase[3]==1) {
      lcf[3] = HZRstr.lnmu3[G.I]+HZRstr.lng3[G.I];
      lhf[3] = HZRstr.lnmu3[G.I]+HZRstr.lnsg3[G.I];
    }
  } else {
    hzr_calc_loge_mu();
    if(!hzr_cum_haz_func(time,lcf,&HZRstr.lng1,lhf,&HZRstr.lnsg1,FALSE))
      return FALSE;
  }

  if(!hzr_cum_haz_calc(&cumhaz,cf,lcf))
    return FALSE;
 
  /* CALCULATE LOG CUMULATIVE HAZARD FUNCTION FOR STARTING TIME (STIME) */
  cumhst = ZERO;
  for(k=1; k<=3; k++)
    cfst[k] = cfct[k] = ZERO;
  stime = Common.obs[G.Im1p7+6];
  if(stime>ZERO) {
    if(!hzr_cum_haz_func(stime,lcfst,&HZRstr.lng1st,NULL,NULL,FALSE))
      return FALSE;
    if(!hzr_cum_haz_calc(&cumhst,cfst,lcfst))
      return FALSE;
  }
 
  /* HAZARD FUNCTION (IF NEEDED) AND ITS ASSOCIATED WEIGHT */
  c1 = Common.obs[G.Im1p7+2];
  weight = Common.obs[G.Im1p7+7];
  c1w = c1*weight;
  if(c1>ZERO) {
    if(!hzr_cum_haz_calc(&hazard,hf,lhf))
      return FALSE;
    if(hazard<=ZERO) {
      hzr_set_error("(SETLIK930)",2);
      return FALSE;
    }
    lhazrd = LOGe(hazard);
    if(G.dograd || G.dohess)
      lc1haz = -TWO*lhazrd;
  }
 
  /* CALCULATE INTERVAL CUMULATIVE HAZARD PIECE, IF NEEDED */
  c3 = Common.obs[G.Im1p7+4];
  c3w = c3*weight;
  if(c3>ZERO) {
    cumhct = ZERO;
    ctime = Common.obs[G.Im1p7+5];
    if(ctime>ZERO) {
      if(HZRstr.mode<=0 && G.entry==D2LCON) {
	if(Common.phase[1]==1)
	  lcfct[1] = HZRstr.lnmu1[G.I]+HZRstr.lng1ct[G.I];
	if(Common.phase[2]==1)
	  lcfct[2] = HZRstr.lnmu2[G.I]+HZRstr.lng2ct[G.I];
	if(Common.phase[3]==1)
	  lcfct[3] = HZRstr.lnmu3[G.I]+HZRstr.lng3ct[G.I];
      } else if(!hzr_cum_haz_func(ctime,lcfct,&HZRstr.lng1ct,
				  NULL,NULL,FALSE))
	return FALSE;
      if(!hzr_cum_haz_calc(&cumhct,cfct,lcfct))
	return FALSE;
    }
 
    /**
       CALCULATE NELSON-TYPE APPROXIMATION TO HAZARD FUNCTION BY DIFFERENCES
    **/
    dcf = cumhaz-cumhct;
    dt = time-ctime;
    if(dcf<=ZERO || dt<=ZERO) {
      hzr_set_error("(SETLIK950)",2);
      return FALSE;
    }
    lct = hzd_ln_A_div_B(dcf,dt);
    if(Common.errorno) {
      hzr_set_error("(SETLIK960)",Common.errorno);
      return FALSE;
    }
  }
 
  /* NOW FORM THE LOG LIKLIHOOD FUNCTION */
  c2 = Common.obs[G.Im1p7+3];
  c1c2c3 = c1w+c2+c3w;
 
  if(G.entry==DLLIKE) {
    /**
       FORM THE GLOBAL LOGLIKELIHOOD FUNCTION FOR ALL SUBJECTS, USING
       HIGH PRECISION TO SKIRT THE PROBLEM OF ADDING SMALL VALUES TO A
       LARGE CUMULATIVE SUM.
    **/
    llike = -(c1c2c3)*(cumhaz-cumhst);
    if(c1>ZERO)
      llike += (c1w*LOGe(hazard));
    if(c3>ZERO)
      llike += (c3w*lct);
    G.lnlike = QSUM(G.lnlike,QEXTD(llike));
  }
 
  /**
     If neither ANGRAD nor ANHESS are set, go ahead and return now,
     rather than wade through huge IF/ENDIF.
  **/
  if(!(G.dograd || G.dohess))
    return TRUE;
 
  /**
     NOW CALCULATE THE SCALING FACTORS FOR EACH DERIVATIVE IF THESE
     ARE NEEDED
  **/
  for(k=1; k<=3; k++) {
    if(Common.phase[k]!=1)
      continue;
    if(G.dograd)
      G.d1llp[k] = -c1c2c3*(cf[k]-cfst[k]);
    if(G.dohess)
      G.d2llij[k][k] = ZERO;
    if(c1>ZERO) {
      if(G.dograd) 
	G.d1llp[k] += c1w*ePOW(lhf[k]-lhazrd);
      if(G.dohess) {
	der1 = c1w*ePOW(lhf[k]+lc1haz);
	G.d2llij[k][k] = der1*(hazard-hf[k]);
      }
    }
 
    if(G.dohess)
      G.d2llij[k][k] -= c1c2c3*(cf[k]-cfst[k]);
 
    if(c3>ZERO) {
      if(G.dograd)
	G.d1llp[k] += c3w*(cf[k]-cfct[k])/dcf;
      if(G.dohess) {
	der3 = c3w*((cf[k]-cfct[k])/(dcf*dcf));
	G.d2llij[k][k] += (der3*(dcf+(cfct[k]-cf[k])));
      }
    }
 
    /* FOR SECOND DERIVATIVES, WE MUST FORM CROSS-PRODUCT TERMS */
    if(G.dohess)
      for(l=k+1; l<=3; l++)
	if(Common.phase[l]==1) {
	  G.d2llij[k][l] = ZERO;
	  if(c1>ZERO)
	    G.d2llij[k][l] = -der1*hf[l];
	  if(c3>ZERO)
	    G.d2llij[k][l] += der3*(cfct[l]-cf[l]);
	}
  }
 
  /* IF DOING FIRST DERIVATIVES, SAVE PIECES. */
  if(G.dograd) {
    if(Common.phase[1]==1) HZRstr.d1ll1[G.I] = G.d1llp[1];
    if(Common.phase[2]==1) HZRstr.d1ll2[G.I] = G.d1llp[2];
    if(Common.phase[3]==1) HZRstr.d1ll3[G.I] = G.d1llp[3];
  }
 
  /**
     NOW SAVE THE APPROPRIATE SCALING FACTORS FOR THE DERIVATIVES FOR
     USE IN ENTRY DLLADD.
  **/
  if(G.dohess) {
    if(Common.phase[1]==1) {
      HZRstr.d2ll11[G.I] = G.d2llij[1][1];
      if(Common.phase[2]==1) HZRstr.d2ll12[G.I] = G.d2llij[1][2];
      if(Common.phase[3]==1) HZRstr.d2ll13[G.I] = G.d2llij[1][3];
    }
    if(Common.phase[2]==1) {
      HZRstr.d2ll22[G.I] = G.d2llij[2][2];
      if(Common.phase[3]==1) HZRstr.d2ll23[G.I] = G.d2llij[2][3];
    }
    if(Common.phase[3]==1)
      HZRstr.d2ll33[G.I] = G.d2llij[3][3];
  }
  return TRUE;
} 
