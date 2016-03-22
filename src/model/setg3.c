#include <string.h>
#include <common.h>
#include <hzdinc.h>

#include <hzr_parm.h>
#include <hzr_set_parm_err.h>

void SETG3_ignore_tau(void);
void SETG3_weibull(void);
void SETG3_alpha_fixup(logical (*parm_fixed_func)(void));
void SETG3_alpha_gener(logical (*parm_fixed_func)(void));
void SETG3_alpha_gt_0(void);
void SETG3_alpha_le_0(void);
void SETG3_all_gt_0(void);
void SETG3_all_le_0(void);
void SETG3_eta_gt_0(void);
void SETG3_eta_le_0(void);
void SETG3_gamma_gt_0(void);
void SETG3_gamma_le_0(void);
void SETG3_ignore_tau(void);
void SETG3_verify_ge_2(void);

logical hzr_parms_ge_fixed(void);
logical hzr_parms_ge_1fixed(void);
logical hzr_parm_gamma_fixed(void);
logical hzr_parm_eta_fixed(void);
logical hzr_returns_TRUE(void);
logical hzr_parms_ge_estim(void);
logical hzr_parms_ge_1estim(void);


void SETG3(void)

     /***********************************************************************

   SETUP OF LATE INCREASING HAZARD PHASE

   PURPOSE
     1.  TO SET G3FLAG FOR CALCULATION OF SPECIFIC FORMS FOR THE
         LATE PHASE MODEL

     2.  TO SUPPLY DEFAULT VALUES FOR THE NON- OR MISSPECIFIED
         MODEL PARAMETERS

     THE LATE PHASE MODEL IS BASED ON A 4-PARAMETER SPECIAL CASE OF
     THE HYPERGEOMETRIC SERIES:

             _                       _
            | _            _          |ETA
            ||     _ _      |1/ALPHA  |
            ||    |   |GAMMA|         |
            ||    | T |     |         |   TAU*ALPHA
       G3 = || 1 +|---|     |       -1|   --------- TAU
            ||    |TAU|     |         |   ETA*GAMMA
            ||    |_ _|     |         |
            ||_            _|         |
            |_                       _|

       WHERE

         CF3(T) IS THE LATE PHASE CUMULATIVE HAZARD = MU3*G3(T)
         MU3 IS A PARAMETRIC SCALING FUNCTION

     IN ORDER TO MAINTAIN SEPARATION OF PHASES (AND, THEREFORE,
     MAXIMUM OPPORTUNITY TO RESOLVE INDIVIDUAL PHASES), THE PARAMETERS
     OF G3(T) HAVE BEEN RESTRICTED SO THAT ITS TIME DERIVATIVE
     SG3(T) HAS THE FOLLOWING PROPERTIES:

       1.  SG3(T=0) = 0
       2.  THE FIRST TIME DERIVATIVE OF SG3(T) IS POSITIVE
           (THAT IS, SG3(T) IS RISING FOR ALL T)
       3.  THE SECOND TIME DERIVATIVE OF SG3(T) IS POSITIVE
           (THAT IS, SG3(T) IS CONCAVE UPWARD FOR ALL T)

     THE TWO PARAMETERS ETA AND GAMMA INFLUENCE THE BEHAVIOUR OF
     SG3(T) IN THE VICINITY OF T=0, AND THE ABOVE CONDITIONS REQUIRE
     THAT THE PRODUCT OF ETA AND GAMMA BE GREATER THAN OR EQUAL TO 2.
     ALPHA CONTROLS THE BEHAVIOUR OF SG3(T) FOR T/TAU LARGE. IF
     GAMMA*ETA/ALPHA=2, AND ETA=2, THE LATE PHASE HAZARD FUNCTION
     HF3(T)=MU3*SG3(T) WILL ASYMPTOTE TO A CONSTANT UPWARD SLOPE OF
     MU3 AT LARGE VALUES OF T/TAU, TAU WILL BE THE BACK-EXTRAPOLATED
     TIME AT WHICH THIS ASYMPTOTE INTERSECTS THE TIME AXIS FOR
     SG3(T)=0, AND GAMMA WILL INCREASE AS THE RATE AT WHICH SG3(T)
     APPROACHES THE ASYMPTOTE INCREASES.  ETA>3 RETARDS THE RATE OF
     THE PROCESS AT SMALL VALUES OF T/TAU, ALTHOUGH EVENTUALLY
     RISING AT THE RATE MU3.

     WE PERMIT SPECIFICATION BOTH THAT THE PRODUCT GAMMA*ETA=2
     (GETWO=.TRUE.) AND/OR THE EXPRESSION GAMMA*ETA/ALPHA=2
     (GAETWO=.TRUE.).  HOWEVER, IF BOTH ARE .TRUE. THE HAZARD
     FUNCTION RISES IN A STRAIGHT LINE FROM THE ORIGIN AT A SLOPE
     OF MU3; THUS, WE ALSO SET TAU TO 1.0.

     NOTE:  IF ALPHA=1, THEN THE ABOVE EQUATION COLLAPSES TO A SINGLE
     SHAPING PARAMETER MODEL, WEIBULL IN FORM:

             GAMMA
       G3 = T

     LIMITING CASE

       LET

         ALPHA APPROACH ZERO

       THEN

               _                _
              |  _     _         |ETA
              | |      |GAMMA    |
              | |T/TAU |         |
              | |_    _|         |      TAU
         G3 = |E              - 1|   --------- TAU
              |                  |   ETA*GAMMA
              |                  |
              |                  |
              |_                _|


     N O N - S T A N D A R D   U S E   O F   L A T E   P H A S E

     THE FACILITY TO REMOVE THE ABOVE RESTRICTIONS ON THE RELATION-
     SHIPS OF LATE PHASE PARAMETERS IS BUILT INTO THE SYSTEM.  IF
     THE PARAMETERS ARE SIMPLY CONSTRAINED TO BE POSITIVE, THEN THE
     LATE PHASE CAN, FOR EXAMPLE, BECOME AN ALTERNATIVE WAY TO MODEL
     A SINGLE EARLY PHASE ACCOUNTING FOR ALL THE EVENTS BECAUSE ITS
     SCALE FACTOR IS NOT RELATED TO A FINITE AREA BENEATH THE HAZARD
     FUNCTION.  IT THUS BECOMES A GENERALIZED WEIBULL MODEL.

   DESCRIPTION OF PARAMETERS
     INPUT
       SMUL   - REAL*8  LATE PHASE SCALING PARAMETER
       STAU   - REAL*8  TIME WHEN RISING HAZARD ACCELERATES
       SGAMMA - REAL*8  RATE OF ACCELERATION TO ASYMPTOTE
       SALPHA - REAL*8  SHAPE FOR T/TAU LARGE
       SETA   - REAL*8  OVERALL SHAPING POWER
       GETWO  - LOGICAL .TRUE. IF ETA*GAMMA=2
       GAETWO - LOGICAL .TRUE. IF ETA*GAMMA/ALPHA=2
       WEIBUL - LOGICAL .TRUE. IF ALL PARAMETERS ARE SIMPLY RESTRICTED
                               TO POSITIVE VALUES
       TMAX   - REAL*8  MAXIMUM FOLLOWUP TIME
       STATUS - INTEGER VECTOR OF ESTIMATION FLAGS
                        0 - PARAMETER IS NOT TO BE ESTIMATED (FIXED)
                        1 - PARAMETER IS TO BE ESTIMATED

     OUTPUT
       CHGFLG - CHARACTER*1 12-VECTOR WHERE * DENOTES CHANGE IN
                        PARAMETER VALUE FROM SPECIFICATION AS FOLLOWS:
                        8 - MUL
                        9 - TAU
                       10 - GAMMA
                       11 - ALPHA
                       12 - ETA
       MUL    - REAL*8  LATE PHASE SCALING PARAMETER

   DESCRIPTION OF PARAMETERS IN COMMON BLOCKS /LATE/ AND /ERROR/
     INPUT
       NONE

     OUTPUT
       TAU    - REAL*8  TIME WHEN RISING HAZARD ACCELERATES
       GAMMA  - REAL*8  RATE OF ACCELERATION TO ASYMPTOTE
       ALPHA  - REAL*8  SHAPE FOR T/TAU LARGE
       ETA    - REAL*8  OVERALL SHAPING POWER
       G3FLAG - INTEGER FLAG
                        1 - ALPHA>0:  GENERIC MODEL
                        2 - ALPHA=0:  LIMITING (EXPONENTIAL) CASE
                        3 - ALPHA>0:  UNCONSTRAINED GENERIC MODEL
                        4 - ALPHA=0:  UNCONSTRAINED EXPONENTIAL CASE
       ERROR  - INTEGER ERROR FLAG
                        0 - NO ERROR
                        1 - ERROR
       ERRFLG - CHARACTER*48 STRING OF *** DENOTING TERMINAL ERROR
                FOR FIXED INADMISSIBLE SPECIFIED VALUE
                OF THE PARTICULAR PARAMETER AS FOLLOWS:
                (34:36) - MUL
                (37:39) - TAU
                (40:42) - GAMMA
                (43:45) - ALPHA
                (46:48) - ETA

   SUBPROGRAMS REQUIRED
     NONE

   REMARKS
     1.  INPUT PARAMETERS ARE NOT CHANGED BY THE SUBROUTINE.  THE
         OUTPUT SPECIFICATION, HOWEVER, IS THE ONE USED BY THE
         OPTIMIZATION PROGRAM.  CHANGES IN THE SPECIFICATION ARE
         FLAGGED INDIVIDUALLY BY PARAMETER (CHGFLG).

     2.  MUL WILL BE SET TO 1.0 IF IT IS INPUT AS ZERO OR NEGATIVE.

     3.  IF TAU <= ZERO, THEN SET IT TO 2/3 THE MAXIMUM FOLLOWUP
         INTERVAL.

     4.  IF GAETWO=.FALSE. (GAMMA*ETA/ALPHA NOT SET TO 2.0) AND
         ALPHA <= 0.0, WE ASSUME THE LIMITING EXPONENTIAL FORM OF
         THE MODEL IS DESIRED.

   METHOD
     BASED UPON THE SPECIFICATIONS FOR ALPHA, THE FLAG FOR THE
     PRODUCT GAMMA*ETA=2 AND THE FLAG FOR THE EXPRESSION
     GAMMA*ETA/ALPHA=2, THE TYPE OF MODEL IS DETERMINED.

  _____________________________________________________________________

     ALPHA  GETWO  GAETWO  G3FLAG    MODEL    GAMMA*ETA  GAMMA*ETA
                                                         ---------
                                                           ALPHA
  _____________________________________________________________________

       >0     F       F       1     GENERIC       >2        >2
       >0     T       F       2     GENERIC       =2        >2
       >0     F       T       3     GENERIC       >2        =2
       >0     T       T       4     GENERIC       =2        =2
      <=0     F       F       5   EXPONENTIAL     >2     UNDEFINED
      <=0     T       F       6   EXPONENTIAL     =2     UNDEFINED
      <=0     F       T       3     GENERIC       >2        =2
      <=0     F       T       4     GENERIC       =2        =2
  _____________________________________________________________________


   IMPLEMENTATION
     E.H. BLACKSTONE,MD      OCTORBER 1985

   REVISIONS
     MARCH 28,1986 - MOVE 'STATUS' TO ARGUMENT LIST (VARIABLE
                     DIMENSIONED ARRAY)
     JUNE 5, 1986  - CHGFLG CHANGED TO CHARACTER*48 AND ADDED
                     ERROR TRACING MESSAGE
     October 1993  - Converted to HZRCOR C-version 4.0(0)
     DEC 15, 1994  - LOGIC OF GETWO CHANGED TO PROPERLY REFLECT THE
                     FACT THAT ONE OF THE TWO PARAMETERS MUST BE 
                     FIXED, AND IF ONE IS ALREADY, THE OTHER MUST
                     BE FIXED AS WELL.

      HZD4.033 (07/24/95)
         -  Implemented above 12/15/94 change by EHB.
         -  Made two corrections to logic that should've been FIXing
            ALPHA when FIXGAE2 is specified.  Ref 12/15/94 notes from EHB
            and item #1 in 5/9/95 triage list.
      HZD4.034 (07/24/95)
         Corrections to 4.033 code handling ALPHA FIXing.
      HZD4.035 (07/24/95)
         Yet another correction to the 4.033 code.

   VERIFICATION
     NOVEMBER 15,1985
     LIMITED EXPLORATION OF THE MANY POSSIBLE COMBINATIONS OF INPUT
     VARIABLES.

***********************************************************************/

