#include <math.h>
#include "hzdinc.h"


void FDGRAD(double *xc,double fc,double *sx,double *g,
		   void (*fn)(double *,double *,long),double eta,long n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO CALCULATE A FORWARD DIFFERENCE
    APPROXIMATION TO GRAD(F) AT X = XC USING VALUES OF F(X)

    THE ARGUMENTS OF THE SUBROUTINE HAVE THE FOLLOWING MEANINGS:

         XC()......ON INPUT, N-VECTOR OF PARAMETER VALUES AT WHICH TH
                   GRADIENT IS TO BE APPROXIMATED. XC NOT CHANGED BY
                   THE ROUTINE

         SX()......ON INPUT, N-VECTOR CONTAINING THE DIAGONAL ELEMENTS
                   OF THE SCALING MATRIX DX

         G().......ON OUTPUT,N-VECTOR CONTAINING THE FINITE DIFFERENCE
                   APPROXIMATION TO THE GRADIENT

         FC........ON INPUT FC CONTAINS THE VALUE OF THE OBJECTIVE
                   FUNCTION AT XC

         FN........EXTERNAL SUBROUTINE USED TO CALCULATE VALUES OF
                   THE OBJECTIVE FUNCTION; CALL FN(X,F,N)

         ETA.......ON INPUT, ETA = 10**-DIGITS WHERE DIGITS IS THE
                   NUMBER OF RELIABLE BASE TEN DIGITS IN THE COMPUTED
                   VALUE OF THE OBJECTIVE FUNCTION

         N.........N IS THE DIMENSION OF THE OBJECTIVE FUNCTION

    REF: DENNIS,J.E. AND SCHNABEL,R.B., "NUMERICAL METHODS FORR
               UNCONSTRAINED OPTIMIZATION AND NONLINEAR EQUATIONS",
               PRENTICE-HALL (1983)

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984
                      JUN 1996 - restored even if there is an error  

    FUNCTION/SUBROUTINE CALLED - DSQRT

***********************************************************************/

{
  double sqreta,fj,sg,tempj,stepj;
  long j;

  sqreta = sqrt(eta);
  for(j=0; j<n; j++) {
    sg = (xc[j]<ZERO) ? -ONE : ONE;
    stepj = sqreta*sg*
      ((fabs(xc[j])>(ONE/sx[j])) ? fabs(xc[j]) :
       (ONE/sx[j]));
    tempj = xc[j];
    xc[j] += stepj;
    stepj = xc[j] - tempj;

    (*fn)(xc,&fj,n);
    xc[j] = tempj;
    if(Common.errorno)
      return;
    g[j] = (fj-fc)/stepj;
  }
}
