#include <math.h>

#include "common.h"

#include "setdll.h"
#include "uminck.h"
#include "setobj.h"
#include "fdgrad.h"
#include "umstp0.h"
#include "fdhess.h"
#include "chlskm.h"

#include "cndest.h"
#include "choslv.h"
#include "ihesfa.h"
#include "linesr.h"
#include "umstop.h"
#include "bfgsfa.h"

void setoptim(void)

     /***********************************************************************

    THIS ROUTINE IS THE MAIN DRIVER FOR AN UNCONSTRAINED OPTIMIZATION
    CODE BASED ON A QUASI-NEWTON APPROACH WITH BFGS FACTORED SECANT
    UPDATES OR PURE NEWTON STEPS AS DESCRIBED IN:

    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS", PRENTICE-HALL,
              1983

    THIS CODE AND ASSOCIATED SUBROUTINES ARE BASICALLY IMPLEMENTATIONS
    OF THE ALGORITHMS DESCRIBED IN APPENDIX A OF THE REFERENCE. AS
    FAR AS WAS POSSIBLE, ALL VARIABLES AND ROUTINES WERE NAMED IN
    ACCORDANCE WITH THE NOTATION USED THERE. THIS CODE CORRESPONDS TO
    ALGORITHM  D6.1.1  WITH GLOBAL=4,FACTSEC=.TRUE. OR  .FALSE. AND
    ANALHESS=.TRUE. OR .FALSE. THE CODE ACCEPTS TWO CHOICES FOR THE
    GRADIENT CORRESPONDING TO ANGRAD (ANALYTIC GRADIENT ) = .TRUE.
    OR .FALSE.



    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

         L().......A REAL WORK STORAGE ARRAY OF LENGTH AT LEAST N**2
                   WHERE N IS THE DIMENSION OF THE SPACE OF INDEP-
                   ENDENT VARIABLES X(N). L IS USED BY THE CODE TO
                   STORE THE FACTORED BFGS APPROXIMATION TO THE
                   HESSIAN MATRIX OF THE OBJECTIVE FUNCTION

         X0()......A REAL N-VECTOR USED TO PASS INITIAL ESTIMATES TO
                   THE ROUTINE. X0 MUST BE SPECIFIED BY THE USER

         XC()......A REAL WORK STORAGE ARRAY OF LENGTH AT LEAST N

         XPLUS()...A REAL N-VECTOR WHICH ON OUTPUT FROM THE ROUTINE
                   RETURNS THE BEST ESTIMATE OF THE MINIMIZER OF THE
                   OBJECTIVE FUNCTION

         GC()......A REAL WORK STORAGE ARRAY OF LENGTH AT LEAST N

         GPLUS()...A REAL N-VECTOR WHICH ON OUTPUT FROM THE ROUTINE
                   WITH IRET = 0 OR IRET >= 2 RETURNS THE GRADIENT
                   OR FINITE DIFFERENCE ESTIMATE OF THE GRADIENT AT
                   XPLUS

         S().......A REAL WORK STORAGE ARRAY OF LENGTH AT LEAST N

         Y().......A REAL WORK STORAGE ARRAY OF LENGTH AT LEAST N

         SX()......A REAL WORK STORAGE ARRAY OF LENGTH AT LEAST N

         TYPX()....REAL N-VECTOR WHICH THE USER CAN SPECIFY OR ALLOW
                   TO DEFAULT. THE I-TH COMPONENT OF TYPX IS A POSITIVE
                   SCALAR SPECIFYING THE TYPICAL MAGNITUDE OF X(I).
                   IT IS IMPORTANT TO SUPPLY VALUES OF TYPX WHEN THE
                   COMPONENTS OF X() ARE EXPECTED TO BE OF VERY DIFF-
                   ERENT SIZES. FOR EXAMPLE, IF

                   X(1) IS IN (-10**10,10**10),

                   X(2) IS IN (-10**2,-10**4), AND

                   X(3) IS IN (-6*10**-6,9*10**-6)

                   THEN AN APPROXIMATE CHOICE OF TYPX WOULD BE
                   TYPX = (10**10,10**3,7*10**-6)

                   IF THE USER WISHES TO USE THE DEFAULT VALUES,
                   TYPX(1) SHOULD BE SET TO -1.0

                   THE DEFAULT VALUE IS TYPX(I) = 1.0D0


         FPLUS.....A REAL SCALAR WHICH ON OUTPUT WITH IRET = 0 OR
                   IRET >= 2 CONTAINS THE VALUE OF THE OBJECTIVE
                   FUNCTION AT XPLUS

         TYPF......A POSTIVE REAL SCALAR ESTIMATING THE MAGNITUDE OF
                   THE OBJECTIVE FUNCTION, F(X) NEAR THE MINIMUM X*.
                   IT IS USED IN THE GRADIENT STOPPING CONDITION
                   ASSOCIATED WITH GRADTL (BELOW). IF TYPF IS TOO
                   LARGE THE ALGORITHM MAY TERMINATE PREMATURELY.
                   IN PARTICULAR, IF ABS(F(X0)) >> ABS(F(X*)), TYPF
                   NEEDS TO BE NEAR ABS(F(X*)). IF THE USER WISHES
                   TO USE THE DEFAULT, SET TYPF = -1.0

                   THE DEFAULT VALUE IS TYPF = 1.0D0

         STPTOL....ON INPUT, A POSITIVE SCALAR GIVING THE TOLERANCE
                   AT WHICH THE SCALED DISTANCE BETWEEN TWO SUCCESSIVE
                   ITERATES IS CONSIDERED CLOSE ENOUGH TO ZERO TO
                   TERMINATE THE ALGORITHM. THE ALGORITHM IS TERMINATED
                   IF,

                     MAX   ((ABS(XPLUS(I)-XC(I))/DENOM ) < = STPTOL
                   1<=I<=N

                   WHERE DENOM = MAX(ABS(XPLUS(I)),TYPX(I))

                   STPTOL SHOULD BE AT LEAST AS SMALL AS 10**-D WHERE
                   D IS THE NUMBER OF ACCURATE DIGITS THE USER DESIRES
                   IN THE SOLUTION X*. THE ALGORITHM MAY TERMINATE
                   PREMATURELY IF STPTOL IS TOO LARGE, ESPECIALLY WHEN
                   USING BFGS UPDATES. FOR EXAMPLE STPTOL=MACHEP**1/3
                   IS OFTEN TOO LARGE IN THIS CASE

                   SUGGESTED DEFAULT VALUE: STPTOL = MACHEP**(2/3)

                   TO FORCE THE DEFAULT, SET STPTOL = -1.0

         GRADTL....ON INPUT, A POSITIVE SCALAR GIVING THE TOLERANCE AT
                   WHICH THE SCALED GRADIENT IS CONSIDERED CLOSE
                   ENOUGH TO ZERO TO TERMINATE THE ALGORITHM. THE ALGO-
                   RITHM IS STOPPED IF

                     MAX  (NUMERATOR(I)/DENOMINATOR) < = GRADTL
                   1<=I<=N

                   WHERE NUMERATOR=GPLUS(I)*MAX(ABS(X(I)),TYPX(I))

                         DENOMINATOR=MAX(ABS(F),TYPF)

                   IF A FINITE DIFFERENCE GRADIENT IS USED THEN
                   SQRT(ETA) IS THE SMALLEST VALUE OF GRADTL FOR
                   WHICH THIS THE ABOVE CONDITION CAN BE SATISFIED

                   NOTE: THIS IS THE PRIMARY STOPPING CONDITION FOR
                   THE ALGORITHM AND HENCE GRADTL SHOULD REFLECT THE
                   USER'S IDEA OF WHAT CONTITUTES A SOLUTION OF THE
                   PROBLEM. SEE SECTION 7.2 OF REFERENCE

                   SUGGESTED DEFAULT VALUE: GRADTL = MACHEP**(1/3)

                   TO FORCE THE DEFAULT, SET GRADTL = -1.0

         MAXSTP....A POSITIVE SCALAR GIVING THE MAXIMUM ALLOWABLE
                   SCALED STEPLENGTH 2-NORM(DX*(XPLUS-XC)) AT ANY
                   ITERATION. MAXSTP IS USED TO PREVENT STEPS THAT
                   WOULD CAUSE THE OPTIMIZATION ALGORITHM TO OVERFLOW
                   OR LEAVE THE DOMAIN OF INTEREST, AS WELL AS TO
                   DETECT DIVERGENCE. IT SHOULD BE CHOSEN SMALL
                   ENOUGH TO PREVENT THE JUST MENTIONED PROBLEMS BUT
                   LARGER THAN ANY ANTICIPATED REASONABLE STEPSIZE.
                   THE ALGORITHM HALTS IF IT TAKES 5 CONSECUTIVE STEPS
                   OF LENGTH MAXSTP. IF THE DEFAULT VALUE IS DESIRED
                   SET MAXSTP = -1.0

                   DEFAULT VALUE IS:

                   MAXSTP = (10**3)*MAX(2-NORM(DX*X0),2-NORM(DX))

         FN........EXTERNAL USER SUPPLIED SUBROUTINE WHICH SUPPLIES
                   VALUES OF THE OBJECTIVE FUNCTION TO THE PROGRAM.
                   IT IS CALLED AS:
                                 CALL FN(X,F,N)
                   WHERE X() IS THE VECTOR OF VARIABLES OVER WHICH THE
                   OPTIMIZATION IS BEING PERFORMED, F IS THE VALUE
                   OF THE OBJECTIVE FUNCTION AT X() RETURNED BY THE
                   ROUTINE AND N IS THE NUMBER OF PARAMETERS BEING
                   OPTIMIZED.(IE, THE DIMENSION OF X() ).

         GRAD......EXTERNAL USER SUPPLIED SUBROUTINE USED WHEN ANGRAD
                   IS .TRUE. TO SUPPLY VALUES OF THE GRADIENT OF THE
                   OBJECTIVE FUNCTION. IT IS CALLED AS:
                                CALL GRAD(X,G,N)
                   WHERE X() IS THE VECTOR OF VARIABLES OVER WHICH THE
                   OPTIMIZATION IS BEING PERFORMED,G() IS THE GRADIENT
                   OF THE OBJECTIVE FUNCTION AT X() AND N IS THE
                   NUMBER OF PARAMETERS BEING OPTIMIZED.
                   IF NUMERICAL GRADIENTS ARE BEING USED A DUMMY
                   ROUTINE MUST BE SUPPLIED

         HESS......EXTERNAL USER SUPPLIED SUBROUTINE USED WHEN ANHESS
                   IS .TRUE. TO SUPPLY VALUES OF THE HESSIAN OF THE
                   OBJECTIVE FUNCTION. IT IS CALLED AS:
                                CALL HESS(L,X,N)
                   WHERE L IS AN N X N HESSIAN MATRIX TO BE RETURNED
                   BY THE ROUTINE. L SHOULD BE SPECIFIED WITHIN THE
                   ROUTINE BY THE STATEMENT
                       REAL*8 L(N,N)
                   X() IS THE VECTOR OF VARIABLES OVER WHICH THE
                   OPTIMIZATION IS BEING PERFORMED AND N IS THE NUMBER
                   OF PARAMETERS BEING OPTIMIZED.
                   IF FINITE DIFFERENCE HESSIAN IS BEING USED A DUMMY
                   ROUTINE MUST BE SUPPLIED.

         IRET......INTEGER RETURN CODE FLAG WITH THE FOLLOWING
                   MEANINGS:
                     IRET = 0, NORMAL RETURN. ESTIMATE RETURNED IN
                               XPLUS ASSUMED TO BE A MINIMUM. THE VALUE
                               OF THE OBJECTIVE FUNCTION AND THE GRAD-
                               IENT AT XPLUS ARE RETURNED RESPECTIVELY
                               IN FPLUS AND GPLUS
                     IRET = 1, TERMINAL ERROR, VALUE OF N PASSED TO
                               ROUTINE < 1
                     IRET = 2, THE INITIAL ESTIMATE X0 APPEARS TO BE A
                               CRITICAL POINT. IT IS POSSIBLE THAT X0
                               IS A MAXIMIZER OR A SADDLE POINT. X0
                               SHOULD BE PERTURBED AND THE ROUTINE
                               TRIED AGAIN
                     IRET = 3, SCALED DISTANCE BETWEEN THE LAST TWO
                               STEPS IS LESS THAN STPTOL. XPLUS MAY BE
                               AN APPROXIMATE MINIMIZER. IT IS ALSO
                               POSSIBLE THAT THE MINIMIZER IS MAKING
                               SLOW PROGRESS AND IS NOT NEAR A
                               MINIMIZER. IT IS ALSO POSSIBLE THAT
                               STPTOL IS TOO LARGE
                     IRET = 4, LAST GLOBAL STEP FAILED TO FIND A POINT
                               LOWER THAN THE CURRENT X, (XC); EITHER
                               XC IS AN APPROXIMATE MINIMIZER AND NO
                               MORE ACCURACY IS POSSIBLE OR AN
                               INACCURATELY CODED ANALYTIC GRADIENT
                               IS BEING USED OR THE FINITE DIFFERENCE
                               GRADIENT IS TOO INACCURATE OR STPTOL IS
                               TOO LARGE
                     IRET = 5, ITERATION LIMIT EXCEEDED. XPLUS,FPLUS
                               AND GPLUS CONTAIN RESPECTIVELY THE
                               CURRENT BEST ESTIMATES OF THE
                               MINIMIZER, THE OBJECTIVE FUNCTION AND
                               THE GRADIENT OF THE OBJECTIVE FUNCTION
                     IRET = 6, FIVE CONSECUTIVE STEPS OF LENGTH
                               MAXSTP HAVE BEEN TAKEN; EITHER F(X) IS
                               UNBOUNDED BELOW OR F(X) HAS A FINITE
                               ASYMPTOTE IN SOME DIRECTION OR
                               MAXSTP IS TOO SMALL
                     IRET = 7, THE PROBLEM HAS BECOME SO ILLCONDITIONED
                               THAT IT IS POSSIBLE THAT FEWER THAN TWO
                               RELIABLE DIGITS ARE BEING CALCULATED
                               IN OBTAINING THE DIRECTION OF SEARCH.
                               IF FINITE DIFFERENCE GRADIENTS AND/OR
                               FINITE DIFFERENCE HESSIANS ARE BEING
                               USED THE PROBLEM CAN POSSIBLY BE SOLVED
                               BY USING ANALYTIC DERIVATIVES. THIS
                               PROBLEM IS SYMPTOMATIC OF OVER
                               SPECIFICATION (PARAMETERIZATION) OF THE
                               MODEL AND SHOULD BE VIEWED WITH CONCERN.

         PRTCOD....AN INTEGER INPUT FLAG WHICH CONTROLS THE OUTPUT
                   PRINTED DURING THE COMPUTATION. IT HAS THE
                   FOLLOWING LEVELS AND MAGNITUDES:

                     PRTCOD  = 0, PRODUCE NO OUTPUT
                     PRTCOD >= 1, PRINT ITRCNT,XPLUS,FPLUS,GPLUS
                     PRTCOD  = 2, PRINT INITIAL VALUES OF ALL
                                  RELEVANT VARIABLES

         ITRLMT....A POSITIVE INTEGER SPECIFYING THE MAXIMUM NUMBER OF
                   ITERATIONS. TO OBTAIN DEFAULT VALUE SET = -1
                   DEFAULT VALUE: ITRLMT = 100

         N.........DIMENSION OF THE PROBLEM

         ANGRAD....LOGICAL VARIABLE WHICH IS USER SPECIFIED TO INDICATE
                   WHETHER ANALYTIC DERIVATIVES ARE TO BE USED FOR THE
                   GRADIENT CALCULATION

                   ANGRAD = .TRUE. WHEN ANALYTIC DERIVATIVES ARE TO BE
                                   USED
                   ANGRAD = .FALSE. WHEN FINITE DIFFERENCE DERIVATIVES
                                   ARE TO BE USED

          TRUHES...LOGICAL VARIABLE WHICH IS USED ON INPUT TO SPECIFY
                   WHETHER A    NEWTON     STEP IS TO BE USED FOR
                   THE FIRST STEP IN A FACTORED  BFGS    ALGORITHM
                   OR IF A SCALED STEEPEST DESCENT FIRST STEP SHOULD
                   BE TAKEN

                   TRUHES = .TRUE. WHEN WISH FIRST STEP TO BE TRUE
                                   NEWTON STEP
                   TRUHES = .FALSE.WHEN WISH FIRST STEP TO BE SCALED
                                   STEEPEST DESCENT

          NEWTON...LOGICAL VARIABLE WHICH IS USED TO CHOOSE THE
                   THE METHOD.
                   NEWTON = .TRUE. WHEN A NEWTON METHOD IS TO BE USED.
                                   SEE ANHESS BELOW FOR FURTHER
                                   IMPORTANT INFORMATION.
                   NEWTON = .FALSE.WHEN A FACTORED SECANT METHOD IS
                                   TO BE USED.

          ANHESS...LOGICAL VARIABLE WHICH IS USED WHEN NEWTON IS
                   .TRUE. OR WHEN NEWTON IS .FALSE. AND TRUHES IS
                   .TRUE. WHEN BOTH ARE FALSE, ANHESS IS NOT USED.
                   ANHESS = .TRUE. WHEN AN ANALYTIC HESSIAN IS TO BE
                            USED.
                   ANHESS = .FALSE.WHEN A FINITE DIFFERENCE HESSIAN
                            IS TO BE USED.

                   IHPASS = LOGICAL VARIABLE USED WHEN NEWTON IS .TRUE.
                            OR WHEN TRUHES IS .TRUE. TO INDICATE TO
                            THAT THE FACTORED FORM OF THE INITIAL
                            HESSIAN IS BEING PASSED IN L.
                            IHPASS = .TRUE. USER MUST SUPPLY THE
                                     CHOLESKY FACTORIZATION OF THE
                                     HESSIAN MATRIX AT X0() IN THE
                                     LOWER TRIANGULAR PORTION OF L(,).
                            IHPASS = .FALSE.THE ROUTINE CALCULATES THE
                                     HESSIAN AT X0() AND FACTORS WHEN
                                     APPROPRIATE. IF NEWTON AND TRUHES
                                     ARE BOTH FALSE THIS PARAMETER HAS
                                     NO EFFECT.


    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS", PRENTICE-HALL,
              1983
         GILL,MURRAY AND WRIGHT, "PRACICAL OPTIMIZATION", ACADEMIC
              PRESS, 1981


    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - NOVEMBER 1990

    FUNCTIONS/SUBROUTINES CALLED - MACHIN,FN,GRAD,HESS,UMINCK,FDHESS,
                                   FDGRAD,UMSTP0,CHLSKM,IHESFA,UMSTOP,
                                   CNDEST,CHOSLV,LINESR,BFGSFA,PRINTIT,
                                   DLOG10

***********************************************************************/