{
  /* Initialize ... */

  Late.g3flag = 0;
  Common.errorno = 0;
  Late.muL = ZERO;
  Late.tau = ZERO;
  Late.gamma = ZERO;
  Late.alpha = ZERO;
  Late.eta = ZERO;

  /**
     IF TAU, GAMMA, OR ETA IS FIXED AT A VALUE THAT IS <= ZERO, THIS IS
     AN ERROR.  ALPHA CAN BE FIXED AT ZERO.
  **/
  if(HZRstr.l.tau<=ZERO && hzr_parm_is_fixed(HZ_TAU)) {
    hzr_set_parm_err("(SETG3900)",HZ_TAU);
    return;
  }
  if(HZRstr.l.gamma<=ZERO && hzr_parm_gamma_fixed()) {
    hzr_set_parm_err("(SETG3910)",HZ_GAMMA);
    return;
  }
  if(HZRstr.l.alpha<ZERO && hzr_parm_is_fixed(HZ_ALPHA)) {
    hzr_set_parm_err("(SETG3920)",HZ_ALPHA);
    return;
  }
  if(HZRstr.l.eta<=ZERO && hzr_parm_eta_fixed()) {
    hzr_set_parm_err("(SETG3930)",HZ_ETA);
    return;
  }

  /* MUL MUST BE POSITIVE, ELSE IT IS SET TO 1.0 */
  Late.muL = HZRstr.l.muL;
  if(Late.muL<=ZERO) {
    Late.muL = ONE;
    hzr_parm_changed(HZ_MUL);
  }

  /**
     CHECK ON REASONABLENESS OF TAU
     IF TAU <= ZERO, THEN SET IT TO 2/3 OF THE LONGEST FOLLOWUP
     INTERVAL.

     NOTE:  IF BOTH GAMMA*ETA=2 AND GAMMA*ETA/ALPHA=2, THE HAZARD
     FUNCTION RISES FROM THE ORIGIN AND TAU IS NOT REQUIRED.
     ALSO IF ALPHA EQUALS 1 (AS IT DOES, FOR EXAMPLE, IN THIS
     SITUATION), THEN THE LATE PHASE MODEL IS REEXPRESSED IN
     TERMS OF A SINGLE EXPONENTIAL FUNCTION OF TIME; THUS WE
     MUST EXAMINE AND POSSIBLY RESET GAMMA AND/OR ETA (IN THIS
     WE CHOOSE GAMMA OVER ETA).

     NOTE:  WE ASSUME THAT A FIXED ALPHA=1 TAKES PRECEDENCE OVER ALL
     OTHER CONSIDERATIONS.
  **/
  Late.tau = HZRstr.l.tau;
  Late.alpha = HZRstr.l.alpha;
  Late.gamma = HZRstr.l.gamma;
  Late.eta = HZRstr.l.eta;
  if((Late.g_two && Late.ga_two) ||
     (Late.alpha==ONE && hzr_parm_is_fixed(HZ_ALPHA))) {
    SETG3_ignore_tau();
  } else if(Late.tau<=ZERO) {
    Late.tau = TWO*HZRstr.Tmax/3.0e0;
    hzr_parm_changed(HZ_TAU);
    if(Late.tau<=ZERO) {
      hzr_set_parm_err("(SETG3950)",HZ_TAU);
      return;
    }
  }

  /**
     DETERMINE THE TYPE OF MODEL BASED UPON ALPHA, GAMMA, ETA, AND
     THE TWO SPECIAL CASE FLAGS.

     IF GAETWO=.FALSE. AND ALPHA IS FIXED AT ZERO, THEN THE LIMITING
     EXPONENTIAL CASE IS BEING SPECIFIED.
  **/
  Late.g3flag = 1;
  if(!Late.ga_two &&
     (Late.alpha==ZERO && hzr_parm_is_fixed(HZ_ALPHA)))
    Late.g3flag = 2;

  /**
     G E N E R A L I Z E D   W E I B U L L  M O D E L
     ------------------------------------------------
     NOW HANDLE THE SPECIAL SITUATION OF THE GENERALIZED WEIBULL, WHERE
     WE ADMIT ALL POSITIVE VALUES OF THE PARAMETERS.  HERE WE WILL HAVE
     NO MERCY ON UNSPECIFIED VALUES SINCE THE USE OF THIS SPECIAL
     SITUATION MUST BE CAREFULLY CONCEIVED.
  **/
  if(HZRstr.weibul) {
    SETG3_weibull();
    return;
  }

  /**
     U S U A L   L A T E   P H A S E   M O D E L
     -------------------------------------------

     NOW SEQUENTIALLY GO THROUGH EACH AND EVERY POSSIBILITY OF ALPHA,
     GAMMA, AND ETA TO SET UP REASONABLE VALUES AND ALSO APPROPRIATE
     FIXING OF PARAMETERS WHEN SPECIAL CASES ARE INVOKED.
  **/

  if(Late.alpha>ZERO && Late.gamma>ZERO && Late.eta>ZERO)
    SETG3_all_gt_0();
  else if(Late.alpha<=ZERO && Late.gamma>ZERO && Late.eta>ZERO)
    SETG3_alpha_le_0();
  else if(Late.alpha>ZERO && Late.gamma<=ZERO && Late.eta>ZERO)
    SETG3_gamma_le_0();
  else if(Late.alpha>ZERO && Late.gamma>ZERO && Late.eta<=ZERO)
    SETG3_eta_le_0();
  else if(Late.alpha<=ZERO && Late.gamma<=ZERO && Late.eta>ZERO)
    SETG3_eta_gt_0();
  else if(Late.alpha<=ZERO && Late.gamma>ZERO && Late.eta<=ZERO)
    SETG3_gamma_gt_0();
  else if(Late.alpha>ZERO && Late.gamma<=ZERO && Late.eta<=ZERO)
    SETG3_alpha_gt_0();
  else if(Late.alpha<=ZERO && Late.gamma<=ZERO && Late.eta<=ZERO)
    SETG3_all_le_0();
}

