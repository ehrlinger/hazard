#include <common.h>
#include <hzdinc.h>

#include <hzr_parm.h>
#include <hzr_set_parm_err.h>
#include <hzd_set_rho.h>

void SETG1_mNuOne_TRUE(void);
void SETG1_mNuOne_FALSE(void);
logical hzr_parms_m_nu_fixed(void);
logical hzr_parms_m_nu_estim(void);

void SETG1(void)

     /***********************************************************************

   SETUP OF EARLY PHASE HAZARD

   PURPOSE
     TO CHECK VALIDITY OF EARLY PHASE SHAPING PARAMETERS AND TO SET
     FLAG FOR CALCULATION OF THE SPECIFIC FORM FOR THE EARLY PHASE.

     EARLY PHASE MODEL IS BASED ON THE COMPLETE UNBOUNDED MODEL
     DEVELOPED BY TURNER, ET AL (SEE REFERENCE BELOW) WHICH WAS
     PRESENTED AS A MODEL OF MORTALITY, Q(T):

               _                                       _
              |          _                        _     |
              |         |      _            _      |-1/N|
              |         |     |              |-1/P |    |
              ||P|-P  P |   N ||N|-N         |     |    |
       Q(T) = |-----+---|1+---|-----+|NP|B(T)|     |    |
              |2|P|  |P||  |N|| 2|N|         |     |    |
              |         |     |_            _|     |    |
              |         |_                        _|    |
              |_                                       _|

                 _                 _
                |  DELTA*T          |
       B(T)=BETA|(E       - 1)/DELTA|
                |_                 _|


     IN THE FORMULATION PROPOSED BY BLACKSTONE, NAFTEL, AND TURNER
     Q(T) IS SCALED BY MU AND REEXPRESSED AS A MODEL OF BOUNDED
     (BY MU) CUMULATIVE HAZARD, SINCE Q(INFINITY)=1.

     IN THIS REEXPRESSION, THE FOLLOWING NOTATION IS USED:

       EARLY PHASE CUMULATIVE HAZARD = MUE*Q(T) = MUE*G1

       RHO = 1/(BETA|NP|)      _                 _
                              |  DELTA*T          |
       B(T) IS REEXPRESSED AS |(E       - 1)/DELTA|
                              |_                 _|
       NU = P

       M = N

       THALF = ESTIMATED EXPRESSION OF RHO DERIVED BY SETTING EARLY
               CUMULATIVE HAZARD TO 1/2

     THE GENERAL MODEL CAN BE EXPRESSED, WITH THE NOTATION AS ABOVE,
     IN TERMS OF THREE GENERIC MODELS, EACH WITH A LIMITING
     EXPONENTIAL CASE:

       M>0, NU>0  THE NEGATIVE GENERIC MODEL.
              _           _
             |   _  _      |-1/M
             |  |    |-1/NU|
             |  |B(T)|     |
          G1=|1+|----|     |
             |  |RHO |     |
             |  |_  _|     |
             |_           _|

       LIMITING CASE 1:  M=0, NU>0
                 _          _
                |  _  _      |
                | |    |-1/NU|
                | |B(T)|     |
          G1=EXP|-|----|     |
                | |RHO |     |
                | |_  _|     |
                |_          _|


       M<0, NU>0  THE POSITIVE GENERIC MODEL
              _             _
             |   _    _      |-1/M
             |  |      |-1/NU|
             |  |  B(T)|     |
          G1=|1-|1+----|     |
             |  |  RHO |     |
             |  |_    _|     |
             |_             _|

       LIMITING CASE 2:   M<0, NU=0
              _           _
             |      _    _ |-1/M
             |     |      ||
             |     |  B(T)||
          G1=|1-EXP|- ----||
             |     |  RHO ||
             |     |_    _||
             |_           _|


       M>0, NU<0  THE MIXED GENERIC CASE
                _           _
               |   _  _      |-1/M
               |  |    |-1/NU|
               |  |B(T)|     |
          G1=1-|1+|----|     |
               |  |RHO |     |
               |  |_  _|     |
               |_           _|

       LIMITING CASE 3:  M=0, NU<0
                   _          _
                  |  _  _      |
                  | |    |-1/NU|
                  | |B(T)|     |
          G1=1-EXP|-|----|     |
                  | |RHO |     |
                  | |_  _|     |
                  |_          _|


   DESCRIPTION OF PARAMETERS
     INPUT
       SMUE   - REAL*8  EARLY PHASE SCALING PARAMETER
       SDELTA - REAL*8  PARAMETER ON THE EXPONENTIAL TRANSFORM OF T
       STHALF - REAL*8  HALF-TIME OF EARLY CUMULATIVE HAZARD
       SNU    - REAL*8  PARAMETER RELATED TO G1 SHAPE NEAR T=0
       SM     - REAL*8  PARAMETER RELATED TO OVERALL SHAPE
       STATUS - INTEGER VECTOR OF ESTIMATION FLAGS
                        0 - PARAMETER IS NOT TO BE ESTIMATED (FIXED)
                        1 - PARAMETER IS TO BE ESTIMATED

     OUTPUT
       CHGFLG - CHARACTER*1 12-VECTOR WHERE * DENOTES CHANGE IN
                        PARAMETER VALUE FROM SPECIFICATION AS FOLLOWS:
                        1 - MUE
                        2 - DELTA
                        3 - THALF
                        5 - NU
                        6 - M
       MUE    - REAL*8  EARLY PHASE SCALING PARAMETER

   DESCRIPTION OF PARAMETERS IN COMMON BLOCKS /EARLY/ AND /ERROR/
     INPUT
       MNUONE - LOGICAL IF .TRUE. M*NU=1 OR -1

     OUTPUT
       DELTA  - REAL*8  PARAMETER ON THE EXPONENTIAL TRANSFORM OF T
       DELTA0 - LOGICAL .TRUE. IF DELTA IS FIXED AT ZERO
       DELTAN - LOGICAL .TRUE. IF DELTA IS NEGATIVE
       THALF  - REAL*8  HALF-TIME OF EARLY CUMULATIVE HAZARD
       NU     - REAL*8  PARAMETER RELATED TO G1 SHAPE NEAR T=0
       M      - REAL*8  PARAMETER RELATED TO OVERALL SHAPE
       G1FLAG - INTEGER FLAG TO SELECT SPECIAL CASES OF G1
                        1 - M>0, NU>0:  NEGATIVE GENERIC MODEL
                        2 - M=0, NU>0:  LIMITING CASE 1
                        3 - M<0, NU>0:  POSITIVE GENERIC MODEL
                        4 - M<0, NU=0:  LIMITING CASE 2
                        5 - M>0, NU<0:  MIXED GENERIC MODEL
                        6 - M=0, NU<0:  LIMITING CASE 3
       ERROR  - INTEGER ERROR FLAG
                        0 - NO ERROR DETECTED
                        1 - ERRORS DETECTED

       ERRFLG - CHARACTER*48 STRING *** DENOTING TERMINAL ERROR
                FOR FIXED INADMISSIBLE SPECIFIED VALUE
                OF THE PARTICULAR PARAMETER AS FOLLOWS:
                (01:12)- SUBROUTINE NAME AND CODE NUMBER UNIQUE TO
                         THAT SUBROUTINE TO FACILITATE ERROR TRACING
                (13:15)- MUE
                (16:18)- DELTA
                (19:21)- THALF
                (25:27)- NU
                (28:30)- M

   REMARKS
     1.  INPUT PARAMETERS ARE NOT CHANGED BY THE SUBPROGRAM, BUT THEIR
         OUTPUTTED VALUE MAY BE CHANGED IF THE MAGNITUDE DOES NOT MEET
         RESTRICTIONS NOTED BELOW UNDER METHOD (CHANGES ARE DENOTED
         BY THE CHGFLG).

     2.  IF A PARAMETER VALUE IS INADMISSIBLE, YET IS DESIGNATED
         AS A FIXED (NON-ESTIMATED) PARAMETER, THE ERROR FLAG
         IS SET WITHOUT CONTINUING TO PROCESS THE INFORMATION
         FURTHER, AND THE PARAMETER(S) ASSOCIATED WITH THE ERROR
         IS FLAGGED BY ERRFLG.

     3.  THE STATUS FLAG VECTOR IS ALTERED IF NECESSARY TO REFLECT
         THE LIMITING AND SPECIAL CASES IN WHICH THE RANK OF THE
         PARAMETER VECTOR TO BE ESTIMATED IS REDUCED.

     4.  THIS SUBROUTINE MUST BE CALLED BEFORE THE ROUTINES TO EVALUATE
         CUMULATIVE HAZARD AND HAZARD FUNCTIONS (SUBROUTINE DLG1)

   SUBPROGRAMS REQUIRED
     DG1RHO - CALCULATE MODEL PARAMETER RHO FROM COMPUTATIONAL
              PARAMETER THALF

   METHOD
     1.  FOR THE FORM OF MODEL AND LIMITING CASES, SEE:
           TURNER ME JR, HAZELRIG JB, BLACKSTONE EH:  BOUNDED SURVIVAL,
           MATHEMATICAL BIOSCIENCES 59:33-46,1982.

     2.  MANAGEMENT OF INPUT PARAMETER INFORMATION:
   ___________________________________________________________________

   INPUT SPECIFICATIONS       ACTION TAKEN FOR OUTPUT SPECIFICATION
   ____________________    ___________________________________________

    M    NU   M*NU=|1|?    PARAMETER VALUES ON OUTPUT    G1FLAG CHANGE
   ___________________________________________________________________

   <0    <0    .TRUE.      NU = -1/M, VALUE OF NU IGNORED   3     Y
   <0    =0    .TRUE.      NU = -1/M                        3     N
   <0    >0    .TRUE.      NU = -1/M, VALUE OF NU IGNORED   3     Y*

   =0    <0    .TRUE.      M = -1/NU                        5     N
   =0    =0    .TRUE.      NU = M = 1                       1     Y
   =0    >0    .TRUE.      M = 1/NU                         1     N

   >0    <0    .TRUE.      NU = -1/M, VALUE OF NU IGNORED   5     Y*
   >0    =0    .TRUE.      NU = 1/M                         1     N
   >0    >0    .TRUE.      NU = 1/M,  VALUE OF NU IGNORED   1     Y*

   <0    <0    .FALSE.     NU = -NU, M = -M (CHANGE SIGNS)  1     Y
   <0    =0    .FALSE.     SPECIAL CASE 2.                  4     N
   <0    >0    .FALSE.     OUTPUT=INPUT                     3     N

   =0    <0    .FALSE.     SPECIAL CASE 3.                  6     N
   =0    =0    .FALSE.     NU = M = 1 (NEGATIVE GENERIC)    1     Y
   =0    >0    .FALSE.     SPECIAL CASE 1.                  2     N

   >0    <0    .FALSE.     OUTPUT=INPUT                     5     N
   >0    =0    .FALSE.     M = -M, SELECT SPECIAL CASE 2.   4     Y
   >0    >0    .FALSE.     OUTPUT=INPUT                     1     N
   ___________________________________________________________________

     *but not if NU is input as exactly +/- 1/M

     NOTE:  IF NU IS FIXED, THEN CHANGES ARE MADE INSTEAD TO M

     NOTE:  IF NU AND M ARE SPECIFIED AS ZERO, AND EITHER ONE IS
            FIXED AT ZERO, THEN POSITIVE AND NEGATIVE GENERIC
            LIMITING CASES ARE INVOKED RESPECTIVELY, AND THE OTHER
            PARAMETER IS SET TO A DEFAULT VALUE

     IN ADDITION TO THE ABOVE:
       MUE   - IF MUE <= 0 IT IS SET TO 1 ON THE OUTPUT SPECIFICATION
       THALF - IF THALF <= 0 IT IS SET TO 1 ON THE OUTPUT SPECIFICATION

   IMPLEMENTATION
     E.H. BLACKSTONE,MD     SEPTEMBER 1985

   REVISIONS
     MARCH 28,1986 - ADD VARIABLE DIMENSIONED ARRAY 'STATUS' TO
                     CALLING ARGUMENTS
     JUNE 5, 1986  - CHANGED ERRFLG TO CHARACTER*48 AND ADDED
                     ERROR TRACING MESSAGE
     OCT 15, 1986  - ACCOMMODATE CHANGE OF DELTA TO BE CONSTRAINED
                     BETWEEN 0 AND 1
     JUL 24, 1992  - DELTA CONSTRAINED BETWEEN -1 AND +1
     Oct 1993      - Converted to HZRCOR C-version 4.0(0)
     NOV 2, 1994   - HANDLE FIXING OF M OR NU WHEN MNUONE IS TRUE

   VERIFICATION
     SEPTEMBER 30, 1985
     ALL POSSIBLE COMBINATIONS OF INPUT TESTED, WITH DESIRED OUTPUT

***********************************************************************/

