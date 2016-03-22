#include <string.h>
#include <math.h>
#include "hzdinc.h"

void IHESFA(double *l,double *sx,double f0,double typf,int n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO INITIALIZE THE FACTORED SECANT
    APPROXIMATION TO THE HESSIAN MATRIX. CLASSICALLY, L HAS BEEN SET
    TO I SO THAT THE FIRST STEP IS A STEEPEST DESCENT STEP. HERE WE
    USE A VARIANT OF THAT WHERE BY

                    L = MAX(F0,TYPF)*DIAG(SX(1),...,SX(N))

    THE ARGUMENTS OF THE SUBROUTINE HAVE THE FOLLOWING MEANINGS:

         L(,)......REAL N X N MATRIX. THE LOWER TRIANGULAR PORTION
                   OF WHICH IS INITIALIZED BY THIS ROUTINE AS
                   DESCRIBED ABOVE

         SX()......ON INPUT, N-VECTOR CONTAINING THE DIAGONAL ELEMENTS
                   OF THE SCALING MATRIX, DX.  SX(I) = 1/TYPX(I)
                   TYPX() IS AN N-VECTOR WHOSE I-TH COMPONENT IS A
                   POSITIVE SCALAR SPECIFYING THE TYPICAL MAGNITUDE OF
                   X(I). IT IS IMPORTANT TO SUPPLY VALUES OF TYPX WHEN
                   THE COMPONENTS OF X ARE EXPECTED TO BE VERY
                   DIFFERENT IN MAGNITUDE. IN THIS CASE THE CODE MAY
                   WORK BETTER WITH GOOD SCALING INFORMATION THAN WITH
                   DX = I. WHEN ALL THE X(I) ARE SCALED THE SAME,
                   DX = I WORKS WELL.

                   SUGGESTED DEFAULT VALUE: TYPX(I) = 1, I=1,...,N

         F0........REAL SCALAR CONTAINING THE VALUE OF F AT THE INITIAL
                   ESTIMATE X0 OF THE MINIMIZER

         TYPF......ON INPUT, A POSITIVE SCALAR ESTIMATING THE MAGNITUDE
                   OF THE OBJECTIVE FUNCTION NEAR THE MINIMIZER X*. IF
                   TOO LARGE A VALUE IS PROVIDED FOR TYPF, THE ALGO-
                   RITHM MAY HALT PREMATURELY. IN PARTICULAR, IF F(X0)
                   IS >> F(X*), TYPF SHOULD BE APPROXIMATELY ABS(F(X*))

                   SUGGESTED DEFAULT VALUE: TYPF = 1

         N.........THE DIMENSION OF THE PROBLEM

    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS", PRENTICE-HALL,
              1983

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984

    FUNCTIONS/SUBROUTINES CALLED - DMAX1,DABS,DSQRT

***********************************************************************/

{
  double *li,*lii,*lij;
  double scale;
  int i,j,n1;
   

  scale = fabs(f0)>typf ? fabs(f0) : f0;
  scale = sqrt(scale);

  n1 = n+1;
  for(i=0,li=lii=l; i<n; i++,li+=n,lii+=n1) {
    *lii = scale*sx[i];
    for(j=0,lij=li; j<i; j++,lij++)
      *lij = ZERO;
  }
}