void SETG3_ignore_tau(void){
  Late.tau = ONE;
  hzr_parm_set_fixed(HZ_TAU);
  if(HZRstr.l.tau!=ONE)
    hzr_parm_changed(HZ_TAU);
  if(Late.alpha!=ONE && hzr_parm_is_fixed(HZ_ALPHA)) {
    hzr_set_parm_err("(SETG3940)",HZ_ALPHA);
    return;
  }
  Late.alpha = ONE;
  hzr_parm_set_fixed(HZ_ALPHA);
  if(HZRstr.l.alpha!=ONE)
    hzr_parm_changed(HZ_ALPHA);

  /* CASE OF GETWO OR GAETWO, SO GAMMA OR ETA IS 2 AND THE OTHER 1 */
  if(Late.g_two || Late.ga_two) {
    hzr_parm_set_fixed(HZ_GAMMA);
    hzr_parm_set_fixed(HZ_ETA);
    Late.g_two = TRUE;
    Late.ga_two = TRUE;
    if(HZRstr.l.eta==TWO) {
      Late.eta = TWO;
      Late.gamma = ONE;
    } else {
      Late.gamma = TWO;
      Late.eta = ONE;
    }
  } else {
    /* CASE OF ALPHA=1 AND WE WILL ESTIMATE GAMMA*ETA AS THE EXPONENT */
    if(hzr_parms_ge_estim())
      hzr_parm_set_fixed(HZ_ETA);

    if(hzr_parm_eta_fixed()) {
      Late.gamma = HZRstr.l.gamma*HZRstr.l.eta;
      if(Late.gamma<=ZERO)
	Late.gamma = HZRstr.l.eta;
      Late.eta = ONE;
    } else {
      Late.eta = HZRstr.l.gamma*HZRstr.l.eta;
      if(Late.eta<=ZERO)
	Late.eta = HZRstr.l.gamma;
      Late.gamma = ONE;
    }
  }
  if(HZRstr.l.gamma!=Late.gamma)
    hzr_parm_changed(HZ_GAMMA);
  if(HZRstr.l.eta!=Late.eta)
    hzr_parm_changed(HZ_ETA);
}

