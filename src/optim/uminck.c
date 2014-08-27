#include <math.h>

#include "common.h"
void UMINCK(void)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO CHECK THE INITIALIZATION OF
    VARIABLES USED BY THE ALGORITHM AND TO SUPPLY DEFAULT VALUES
    FOR THOSE WHICH ARE NOT USER SUPPLIED

    IN ADDITION, WE HAVE ADDED A REINITIALIZATION ENTRY IN ORDER TO
    UPDATE THE CONVERGENCE CRITERIA BY INFORMATION GAINED IN THE
    ITERATIVE PROCESS (5/91)

    THE ARGUMENTS OF THIS ROUTINE HAVE THE FOLLOWING MEANINGS:

         X0()......REAL N-VECTOR IN WHICH THE USER MUST SPECIFY THE
                   INITIAL VALUES OF THE INDEPENDENT VARIABLES

         TYPX()....REAL N-VECTOR WHICH THE USER CAN SPECIFY OR ALLOW
                   TO DEFAULT. THE I-TH COMPONENT OF TYPX IS A POSITIVE
                   SCALAR SPECIFYING THE TYPICAL MAGNITUDE OF X(I).
                   IF IS IMPORTANT TO SUPPLY VALUES OF TYPX WHEN THE
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


         SX()......REAL N-VECTOR WHICH ON OUTPUT FROM THIS ROUTINE
                   CONTAINS 1.0/TYPX(I), I=1,...,N

         MACHEP....MACHINE EPSILON, SUPPLIED TO THIS ROUTINE BY
                   SUBROUTINE MACHIN. MACHEP IS DEFINED TO BE THE
                   SMALLEST FLOATING POINT NUMBER SUCH THAT
                   1.0 + MACHEP > 1.0

         TYPF......A POSTIVE REAL SCALAR ESTIMATING THE MAGNITUDE OF
                   THE OBJECTIVE FUNCTION, F(X) NEAR THE MINIMUM X*.
                   IT IS USED IN THE GRADIENT STOPPING CONDITION
                   ASSOCIATED WITH GRADTL (BELOW). IF TYPF IS TOO
                   LARGE THE ALGORITHM MAY TERMINATE PREMATURELY.
                   IN PARTICULAR, IF ABS(F(X0)) >> ABS(F(X*)), TYPF
                   NEEDS TO BE NEAR ABS(F(X*)). IF THE USER WISHES
                   TO USE THE DEFAULT, SET TYPF = -1.0

                   THE DEFAULT VALUE IS TYPF = 1.0D0


         ETA.......REAL PARAMETER USED IN THE CODE TO SPECIFY THE REL-
                   ATIVE NOISE IN F(X). THE MAIN USE OF ETA IS IN THE
                   CALCULATION OF FINITE DIFFERENCE STEP SIZES. ETA IS
                   CALCULATED BY THIS ROUTINE FROM FDIGIT ACCORDING TO
                   THE FOLLOWING RULES:

                                 |MACHEP, IF FDIGIT = -1
                         ETA  =  |
                                 |MAX(MACHEP,10**-FDIGIT), OTHERWISE

                   IF FDIGIT IS NOT KNOWN, IT SHOULD BE ESTIMATED BY
                   ONE OF THE METHODS DISCUSSED IN GILL,MURRAY AND
                   WRIGHT (1981), PAGES 333-339


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
                   PROBLEM. SEE SECTION 7.2 OF REFERENCE BELOW

                   SUGGESTED DEFAULT VALUE: GRADTL = MACHEP**(1/3)

                   TO FORCE THE DEFAULT, SET GRADTL = -1.0


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

                   TO FORCE THE DEFAULT SET STPTOL = -1.0


         MAXSTP....A POSITIVE SCALAR GIVING THE MAXIMUM ALLOWABLE
                   SCALED STEPLENGTH 2-NORM(DX*(XPLUS-XC)) AT ANY
                   ITERATION. MAXSTP IS USED TO PREVENT STEPS THAT
                   WOULD CAUSE THE OPTIMIZATION ALGORITHM TO OVERFLOW
                   OR LEAVE THE DOMAIN OF INTEREST, AS WELL AS TO
                   DETECT DIVERGENCE. IT SHOULD BE CHOSEN SMALL
                   ENOUGH TO PREVENT THE JUST MENTIONED PROBLEMS BUT
                   LARGER THAN ANY ANTICIPATED REASONABLE STEPSIZE.
                   THE ALGORITHM HALTS IF IT TAKES 5 CONSECUTIVE STEPS
                   OF LENGTH MAXSTP. IF THE DEFAULT VALUE IS DESIRED,
                   SET MAXSTP = -1.0

                   DEFAULT VALUE IS:

                   MAXSTP = (10**3)*MAX(2-NORM(DX*X0),2-NORM(DX))


         FDIGIT....A POSITIVE INTEGER SPECIFYING THE NUMBER OF RELIABLE
                   BASE 10 DIGITS RETURNED BY THE OBJECTIVE FUNCTION
                   PROGRAM FN. FOR EXAMPLE, IF FN IS THE RESULT OF AN
                   ITERATIVE PROCEDURE EXPECTED TO PRODUCE 5 GOOD
                   DIGITS IN THE RESULT, FDIGITS SHOULD BE SET EQUAL
                   TO 5. IF FN IS EXPECTED TO PROVIDE WITHIN ONE OR
                   TWO OF THE FULL NUMBER OF SIGNIFICANT DIGITS
                   AVAILABLE ON THE HOST COMPUTER, FDIGIT SHOULD BE
                   SET TO -1. THIS CASE IS THE DEFAULT VALUE
                   IF FDIGIT IS NOT KNOWN, SET EQUAL TO -1

         N.........DIMENSION OF THE PROBLEM

         ITRLMT....A POSITIVE INTEGER SPECIFYING THE MAXIMUM NUMBER OF
                   ITERATIONS. TO OBTAIN DEFAULT VALUE SET = -1

                   DEFAULT VALUE: ITRLMT = 100


         PRTCOD....A POSITIVE INTEGER SPECIFYING THE LEVEL OF PRINTING
                   TO BE DONE IN THE COURSE OF THE OPTIMIZATION. THE
                   FOLLOWING ARE VALID CODES:

                     PRTCOD  = 0, PRODUCE NO OUTPUT
                     PRTCOD >= 1, PRINT ITRCNT,XPLUS,FPLUS,GPLUS
                     PRTCOD  = 2, SAME AS 1 BUT PRINT INITIAL VALUES
                                  OF ALL RELEVANT PARAMETERS

         TRMCOD....AN INTEGER VARIABLE USED AS A SUBROUTINE TERMIN-
                   ATION FLAG. FOR THIS ROUTINE IT HAS THE FOLLOWING
                   MEANINGS:

                    = 0, ALL INPUT SATISFACTORY OR CORRECTED BY ROUTINE
                    < 0, FATAL INPUT ERROR
                           = -1, INDICATES N < 1


         ANGRAD....LOGICAL VARIABLE WHICH IS USER SPECIFIED TO INDICATE
                   WHETHER ANALYTIC DERIVATIVES ARE TO BE USED FOR THE
                   GRADIENT CALCULATION

                   ANGRAD = .TRUE. WHEN ANALYTIC DERIVATIVES ARE TO BE
                                   USED

                   ANGRAD = .FALSE. WHEN FINITE DIFFERENCE DERIVATIVES
                                   ARE TO BE USED


    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS", PRENTICE-HALL,
              1983

         GILL,MURRAY AND WRIGHT, "PRACTICAL OPTIMIZATION", ACADEMIC
              PRESS, 1981


    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984

    FUNCTIONS/SUBROUTINES CALLED -


