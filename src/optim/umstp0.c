#include <string.h>
#include <math.h>

#include "common.h"

/*
umstp0 was modified: the counter in the for loop was
originally initialized to 1, this was changed to 0.
I believe that the 1 was a transcription error that
occured as the Fortran code was translated to C

*/


void UMSTP0(double *x0,double f0,double *g0,double *sx,double typf,
	    double gradtl,int *trmcod,int *consec,long n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO DECIDE WHETHER TO TERMINATE
    THE ALGORITHM AT ITERATION ZERO BECAUSE THE INITIAL ESTIMATE X0 IS
    AN APPROXIMATE CRITICAL POINT OF THE OBJECTIVE FUNCTION F(X)

    THE ARGUMENTS OF THE SUBROUTINE HAVE THE FOLLOWING MEANINGS:

         X0()......ON INPUT, REAL N-VECTOR OF INITIAL ESTIMATES OF THE
                   INDEPENDENT VARIABLES

         F0........ON INPUT, REAL SCALAR CONTAINING THE VALUE OF THE
                   OBJECTIVE FUNCTION AT X0

         G0()......ON INPUT, REAL N-VECTOR CONTAINING THE GRADIENT OF
                   THE OBJECTIVE FUNCTION AT X0

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


         TRMCOD....ON OUTPUT, INTEGER FLAG HAVING THE FOLLOWING
                   MEANINGS:
                    0 - X0 IS NOT AN APPROXIMATE CRITICAL POINT,
                        COMPUTATION SHOULD PROCEED
                    1 - X0 APPEARS TO BE AN APPROXIMATE CRITICAL POINT
                        IT IS POSSIBLE THAT X0 IS A LOCAL MAXIMIZER OR
                        A SADDLE POINT. THE CALCULATION SHOULD BE
                        TRIED AGAIN WITH A PERTURBATION OF X0

         CONSEC....INTEGER VARIABLE USED TO KEEP A COUNT OF THE NUMBER
                   OF CONSECUTIVE STEPS TAKEN OF LENGTH MAXSTP. CONSEC
                   IS INITIALIZED TO ZERO BY THIS ROUTINE

         N.........THE DIMENSION OF THE PROBLEM; I.E., THE NUMBER
                   OF INDEPENDENT VARIABLES

    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS",PRENTICE-HALL,
              1983

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984

    FUNCTIONS/SUBROUTINES CALLED - DMAX1,DABS

***********************************************************************/

{
  double den,temp1,temp2;
  int i;

  *consec = 0;
  den = fabs(f0)>typf ? fabs(f0) : typf;
  temp1 = fabs(*g0)*
    (((fabs(*x0)>(ONE/(*sx))) ? fabs(*x0) : (ONE/(*sx)))/den);
  for(i=0; i<n; i++) {
    temp2 = fabs(g0[i])*
      (((fabs(x0[i])>(ONE/(sx[i]))) ? fabs(x0[i]) : (ONE/(sx[i])))/den);
    if(temp2>temp1)
      temp1 = temp2;
  }
  if(temp1>(Params.um0scl*gradtl))
    *trmcod = 0;
  else
    *trmcod = 1;
  return;
}