void SETG3_weibull(void){
  Late.g3flag = Late.g3flag+2;
  if(Late.gamma<=ZERO) {
    hzr_set_parm_err("(SETG3960)",HZ_GAMMA);
    return;
  }
  if(Late.eta<=ZERO) {
    hzr_set_parm_err("(SETG3970)",HZ_ETA);
    return;
  }
  if(Late.alpha<ZERO || (Late.alpha==ZERO && Late.g3flag==3)) {
    hzr_set_parm_err("(SETG3980)",HZ_ALPHA);
    return;
  }
  gte = Late.gamma*Late.eta;

  /* GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO */
  if(Late.g_two) {
    /**
       CHECK TO SEE IF GAMMA*ETA=2 FROM INPUT DATA.  IF NOT, CHANGE
       THE ONE WHICH IS NOT FIXED.
    **/
    if(gte!=TWO) {
      /* IF NEITHER CAN BE CHANGED, THIS IS AN ERROR */
      if(hzr_parms_ge_fixed()) {
	hzr_set_parm_err("(SETG3990)",HZ_GAMMA);
	hzr_set_parm_err("(SETG3990)",HZ_ETA);
	return;
      }
      if(hzr_parm_gamma_fixed()) {
	Late.eta = TWO/Late.gamma;
	if(HZRstr.l.eta!=Late.eta)
	  hzr_parm_changed(HZ_ETA);
      } else {
	Late.gamma = TWO/Late.eta;
	if(HZRstr.l.gamma!=Late.gamma)
	  hzr_parm_changed(HZ_GAMMA);
      }
    }
    /* IF EITHER ETA OR GAMMA IS FIXED, THE OTHER MUST BE */
    if(hzr_parms_ge_1fixed()) {
      hzr_parm_set_fixed(HZ_GAMMA);
      hzr_parm_set_fixed(HZ_ETA);
      /* IF BOTH ETA AND GAMMA ARE FREE, ARBITRARILY FIX ETA */ 
    } else if(hzr_parms_ge_estim())
      hzr_parm_set_fixed(HZ_ETA);
  }

  /**
     NOW TAKE CARE OF ALPHA.  REMEMBER WE HAVE ALREADY SETTLED THE
     MATTER OF ALPHA IF BOTH GAMMA*ETA=2 AND GAMMA*ETA/ALPHA=2.
  **/
  if(Late.g3flag==3)
    SETG3_alpha_fixup(hzr_parms_ge_1estim);
  return;
}