{
  /* Initialize ... */

  Early.g1flag = 0;
  Early.delta = ZERO;
  Early.tHalf = ZERO;
  Early.rho = ZERO;
  Early.nu = ZERO;
  Early.m = ZERO;
  Common.errorno = 0;

  /* MUE must be positive, else it is set to 1.0 */

  Early.muE = HZRstr.e.muE;
  if(Early.muE<=ZERO) {
    Early.muE = ONE;
    hzr_parm_changed(HZ_MUE);
  }

  /**
     IF DELTA>0 IT IS TO BE ESTIMATED (CONSTRAIN BETWEEN 0 AND 1)
     <0 IT IS TO BE ESTIMATED (CONSTRAIN BETWEEN 0 AND -1)
     =0 THE LIMITING NONEXPONENTIAL CASES OF T ARE TO BE SELECTED
     AND DELTA IS FIXED.
  **/

  Early.delta = HZRstr.e.delta;
  Early.delta0 = TRUE;
  Early.deltaN = FALSE;
  if(Early.delta<ZERO) {
    if(Early.delta<-ONE){
      if(hzr_parm_is_fixed(HZ_DELTA)) {
	hzr_set_parm_err("(SETG1900)",HZ_DELTA);
	return;
      } else {
	Early.delta = -0.1e0;
	hzr_parm_changed(HZ_DELTA);
      }
    }
    Early.delta0 = FALSE;
    Early.deltaN = TRUE;
  }
  if(Early.delta==ZERO)
    hzr_parm_set_fixed(HZ_DELTA);
  if(Early.delta>ZERO){
    if(Early.delta>ONE){
      if(hzr_parm_is_fixed(HZ_DELTA)) {
	hzr_set_parm_err("(SETG1901)",HZ_DELTA);
	return;
      } else {
	Early.delta = 0.1e0;
	hzr_parm_changed(HZ_DELTA);
      }
    }
    Early.delta0 = FALSE;
  }

  /**
     THALF MUST BE POSITIVE, ELSE IT IS SET TO 1.0
     HOWEVER, IF IT HAS BEEN FIXED NEGATIVE IT IS IN ERROR.
  **/

  Early.tHalf = HZRstr.e.tHalf;
  if(Early.tHalf<=ZERO){
    if(hzr_parm_is_fixed(HZ_THALF)) {
      hzr_set_parm_err("(SETG1910)",HZ_THALF);
      return;
    } else {
      Early.tHalf = ONE;
      hzr_parm_changed(HZ_THALF);
    }
  }
  Early.m = HZRstr.e.m;
  Early.nu = HZRstr.e.nu;

  if(Early.mNuOne)
    SETG1_mNuOne_TRUE();
  else
    SETG1_mNuOne_FALSE();
  if(Common.errorno==0)
    hzd_set_rho(&Early);
}