{
  long i;

  /**
     BEGIN INITIALIZATION SECTION: THIS INVOLVES THE CHECKING OF INPUT
     PARAMETERS AND SUPPLYING DEFAULT VALUES WHERE REQUIRED.  THE
     CALCULATION OF INITIAL VALUES OF THE OBJECTIVE FUNCTION AND THE
     GRADIENT AND CHECKING TO SEE IF THE INITIAL POINT X0 IS A
     CRITICAL POINT OF THE OBJECTIVE FUNCTION
  **/

  /* INITIALIZE VARIABLES */
  HZRstr.opt.iret = 0;
  HZRstr.opt.trmcod= 0;
  UMINCK();

  if(HZRstr.opt.trmcod) {
    /* RETURN DUE TO INITIAL DATA WRONG, N < 1 */
    HZRstr.opt.iret = 1;
    return;
  }
  DHAZRD(HZRstr.x0,&HZRstr.opt.fc,HZRstr.opt.N);
  if(Common.errorno)
    return;

  if(HZRstr.angrad)
    DAGRAD(HZRstr.gc,HZRstr.opt.N);  /* (X0,GC,N) */
  else
    FDGRAD(HZRstr.x0,HZRstr.opt.fc,HZRstr.sx,HZRstr.gc,
	   DHAZRD,HZRstr.opt.eta,HZRstr.opt.N);  /* (X0,FC,SX,GC,FN,ETA,N) */
  if(Common.errorno)
    return;

  /* CHECK FOR X0 BEING A CRITICAL POINT */
  UMSTP0(HZRstr.x0,HZRstr.opt.fc,HZRstr.gc,HZRstr.sx,HZRstr.opt.typf,
	 HZRstr.opt.gradtl,&HZRstr.opt.trmcod,&HZRstr.opt.consec,HZRstr.opt.N);

  if(HZRstr.opt.trmcod) {
    HZRstr.opt.iret = 2;
    for(i=0; i<HZRstr.opt.N; i++) {
      HZRstr.xplus[i] = HZRstr.x0[i];
      HZRstr.gplus[i] = HZRstr.gc[i];
    }
    HZRstr.opt.fplus = HZRstr.opt.fc;
  }

  /* INITIALIZE THE FACTORED HESSIAN MATRIX */
  if(HZRstr.newton || HZRstr.truhes) {
    if(!HZRstr.ihpass) {
      if(HZRstr.anhess)
	DAHESS(HZRstr.lvec,HZRstr.opt.N);
      else
	FDHESS(HZRstr.x0,HZRstr.sx,HZRstr.lvec,HZRstr.svec,
               HZRstr.yvec,HZRstr.opt.fc,DHAZRD,HZRstr.opt.N);
      if(Common.errorno)
	return;
      CHLSKM(HZRstr.lvec,HZRstr.svec,HZRstr.yvec,HZRstr.sx,
	     HZRstr.opt.N,0);
    }
  } else
    IHESFA(HZRstr.lvec,HZRstr.sx,HZRstr.opt.fc,HZRstr.opt.typf,
	   HZRstr.opt.N);

  for(i=0; i<HZRstr.opt.N; i++)
    HZRstr.xc[i] = HZRstr.x0[i];


  HZRstr.opt.itrcnt = 0;
  /*
    The following line of code
  */

  HZRstr.opt.fplus = HZRstr.opt.fc;
  /*
    follows the fortran code but is different 
    from Dr. Katholi's code
    It is used to provide a value for iteration 0 log 
    likelyhood 
  */


}