void SETG3_all_gt_0(void)
     /**
    *           ALPHA>0, GAMMA>0, ETA>0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ----------------------------------------------------
    *
    *     >0    >0    >0      F      F       1       GENERIC
    *     >0    >0    >0      T      F       1       GENERIC
    *     >0    >0    >0      F      T       1       GENERIC
    *     >0    >0    >0      T      T       1       GENERIC
    **/
{
  gte = Late.gamma*Late.eta;

  SETG3_verify_ge_2();

  /**
     NOW TAKE CARE OF ALPHA.  REMEMBER WE HAVE ALREADY SETTLED THE
     MATTER OF ALPHA IF BOTH GAMMA*ETA=2 AND GAMMA*ETA/ALPHA=2.
  **/
  SETG3_alpha_fixup(hzr_parms_ge_1estim);
}

void SETG3_alpha_le_0(void)
     /**
    *           ALPHA<=0, GAMMA>0, ETA>0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ------------------------------------------------------
    *
    *    <=0    >0    >0      F      F       2     EXPONENTIAL
    *    <=0    >0    >0      T      F       2     EXPONENTIAL
    *    <=0    >0    >0      F      T       1       GENERIC
    *    <=0    >0    >0      T      T       1       GENERIC
    **/
{
  gte = Late.gamma*Late.eta;

  SETG3_verify_ge_2();

  /**
     NOW TAKE CARE OF ALPHA.  IF THE EXPONENTIAL CASE HAS NOT BEEN
     SELECTED (ALPHA NOT FIXED AT ZERO) THEN WE ASSUME THE GENERIC
     MODEL IS DESIRED AND THAT ALPHA MUST BE SET TO AN APPROPRIATE
     VALUE.
  **/
  if(Late.g3flag==1)
    SETG3_alpha_gener(hzr_parms_ge_fixed);
}