***********************************************************************/

{
  double temp1,temp2,temp3;
  long i;

  if(HZRstr.opt.N<1) {
    HZRstr.opt.trmcod = -1;
    return;
  }

  if(*HZRstr.typx<=ZERO)
    for(i=0; i<HZRstr.opt.N; i++)
      HZRstr.typx[i] = ONE;

  for(i=0; i<HZRstr.opt.N; i++)
    HZRstr.sx[i] = ONE/HZRstr.typx[i];

  if(HZRstr.opt.typf<ZERO)
    HZRstr.opt.typf = ONE;

  if(HZRstr.opt.gradtl<=ZERO)
    HZRstr.opt.gradtl = ePOW(LOGe(Machn.Eps)/3.0e0);

  if(HZRstr.opt.stptol<=ZERO)
    HZRstr.opt.stptol = ePOW(TWO*LOGe(Machn.Eps)/3.0e0);

  if(HZRstr.opt.maxstp<=0) {
    temp1 = ZERO;
    temp2 = -ONE;
    for(i=0; i<HZRstr.opt.N; i++) {
      temp3 = HZRstr.sx[i]*HZRstr.sx[i];
      if(temp3>temp2)
	temp2 = temp3;
      temp1 += (HZRstr.sx[i]*HZRstr.x0[i])*
	(HZRstr.sx[i]*HZRstr.x0[i]);
    }
    temp1 = sqrt(temp1);
    temp2 = sqrt(temp2);
    HZRstr.opt.maxstp = 1.0e3*((temp1>temp2) ? temp1 : temp2);
  }
  if(HZRstr.opt.fdigit==(-1))
    HZRstr.opt.eta = Machn.Eps;
  else {
    temp1 = ePOW(LOGe(10.0e0)*(double)HZRstr.opt.fdigit);
    HZRstr.opt.eta = Machn.Eps>temp1 ? Machn.Eps : temp1;
  }

  if(HZRstr.opt.itrlmt==(-1))
    HZRstr.opt.itrlmt = 100;

  /* TEST PRTCOD OUT OF RANGE. */
  if(HZRstr.opt.prtcod<0 || HZRstr.opt.prtcod>2)
    HZRstr.opt.prtcod = 2;

  HZRstr.opt.trmcod = 0;
}

void RECHK(double *x0,double *typx,double *sx,double *typf,double *maxstp,
	   long n)
     /**
   ENTRY TO UPDATE THE CONVERGENCE CRITERIA BASED ON KNOWLEDGE GAINED
   IN THE ITERATIVE PROCESS
**/
{
  double temp1,temp2,temp3;
  long i;

  for(i=0; i<n; i++) {
    typx[i] = fabs(x0[i]);
    if(typx[i]>ZERO)
      sx[i] = ONE/typx[i];
    else
      sx[i] = ONE;
  }

  *typf = fabs(*typf);

  temp1 = ZERO;
  temp2 = -ONE;
  for(i=0; i<HZRstr.opt.N; i++) {
    temp3 = sx[i]*sx[i];
    if(temp3>temp2)
      temp2 = temp3;
    temp1 += ONE;
  }
  temp1 = sqrt(temp1);
  temp2 = sqrt(temp2);
  *maxstp = 1.0e3*((temp1>temp2) ? temp1 : temp2);
}