void optim(void)
     /* ENTER HERE FOR LOOPING THROUGH THE ITERATIONS */
{
  /*double *li;*/
  long i;
  /* int  l,j,k; */
  logical maxtkn;


  HZRstr.opt.itrcnt++;

  /**
     ESTIMATE CONDITION OF HESSIAN OR HESSIAN APPROXIMATION CHECK TO
     SEE IF THE PROBLEM IS ADEQUATELY CONDITIONED TO CONTINUE. IF NOT
     SET IRET=7 AND RETURN.
  **/

  /* THE LIMITS FOR RCONDN HERE APPEAR TO ACTUALLY BE 3-14 */
  CNDEST(HZRstr.lvec,HZRstr.yvec,HZRstr.svec,HZRstr.xplus,
	 &HZRstr.opt.rcondn,HZRstr.opt.N);

  HZRstr.opt.rcondn = log10(HZRstr.opt.rcondn);


  /* EXPLICITLY CHECK CONDITION CODE */
  /* the following code was embedded in an if clause to prevent */
  /* the premature exit from the routine during 1st iteration   */
  if(HZRstr.opt.itrcnt > 1){
    if(HZRstr.opt.condno>ZERO) {
      if(HZRstr.opt.rcondn>HZRstr.opt.condno)
	HZRstr.opt.iret = 7;
    } else {
      /* TAILOR CONDITION CHECKING TO ALGORITHMS */
      if(HZRstr.newton)
	if((!HZRstr.anhess && (-5.0e0+HZRstr.opt.rcondn)>-TWO) ||
	   (HZRstr.anhess && (-16.0e0+HZRstr.opt.rcondn)>-TWO))
	  HZRstr.opt.iret = 7;
      if((!HZRstr.angrad && (-8.0e0+HZRstr.opt.rcondn)>-TWO) ||
	 (HZRstr.angrad && (-16.0e0+HZRstr.opt.rcondn)>-TWO))
	HZRstr.opt.iret = 7;
    }
  }

  if(HZRstr.opt.iret) {
    for(i=0; i<HZRstr.opt.N; i++) {
      HZRstr.xplus[i] = HZRstr.xc[i];
      HZRstr.gplus[i] = HZRstr.gc[i];
    }
    return;
  }

  /* SOLVE (L)*(L-TRANSPOSE)*S=-GC FOR NEXT DIRECTION OF SEARCH */

  CHOSLV(HZRstr.lvec,HZRstr.gc,HZRstr.svec,HZRstr.opt.N);


  /**
     Experimentally, try resetting at each iteration (but before line
     search) the stopping conditions based on the current objective
     function and estimates.
  **/
  RECHK(HZRstr.xc,HZRstr.typx,HZRstr.sx,&HZRstr.opt.fc,&HZRstr.opt.maxstp,
	HZRstr.opt.N);


  LINESR(HZRstr.xc,HZRstr.xplus,HZRstr.gc,HZRstr.gplus,HZRstr.opt.fc,
	 &HZRstr.opt.fplus,HZRstr.svec,HZRstr.yvec,HZRstr.sx,
	 HZRstr.opt.maxstp,HZRstr.opt.stptol,DHAZRD,DAGRAD,HZRstr.opt.eta,
	 HZRstr.opt.N,&HZRstr.opt.iret,&maxtkn,HZRstr.angrad,HZRstr.newton);
  if(Common.errorno)
    return;


  UMSTOP(HZRstr.xc,HZRstr.xplus,HZRstr.gplus,HZRstr.sx,HZRstr.opt.fplus,
	 HZRstr.opt.typf,HZRstr.opt.gradtl,HZRstr.opt.stptol,HZRstr.opt.iret,
	 HZRstr.opt.itrcnt,HZRstr.opt.itrlmt,&HZRstr.opt.trmcod,
	 &HZRstr.opt.consec,maxtkn,HZRstr.opt.N);



  if(HZRstr.opt.trmcod) {
    if(HZRstr.opt.trmcod==1)
      HZRstr.opt.iret = 0;
    else
      HZRstr.opt.iret = 1+HZRstr.opt.trmcod;
    return;
  }


  /**
     ITERATION IS TO CONTINUE. UPDATE SECANT APPROX.  TO HESSIAN OR
     HESSIAN DEPENDING ON NEWTON
  **/
  if(HZRstr.newton) {
    if(HZRstr.anhess)
      DAHESS(HZRstr.lvec,HZRstr.opt.N);
    else
      FDHESS(HZRstr.xplus,HZRstr.sx,HZRstr.lvec,HZRstr.svec,
	     HZRstr.yvec,HZRstr.opt.fplus,DHAZRD,HZRstr.opt.N);
    if(Common.errorno)
      return;
    CHLSKM(HZRstr.lvec,HZRstr.svec,HZRstr.yvec,HZRstr.sx,
	   HZRstr.opt.N,0);
  } else{
    BFGSFA(HZRstr.lvec,HZRstr.gc,HZRstr.gplus,HZRstr.svec,HZRstr.yvec,
	   HZRstr.opt.eta,HZRstr.angrad,HZRstr.opt.N);
  }


  for(i=0; i<HZRstr.opt.N; i++) {
    HZRstr.xc[i] = HZRstr.xplus[i];
    HZRstr.gc[i] = HZRstr.gplus[i];
  }

  HZRstr.opt.fc = HZRstr.opt.fplus;

}