void SETG3_gamma_le_0(void)
     /**
    *           ALPHA>0, GAMMA<=0, ETA>0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ----------------------------------------------------
    *
    *     >0   <=0    >0      F      F       1       GENERIC
    *     >0   <=0    >0      T      F       1       GENERIC
    *     >0   <=0    >0      F      T       1       GENERIC
    *     >0   <=0    >0      T      T       1       GENERIC
    **/
{
  if(hzr_parm_gamma_fixed()) {
    hzr_set_parm_err("(SETG31090)",HZ_GAMMA);
    return;
  }
  hzr_parm_changed(HZ_GAMMA);

  /**
     SINCE GAMMA IS MISSING, BUT ALPHA IS AVAILABLE, WE WILL
     TENTATIVELY USE IT TO OBTAIN A GAMMA
  **/
  if(Late.ga_two)
    Late.gamma = TWO*Late.alpha/Late.eta;
  else
    Late.gamma = 3.0e0*Late.alpha/Late.eta;
  gte = Late.gamma*Late.eta;

  /**
     GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO
     AND SINCE ONLY ETA HAS BEEN GIVEN WE WILL FAVOR IT.
  **/
  if(Late.g_two) {
    hzr_parm_set_fixed(HZ_GAMMA);
    /**
       CHECK TO SEE IF GAMMA*ETA=2 FROM THE ABOVE.
       IF NOT, CHANGE GAMMA.
    **/
    if(gte!=TWO)
      Late.gamma = Late.eta/TWO;
  } else
    /* GAMMA*ETA>2, SO CHECK ON THIS */
    if(gte<=TWO)
      Late.gamma = 3.0e0/Late.eta;

  /**
     NOW TAKE CARE OF ALPHA.  REMEMBER WE HAVE ALREADY SETTLED THE
     MATTER OF ALPHA IF BOTH GAMMA*ETA=2 AND GAMMA*ETA/ALPHA=2.
  **/
  SETG3_alpha_fixup(hzr_parm_eta_fixed);
}

void SETG3_eta_le_0(void)
     /**
    *           ALPHA>0, GAMMA>0, ETA<=0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ----------------------------------------------------
    *
    *     >0    >0   <=0      F      F       1       GENERIC
    *     >0    >0   <=0      T      F       1       GENERIC
    *     >0    >0   <=0      F      T       1       GENERIC
    *     >0    >0   <=0      T      T       1       GENERIC
    **/
{
  if(hzr_parm_eta_fixed()) {
    hzr_set_parm_err("(SETG32020)",HZ_ETA);
    return;
  }
  hzr_parm_changed(HZ_ETA);

  /* SINCE WE HAVE NO ETA, GET ONE FROM GAMMA AND ALPHA */
  if(Late.ga_two)
    Late.eta = TWO*Late.alpha/Late.gamma;
  else
    Late.eta = 3.0e0*Late.alpha/Late.gamma;
  gte = Late.gamma*Late.eta;

  /**
     GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO.
     SINCE ETA HAS NOT BEEN SPECIFIED, WE WILL NOT ESTIMATE IT.
  **/
  if(Late.g_two) {
    hzr_parm_set_fixed(HZ_ETA);
    /**
       CHECK TO SEE IF GAMMA*ETA=2 FROM THE ABOVE.
       IF NOT, CHANGE ETA.
    **/
    if(gte!=TWO)
      Late.eta = Late.gamma/TWO;
  } else
    /* GAMMA*ETA>2, SO CHECK ON THIS */
    if(gte<=TWO)
      Late.eta = 3.0e0/Late.gamma;

  /**
     NOW TAKE CARE OF ALPHA.  REMEMBER WE HAVE ALREADY SETTLED THE
     MATTER OF ALPHA IF BOTH GAMMA*ETA=2 AND GAMMA*ETA/ALPHA=2.
  **/
  SETG3_alpha_fixup(hzr_parm_gamma_fixed);
}

void SETG3_eta_gt_0(void)
     /**
    *           ALPHA<=0, GAMMA<=0, ETA>0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ------------------------------------------------------
    *
    *    <=0   <=0    >0      F      F       2     EXPONENTIAL
    *    <=0   <=0    >0      T      F       2     EXPONENTIAL
    *    <=0   <=0    >0      F      T       1       GENERIC
    *    <=0   <=0    >0      T      T       1       GENERIC
    **/
{
  if(hzr_parm_gamma_fixed()) {
    hzr_set_parm_err("(SETG32050)",HZ_GAMMA);
    return;
  }
  hzr_parm_changed(HZ_GAMMA);

  /**
     GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO
     WE WILL FIX GAMMA RATHER THAN ETA SINCE WE HAVE SOME DATA ON THIS
     PARAMETER
  **/
  if(Late.g_two) {
    hzr_parm_set_fixed(HZ_GAMMA);
    Late.gamma = TWO/Late.eta;
  } else
    Late.gamma = 3.0e0/Late.eta;

  /**
     NOW TAKE CARE OF ALPHA.  IF THE EXPONENTIAL CASE HAS NOT BEEN
     SELECTED (ALPHA NOT FIXED AT ZERO) THEN WE ASSUME THE GENERIC
     MODEL IS DESIRED AND THAT ALPHA MUST BE SET TO AN APPROPRIATE
     VALUE.
  **/
  if(Late.g3flag==1)
    SETG3_alpha_gener(hzr_parm_eta_fixed);
}

void SETG3_gamma_gt_0(void)
     /**
    *           ALPHA<=0, GAMMA>0, ETA<=0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ------------------------------------------------------
    *
    *    <=0    >0   <=0      F      F       2     EXPONENTIAL
    *    <=0    >0   <=0      T      F       2     EXPONENTIAL
    *    <=0    >0   <=0      F      T       1       GENERIC
    *    <=0    >0   <=0      T      T       1       GENERIC
    **/
{
  if(hzr_parm_eta_fixed()) {
    hzr_set_parm_err("(SETG32080)",HZ_ETA);
    return;
  }
  hzr_parm_changed(HZ_ETA);

  /**
     GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO
     WE WILL FIX ETA SINCE WE DO HAVE SOME INFORMATION ON GAMMA.
  **/
  if(Late.g_two) {
    hzr_parm_set_fixed(HZ_ETA);
    Late.eta = TWO/Late.gamma;
  } else
    Late.eta = 3.0e0/Late.gamma;

  /**
     NOW TAKE CARE OF ALPHA.  IF THE EXPONENTIAL CASE HAS NOT BEEN
     SELECTED (ALPHA NOT FIXED AT ZERO) THEN WE ASSUME THE GENERIC
     MODEL IS DESIRED AND THAT ALPHA MUST BE SET TO AN APPROPRIATE
     VALUE.
  **/
  if(Late.g3flag==1)
    SETG3_alpha_gener(hzr_parm_gamma_fixed);
}