void SETG1_mNuOne_TRUE(void){
  /**
     M<0, NU<0
  **/

  if(Early.m<ZERO && Early.nu<ZERO) {
    if(hzr_parms_m_nu_fixed()) {
      hzr_set_parm_err("(SETG1920)",HZ_NU); /* ERROR if both fixed */
      hzr_set_parm_err("(SETG1920)",HZ_M); /* negative...         */
      return;
    }
    /**
       IF NU FIXED, THEN SIGN OF M IS CHANGED, NEGATIVE GENERIC IS
       SELECTED, AND THAT VALUE BECOMES A FIXED ONE (SEE BELOW).
       OTHERWISE, WE ACCEPT M, FIX IT, AND CHANGE SIGN OF NU.
       THIS IS THE POSITIVE GENERIC MODEL.
    **/
    if(hzr_parm_is_fixed(HZ_NU)) {
      Early.m = -ONE/Early.nu;
      Early.g1flag = 5;
      hzr_parm_changed(HZ_M);
    } else {
      Early.nu = -ONE/Early.m;
      Early.g1flag = 3;
      hzr_parm_changed(HZ_NU);
    }
    /* M IS REMOVED FROM ESTIMATION LIST AND FIXM FIXES ALL */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    return;
  }

  /**
             M<0, NU=0
   **/

  if(Early.m<ZERO && Early.nu==ZERO) {
    /**
       Here, we will take M as 'given' and formulate NU according to
       M*NU=-1, even if NU is said to be fixed (conflict with MNUONE)
    **/
    Early.nu = -ONE/Early.m;
    Early.g1flag = 3;
    /**
       NU will then not be estimated.
       However, we will take it that if NU was asked to be fixed,
       we also fix M.
    **/
    if(hzr_parm_is_fixed(HZ_NU))
      hzr_parm_set_fixed(HZ_M);
    hzr_parm_set_fixed(HZ_NU);
    return;
  }

  /**
             M<0, NU>0
   **/

  if(Early.m<ZERO && Early.nu>ZERO) {
    Early.g1flag = 3;
    if(hzr_parm_is_fixed(HZ_NU)) {
      /**
	 If NU is fixed, the we alter M and that value becomes
	 a fixed one (see below).
      **/
      Early.m = -ONE/Early.nu;
      if(Early.m!=HZRstr.e.m)
	hzr_parm_changed(HZ_M);
    } else {
      /**
	 Otherwise NU is made to follow M, and then M is removed
	 from the estimation parameter list.
      **/
      Early.nu = -ONE/Early.m;
      if(Early.nu!=HZRstr.e.nu)
	hzr_parm_changed(HZ_NU);
    }
    /* Remove M from estimation list and check for any fixing */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    return;
  }

  /**
             M=0, NU<0
   **/

  if(Early.m==ZERO && Early.nu<ZERO) {
    /**
       We choose the mixed generic model, make M*NU=-1, and let
       MNUONE take precedence over FIXM, if it is specified.
    **/
    Early.m = -ONE/Early.nu;
    hzr_parm_changed(HZ_M);
    Early.g1flag = 5;
    /* ONE OF THE PARAMETERS IS REMOVED FROM ESTIMATION, NAMELY M. */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    return;
  }

  /**
             M=0, NU=0
   **/

  if(Early.m==ZERO && Early.nu==ZERO) {
    /* BOTH M AND NU CANNOT BE FIXED AT ZERO */
    if(hzr_parms_m_nu_fixed()) {
      hzr_set_parm_err("(SETG1930)",HZ_NU);
      hzr_set_parm_err("(SETG1930)",HZ_M);
      return;
    }
    /* WE NEED TO ESTIMATE ONLY ONE PARAMETER, AND THIS WILL BE NU */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    Early.m = ONE;
    Early.nu = ONE;
    /* WE PREFERENTIALLY CHOOSE THE NEGATIVE GENERIC MODEL */
    Early.g1flag = 1;
    hzr_parm_changed(HZ_NU);
    hzr_parm_changed(HZ_M);
    return;
  }

  /**
             M=0, NU>0
   **/

  if(Early.m==ZERO && Early.nu>ZERO) {
    /* M changed even if it is fixed, MNUONE taking precedence */
    Early.m = ONE/Early.nu;
    hzr_parm_changed(HZ_M);
    /* WE WILL CHOOSE THE NEGATIVE GENERIC MODEL */
    Early.g1flag = 1;
    /* ONLY ONE PARAMETER IS ESTIMATED AND WE LET THIS BE NU */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    return;
  }

  /**
             M>0, NU<0
   **/

  if(Early.m>ZERO && Early.nu<ZERO) {
    Early.g1flag = 5;
    /* IF NU IS FIXED, WE WILL SET M TO M*NU=-1 */
    if(hzr_parm_is_fixed(HZ_NU)) {
      Early.m = -ONE/Early.nu;
      if(Early.m!=HZRstr.e.m)
	hzr_parm_changed(HZ_M);
    } else {
      /* OTHERWISE, M IS USED TO CALCULATE NU, AND IS */
      /* REMOVED FROM ESTIMATION                      */
      Early.nu = -ONE/Early.m;
      if(Early.nu!=HZRstr.e.nu)
	hzr_parm_changed(HZ_NU);
    }
    /* Remove M from estimation here. */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    return;
  }

  /**
             M>0, NU=0
   **/

  if(Early.m>ZERO && Early.nu==ZERO) {
    /* WE CHOOSE THE NEGATIVE GENERIC MODEL */
    Early.nu = ONE/Early.m;
    hzr_parm_changed(HZ_NU);
    Early.g1flag = 1;
    /* AND WE REMOVE NU FROM ESTIMATION SINCE NO VALUE */
    /* WAS SUPPLIED FOR IT.                            */
    if(hzr_parm_is_fixed(HZ_NU))
      hzr_parm_set_fixed(HZ_M);
    hzr_parm_set_fixed(HZ_NU);
    return;
  }

  /**
             M>0, NU>0
   **/

  if(Early.m>ZERO && Early.nu>ZERO) {
    /* SELECT NEGATIVE GENERIC MODEL */
    Early.g1flag = 1;
    /* IF NU IS FIXED, THEN CALCULATE M FROM IT */
    if(hzr_parm_is_fixed(HZ_NU)) {
      Early.m = ONE/Early.nu;
      if(Early.m!=HZRstr.e.m)
	hzr_parm_changed(HZ_M);
    } else {
      /* OTHERWISE GET NU FROM M AND REMOVE M FROM ESTIMATION */
      Early.nu = ONE/Early.m;
      if(Early.nu!=HZRstr.e.nu)
	hzr_parm_changed(HZ_NU);
    }
    /* Here, M is removed but either FIXM or FIXNU fixes all. */
    if(hzr_parm_is_fixed(HZ_M))
      hzr_parm_set_fixed(HZ_NU);
    hzr_parm_set_fixed(HZ_M);
    return;
  }
}

