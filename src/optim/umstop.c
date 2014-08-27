#include <math.h>
#include "common.h"
/*
umstop was modified: the counters in the for loops were
originally initialized to 1, this was changed to 0.
I believe that the 1 was a transcription error that
occured as the Fortran code was translated to C
*/



void UMSTOP(double *xc,double *xplus,double *gplus,double *sx,
	    double fplus, double typf,double gradtl,double stptol,
	    long iret,long itrcnt, long itrlmt,long *trmcod,
	    long *consec,logical maxtkn,long n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO DECIDE WHETHER TO TERMINATE THE
    SEARCH FOR A MINIMUM AT THE CURRENT VALUE OF THE INDEPENDENT
    VARIABLES

    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

         XC()......ON INPUT, N-VECTOR CONTAINING THE LAST VALUES OF
                   THE INDEPENDENT VARIABLES

         XPLUS()...ON INPUT, N-VECTOR CONTAINING THE CURRENT VALUES OF
                   THE INDEPENDENT VARIABLES

         GPLUS()...ON INPUT, N-VECTOR CONTAINING THE VALUE OF THE
                   GRADIENT OF THE OBJECTIVE FUNCTION AT XPLUS

         SX()......ON INPUT, N-VECTOR CONTAINING THE DIAGONAL ELEMENTS
                   OF THE SCALING MATRIX, DX.  SX(I) = 1/TYPX(I)
                   TYPX() IS AN N-VECTOR WHOSE I-TH COMPONENT IS A
                   POSITIVE SCALAR SPECIFYING THE TYPICAL MAGNITUDE OF
                   X(I). IT IS IMPORTANT TO SUPPLY VALUES OF TYPX WHEN
                   THE COMPONENTS OF X ARE EXPECTED TO BE VERY
                   DIFFERENT IN MAGNITUDE. IN THIS CASE THE CODE MAY
                   WORK BETTER WITH GOOD SCALING INFORMATION THAN WITH
                   DX = I. WHEN ALL THE X(I) ARE SCALED THE SAME
                   DX = I WORKS WELL.

                   SUGGESTED DEFAULT VALUE: TYPX(I) = 1, I=1,...,N

         FPLUS.....ON INPUT, VALUE OF THE OBJECTIVE FUNCTION AT XPLUS

         TYPF......ON INPUT, A POSITIVE SCALAR ESTIMATING THE MAGNITUDE
                   OF THE OBJECTIVE FUNCTION NEAR THE MINIMIZER X*. IF
                   TOO LARGE A VALUE IS PROVIDED FOR TYPF, THE ALGO-
                   RITHM MAY HALT PREMATURELY. IN PARTICULAR, IF F(X0)
                   IS >> F(X*), TYPF SHOULD BE APPROXIMATELY ABS(F(X*))

                   SUGGESTED DEFAULT VALUE: TYPF = 1


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


         IRET......ON INPUT, IRET IS THE TERMINATION CODE FROM THE
                   LINE SEARCH SUBROUTINE, LINESR

         ITRCNT....ON INPUT, CURRENT VALUE OF THE ITERATION COUNTER

         ITRLMT....ON INPUT, VALUE OF THE MAXIMUM NUMBER OF ITERATIONS
                   TO BE ATTEMPTED

         TRMCOD....ON OUTPUT, AN INTEGER VARIABLE WITH THE FOLLOWING
                   MEANINGS:
                       0 - NO TERMINATION CRITERION SATISFIED
                       1 - NORM OF SCALED GRADIENT LESS THAN GRADTL;
                           XPLUS IS PROBABLY AN APPROXIMATE MINIMIZER
                           OF F(X)...UNLESS GRADTL IS TOO LARGE
                       2 - SCALED DISTANCE BETWEEN THE LAST TWO STEPS
                           IS LESS THAN STPTOL;XPLUS MAY BE AN APPROX-
                           IMATE LOCAL MINIMIZER OF F(X). IT IS ALSO
                           POSSIBLE THAT THE ALGORITHM IS MAKING SLOW
                           PROGRESS AND IS NOT NEAR A MINIMIZER OR
                           THAT STPTOL IS TOO LARGE
                       3 - LAST GLOBAL STEP FAILED TO LOCATE A POINT
                           LOWER THAN XC; EITHER XC IS AN APPROXIMATE
                           LOCAL MINIMIZER AND NO MORE ACCURACY IS
                           POSSIBLE OR AN INACCURATELY CODED ANALYTIC
                           GRADIENT IS BEING USED OR THE FINITE DIFF-
                           ERENCE GRADIENT IS TOO INACCURATE OR STPTOL
                           IS TOO LARGE
                       4 - ITERATION LIMIT EXCEEDED
                       5 - FIVE CONSECUTIVE STEPS OF LENGTH MAXSTP
                           HAVE BEEN TAKEN; EITHER F(X) IS UNBOUNDED
                           BELOW OR F(X) HAS A FINITE ASYMPTOTE IN
                           SOME DIRECTION OR MAXSTP IS TOO SMALL

         CONSEC....INTEGER VARIABLE USED TO KEEP A COUNT OF THE NUMBER
                   OF CONSECUTIVE STEPS TAKEN OF LENGTH MAXSTP

         MAXTKN....LOGICAL VARIABLE OUTPUT FROM SUBROUTINE LINESR WHICH
                   iS TRUE IF THE MAXIMUM STEP WAS TAKEN ON THE
                   CURRENT ITERATION

         N.........DIMENSION OF THE PROBLEM


    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS", PRENTICE-HALL,
              1983

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984
    REVISIONS
       July 1984 was previous acknowledged revision.

       HZD4.036 (09/01/95)
          Test after loop for too-small step was erroneously testing
          TEMP2 instead of TEMP1, which resulted in error under some
          environments when N=1 (which caused TEMP2 to be undefined).

    FUNCTIONS/SUBROUTINES CALLED - DMAX1,DABS

***********************************************************************/

{
  double temp1,temp2,denom;
  long i;

  *trmcod = 0;
  if(iret==1) {
    *trmcod = 3;
    return;
  }

  /* CHECK PRINCIPAL TERMINATION CRITERION */
  denom = fabs(fplus)>typf ? fabs(fplus) : typf;
  temp1 = fabs(*gplus)*
    ((fabs(*xplus)>(ONE/(*sx))) ? fabs(*xplus) : (ONE/(*sx)))/denom;
  for(i=0; i<n; i++) {
    temp2 = fabs(gplus[i])*
      ((fabs(xplus[i])>(ONE/(sx[i]))) ? fabs(xplus[i]) : 
       (ONE/(sx[i])))/denom;
    if(temp2>temp1)
      temp1 = temp2;
  }
  if(temp1<=gradtl) {
    *trmcod = 1;
    return;
  }

  /* CHECK FOR SMALL CHANGE IN X */
  temp1 = fabs(*xplus-*xc)/
    (fabs(*xplus)>(ONE/(*sx)) ? fabs(*xplus) : (ONE/(*sx)));
  for(i=0; i<n; i++) {
    temp2 = fabs(xplus[i]-xc[i])/
      (fabs(xplus[i])>(ONE/(sx[i])) ? fabs(xplus[i]) : (ONE/(sx[i])));
    if(temp2>temp1)
      temp1 = temp2;
  }
  if(temp1<=stptol) {
    *trmcod = 2;
    return;
  }

  if(itrcnt>=itrlmt) {
    *trmcod = 4;
    return;
  }

  if(!maxtkn) {
    *consec = 0;
    return;
  }

  i = *consec++;
  if(i>=5) {
    *trmcod = 5;
    return;
  }
  return;
}