void SETG3_alpha_gt_0(void)
     /**
    *           ALPHA>0, GAMMA<=0, ETA<=0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ----------------------------------------------------
    *
    *     >0   <=0   <=0      F      F       1       GENERIC
    *     >0   <=0   <=0      T      F       1       GENERIC
    *     >0   <=0   <=0      F      T       1       GENERIC
    *     >0   <=0   <=0      T      T       1       GENERIC
    **/
{
  if(hzr_parm_gamma_fixed()) {
    hzr_set_parm_err("(SETG33020)",HZ_GAMMA);
    return;
  }
  if(hzr_parm_eta_fixed()) {
    hzr_set_parm_err("(SETG33010)",HZ_ETA);
    return;
  }
  hzr_parm_changed(HZ_GAMMA);
  hzr_parm_changed(HZ_ETA);

  /* WE WILL ARBITRARILY SET ETA TO 2.0 */
  Late.eta = TWO;

  if(Late.ga_two)
    /* WILL RESULT IN GAMMA*ETA/ALPHA=2. */
    Late.gamma = Late.alpha;
  else
    /* MAKES GAMMA*ETA/ALPHA>2. */
    Late.gamma = 1.5e0*Late.alpha;
  gte = Late.gamma*Late.eta;

  /* GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO */
  if(Late.g_two) {
    hzr_parm_set_fixed(HZ_ETA);
    /**
       CHECK TO SEE IF GAMMA*ETA=2 FROM THE ABOVE.
       IF NOT, CHANGE GAMMA.
    **/
    if(gte!=TWO)
      Late.gamma = TWO/Late.eta;
  } else
    /* GAMMA*ETA>2, SO CHECK ON THIS */
    if(gte<=TWO)
      Late.gamma = 3.0e0/Late.eta;

  /**
     NOW TAKE CARE OF ALPHA.  REMEMBER WE HAVE ALREADY SETTLED THE
     MATTER OF ALPHA IF BOTH GAMMA*ETA=2 AND GAMMA*ETA/ALPHA=2.
  **/
  SETG3_alpha_fixup(hzr_returns_TRUE);
}

void SETG3_all_le_0(void)
     /**
    *           ALPHA<=0, GAMMA<=0, ETA<=0
    *
    *   ALPHA GAMMA  ETA    GETWO GAETWO  G3FLAG      MODEL
    *   ------------------------------------------------------
    *
    *    <=0   <=0   <=0      F      F       2     EXPONENTIAL
    *    <=0   <=0   <=0      T      F       2     EXPONENTIAL
    *    <=0   <=0   <=0      F      T       1       GENERIC
    *    <=0   <=0   <=0      T      T       1       GENERIC
    **/
{
  if(hzr_parm_gamma_fixed()) {
    hzr_set_parm_err("(SETG33020)",HZ_GAMMA);
    return;
  }
  if(hzr_parm_eta_fixed()) {
    hzr_set_parm_err("(SETG33010)",HZ_ETA);
    return;
  }
  hzr_parm_changed(HZ_GAMMA);
  hzr_parm_changed(HZ_ETA);

  Late.gamma = ONE;
  /* GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO */
  if(Late.g_two) {
    hzr_parm_set_fixed(HZ_ETA);
    Late.eta = TWO;
  } else
    Late.eta = 3.0e0;

  /**
     NOW TAKE CARE OF ALPHA.  IF THE EXPONENTIAL CASE HAS NOT BEEN
     SELECTED (ALPHA NOT FIXED AT ZERO) THEN WE ASSUME THE GENERIC
     MODEL IS DESIRED AND THAT ALPHA MUST BE SET TO AN APPROPRIATE
     VALUE.
  **/
  if(Late.g3flag==1)
    SETG3_alpha_gener(hzr_returns_TRUE);
}