void SETG1_mNuOne_FALSE(void){
  /**
     M<0, NU<0
  **/

  if(Early.m<ZERO && Early.nu<ZERO) {
    /**
       SINCE THIS MODEL IS NOT VALID, WE CHANGE SIGNS AND PRETEND
       IT IS THE NEGATIVE GENERIC MODEL.  SO, IF ITS PARAMETERS
       ARE BOTH FIXED, WE HAVE AN ERROR.  IF ONE OF THEM IS FIXED,
       WE CAN SELECT A MORE APPROPRIATE MODEL.
    **/
    if(hzr_parms_m_nu_fixed()) {
      hzr_set_parm_err("(SETG1940)",HZ_NU);
      hzr_set_parm_err("(SETG1940)",HZ_M);
      return;
    }
    /* IF NU IS FIXED NEGATIVE, THEN SELECT MIXED GENERIC CASE */
    if(hzr_parm_is_fixed(HZ_NU)) {
      Early.m = -Early.m;
      hzr_parm_changed(HZ_M);
      Early.g1flag = 5;
    } else if(hzr_parm_is_fixed(HZ_M)) {
      /* IF M IS FIXED NEGATIVE, THEN SELECT POSITIVE GENERIC CASE */
      Early.nu = -Early.nu;
      hzr_parm_changed(HZ_NU);
      Early.g1flag = 3;
    } else {
      /**
         OTHERWISE CHANGE BOTH SIGNS AND ESTIMATE NEGATIVE GENERIC MODEL
      **/
      Early.nu = -Early.nu;
      Early.m = -Early.m;
      Early.g1flag = 1;
      hzr_parm_changed(HZ_NU);
      hzr_parm_changed(HZ_M);
    }
    return;
  }

  /**
             M<0, NU=0
   **/

  if(Early.m<ZERO && Early.nu==ZERO) {
    /**
       THIS IS THE LIMITING POSITIVE GENERIC MODEL, NU FIXED AT ZERO

       NOTE:  IF M IS FIXED AND NU IS FREE, WE WILL CHOOSE THE
       POSITIVE GENERIC MODEL.
    **/
    if(!hzr_parm_is_fixed(HZ_NU) && hzr_parm_is_fixed(HZ_M)) {
      Early.g1flag = 3;
      Early.nu = ONE;
      hzr_parm_changed(HZ_NU);
    } else {
      hzr_parm_set_fixed(HZ_NU);
      Early.g1flag = 4;
    }
    return;
  }

  /**
             M<0, NU>0
   **/

  if(Early.m<ZERO && Early.nu>ZERO) {
    /* THIS IS SIMPLY THE POSITIVE GENERIC MODEL */
    Early.g1flag = 3;
    return;
  }

  /**
             M=0, NU<0
   **/

  if(Early.m==ZERO && Early.nu<ZERO) {
    /**
       THIS IS THE LIMITING CASE OF THE MIXED GENERIC MODEL AND M IS
       FIXED AT ZERO.

       NOTE:  IF NU IS FIXED AND M IS FREE, THEN WE WILL ASSUME THAT
       THE USER WANTED THE MIXED GENERIC CASE
    **/
    if(hzr_parm_is_fixed(HZ_NU) && !hzr_parm_is_fixed(HZ_M)) {
      Early.m = ONE;
      hzr_parm_changed(HZ_M);
      Early.g1flag = 5;
    } else {
      hzr_parm_set_fixed(HZ_M);
      Early.g1flag = 6;
    }
    return;
  }

  /**
             M=0, NU=0
   **/

  if(Early.m==ZERO && Early.nu==ZERO) {
    /**
       NO CLUE AS TO THE MODEL, SO IN GENERAL WE WILL CHOOSE THE
       NEGATIVE GENERIC CASE UNLESS ONE OF THE PARAMETERS IS
       SPECIFICALLY SET TO ZERO (BUT NOT BOTH).
    **/
    if(hzr_parms_m_nu_fixed()) {
      hzr_set_parm_err("(SETG1950)",HZ_NU);
      hzr_set_parm_err("(SETG1950)",HZ_M);
      return;
    }
    if(hzr_parms_m_nu_estim()) {
      Early.m = ONE;
      Early.nu = ONE;
      Early.g1flag = 1;
      hzr_parm_changed(HZ_NU);
      hzr_parm_changed(HZ_M);
    } else if(hzr_parm_is_fixed(HZ_NU)) {
      /**
         IF NU IS FIXED AT ZERO, THEN WE WILL CHOOSE THE LIMITING
         CASE OF THE POSITIVE GENERIC MODEL
      **/
      Early.g1flag = 4;
      Early.m = ONE;
      hzr_parm_changed(HZ_M);
    } else {
      /**
         BUT IF M IS FIXED AT ZERO, THEN WE CHOOSE THE LIMITING CASE
         OF THE NEGATIVE GENERIC MODEL
      **/
      Early.g1flag = 2;
      Early.nu = ONE;
      hzr_parm_changed(HZ_NU);
    }
    return;
  }

  /**
             M=0, NU>0
   **/

  if(Early.m==ZERO && Early.nu>ZERO) {
    /**
       THE LIMITING CASE OF THE NEGATIVE GENERIC MODEL (AND WE
       WILL MAKE SURE M IS FIXED AT ZERO).

       NOTE:  IF NU IS FIXED AND M IS FREE, WE WILL SELECT INSTEAD
       THE NEGATIVE GENERIC MODEL
    **/
    if(hzr_parm_is_fixed(HZ_NU) && !hzr_parm_is_fixed(HZ_M)) {
      Early.m = ONE;
      hzr_parm_changed(HZ_M);
      Early.g1flag = 1;
    } else {
      hzr_parm_set_fixed(HZ_M);
      Early.g1flag = 2;
    }
    return;
  }

  /**
             M>0, NU<0
   **/

  if(Early.m>ZERO && Early.nu<ZERO) {
    /* THIS IS SIMPLY THE MIXED GENERIC MODEL */
    Early.g1flag = 5;
    return;
  }

  /**
             M>0, NU=0
   **/

  if(Early.m>ZERO && Early.nu==ZERO) {
    /* THIS MODEL IS NOT VALID, SO IF PARAMETERS ARE FIXED IT IS */
    /* AN ERROR                                                  */
    if(hzr_parms_m_nu_fixed()) {
      hzr_set_parm_err("(SETG1960)",HZ_NU);
      hzr_set_parm_err("(SETG1960)",HZ_M);
      return;
    }
    /* IF M IS FIXED AND NU IS FREE, WE WILL SELECT THE NEGATIVE */
    /* GENERIC MODEL                                             */
    if(!hzr_parm_is_fixed(HZ_NU) && hzr_parm_is_fixed(HZ_M)) {
      Early.nu = ONE;
      hzr_parm_changed(HZ_NU);
      Early.g1flag = 1;
    } else {
      /* OTHERWISE, WE CHANGE THE SIGN OF M, FIX NU AT ZERO, AND */
      /* SELECT THE POSITIVE GENERIC LIMITING CASE               */
      hzr_parm_set_fixed(HZ_NU);
      Early.m = -Early.m;
      Early.g1flag = 4;
      hzr_parm_changed(HZ_M);
    }
    return;
  }

  /**
             M>0, NU>0
   **/

  if(Early.m>ZERO && Early.nu>ZERO) {
    /* THIS IS SIMPLY THE NEGATIVE GENERIC MODEL */
    Early.g1flag = 1;
    return;
  }
}

logical hzr_parms_m_nu_fixed(void){
  return hzr_parm_is_fixed(HZ_NU) && hzr_parm_is_fixed(HZ_M);
}

logical hzr_parms_m_nu_estim(void){
  return !hzr_parm_is_fixed(HZ_NU) && !hzr_parm_is_fixed(HZ_M);
}