void SETG3_alpha_fixup(logical (*parm_fixed_func)(void)){
  gteva = Late.gamma*Late.eta/Late.alpha;
  if(Late.ga_two) {
    if(!Late.g_two && gteva!=TWO) {
      /**
	 HERE WE WILL HAVE TO CHANGE ALPHA TO MAKE THE CALCULATIONS
	 MAKE SENSE.  IF ALPHA IS FIXED WE ARE IN TROUBLE.
      **/
      if(hzr_parm_is_fixed(HZ_ALPHA)) {
	hzr_set_parm_err("(SETG31000)",HZ_ALPHA);
	return;
      }
      Late.alpha = Late.gamma*Late.eta/TWO;
      if(HZRstr.l.alpha!=Late.alpha)
	hzr_parm_changed(HZ_ALPHA);
    }
    /* IF GAMMA*ETA/ALPHA=2, THEN WE CAN FREE UP ONE PARAMETER */
    if((*parm_fixed_func)())
      hzr_parm_set_fixed(HZ_ALPHA);
    return;
  } else
    if(gteva>TWO || HZRstr.weibul)
      return;

  /**
     GAMMA*ETA/ALPHA>2, SO IF THIS IS NOT SO AND ALPHA IS FIXED, WE
     COULD GO BACK AND JUGGLE WITH GAMMA AND ETA, CHECKING ON VARIOUS
     FIX OPTIONS, BUT FOR THIS VERSION, THIS IS SIMPLY FLAGGED AS AN
     ERROR.
  **/
  if(hzr_parm_is_fixed(HZ_ALPHA)) {
    hzr_set_parm_err("(SETG31040)",HZ_ALPHA);
    return;
  }
  Late.alpha = Late.gamma*Late.eta/3.0e0;
  if(HZRstr.l.alpha!=Late.alpha)
    hzr_parm_changed(HZ_ALPHA);
}

void SETG3_alpha_gener(logical (*parm_fixed_func)(void)){
  /**
     HERE WE WILL HAVE TO CHANGE ALPHA TO MAKE THE CALCULATIONS MAKE
     SENSE.  IF ALPHA IS FIXED WE ARE IN TROUBLE.
   **/
  if(hzr_parm_is_fixed(HZ_ALPHA)) {
    hzr_set_parm_err("(SETG31070)",HZ_ALPHA);
    return;
  }
  if(Late.ga_two) {
    Late.alpha = Late.gamma*Late.eta/TWO;
    /* IF GAMMA*ETA/ALPHA=2, THEN WE CAN FREE UP ONE PARAMETER */
    if((*parm_fixed_func)())
      hzr_parm_set_fixed(HZ_ALPHA);
  } else
    Late.alpha = Late.gamma*Late.eta/3.0e0;
  hzr_parm_changed(HZ_ALPHA);
}

void SETG3_verify_ge_2(void){
  /**
     GAMMA*ETA=2, SO AT MOST WE NEED CALCULATE ONLY ONE OF THE TWO
  **/
  if(Late.g_two) {

    /**
       CHECK TO SEE IF GAMMA*ETA=2 FROM INPUT DATA.  IF NOT, CHANGE
       THE ONE WHICH IS NOT FIXED.
    **/
    if(gte!=TWO) {
      /* IF NEITHER CAN BE CHANGED, THIS IS AN ERROR */
      if(hzr_parms_ge_fixed()) {
	hzr_set_parm_err("(SETG31010)",HZ_GAMMA);
	hzr_set_parm_err("(SETG31010)",HZ_ETA);
	return;
      }
      if(hzr_parm_gamma_fixed()) {
	Late.eta = TWO/Late.gamma;
	if(HZRstr.l.eta!=Late.eta)
	  hzr_parm_changed(HZ_ETA);
      } else {
	Late.gamma = TWO/Late.eta;
	if(HZRstr.l.gamma!=Late.gamma)
	  hzr_parm_changed(HZ_GAMMA);
      }
    }
    /* IF EITHER ETA OR GAMMA IS FIXED, THE OTHER MUST BE */
    if(hzr_parms_ge_1fixed()) {
      hzr_parm_set_fixed(HZ_GAMMA);
      hzr_parm_set_fixed(HZ_ETA);
      /* IF BOTH ETA AND GAMMA ARE FREE, ARBITRARILY FIX ETA */ 
    } else if(hzr_parms_ge_estim())
      hzr_parm_set_fixed(HZ_ETA);
  } else if(gte<=TWO) {
    /* GAMMA*ETA<2, SO CHECK ON THIS */
    if(hzr_parms_ge_fixed()) {
      hzr_set_parm_err("(SETG31020)",HZ_GAMMA);
      hzr_set_parm_err("(SETG31020)",HZ_ETA);
      return;
    }
    if(hzr_parm_gamma_fixed()) {
      Late.eta = 3.0e0/Late.gamma;
      if(HZRstr.l.eta!=Late.eta)
	hzr_parm_changed(HZ_ETA);
    } else {
      Late.gamma = 3.0e0/Late.eta;
      if(HZRstr.l.gamma!=Late.gamma)
	hzr_parm_changed(HZ_GAMMA);
    }
  }
}

logical hzr_parms_ge_fixed(void){
  return hzr_parm_gamma_fixed() && hzr_parm_eta_fixed();
}

logical hzr_parms_ge_1fixed(void){
  return hzr_parm_gamma_fixed() || hzr_parm_eta_fixed();
}

logical hzr_parm_gamma_fixed(void){
  return hzr_parm_is_fixed(HZ_GAMMA);
}

logical hzr_parm_eta_fixed(void){
  return hzr_parm_is_fixed(HZ_ETA);
}

logical hzr_returns_TRUE(void){
  return TRUE;
}

logical hzr_parms_ge_estim(void){
  return !hzr_parm_gamma_fixed() && !hzr_parm_eta_fixed();
}

logical hzr_parms_ge_1estim(void){
  return !hzr_parm_gamma_fixed() || !hzr_parm_eta_fixed();
}
