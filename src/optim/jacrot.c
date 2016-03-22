#include <string.h>
#include <math.h>
#include "hzdinc.h"

void JACROT(double *m,double a,double b,int i,int n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO PREMULTIPLY M BY THE JACOBI
    ROTATION MATRIX J(I,I+1,A,B) WHERE THE MATRIX J IS DEFINED AS:

                   |                             |
                   | 1                         0 |
                   |    1                        |
                   |       .                     |
                   |          1                  |
    J(I,I+1,A,B) = |             C -S            | <--- I-TH ROW
                   |             S  C            |
                   |                   1         |
                   |                      .      |
                   |                         .   |
                   |                           1 |
                   |                             |
                                 |
                              I-TH COL


           C = A/SQRT(A*A+B*B)  AND   S = B/SQRT(A*A+B*B)

    THE ARGUMENTS OF THE SUBROUTINE HAVE THE FOLLOWING MEANINGS:

         M(,).......ON INPUT,THE N X N MATRIX TO BE TRANSFORMED.
                    ON OUTPUT,THE RESULT OF J*M.

         A,B........ON INPUT,SCALARS USED TO DEFINE THE TRANSFORMATION
                    - SEE ABOVE DESCRIPTION OF J. NOT CHANGED BY THE
                    CODE.

         I..........THE COLUMN  INVOLVED IN THE TRANSFORMATION.IT IS
                    ASSUMED THAT M(I,J)=0 FOR J < I.

         N..........ROW - COLUMN DIMENSION OF M


    REF: DENNIS AND SCHNABEL,"NUMERICAL METHODS FOR UNCONSTRAINED
                OPTIMIZATION AND NONLINEAR EQUATIONS",PRENTICE-HALL
                (1983)

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984

    FUNCTIONS/SUBROUTINES CALLED - DSQRT

***********************************************************************/

{
  double c,s,den,temp1,temp2;
  double *mij,*mi1j;
  int j;

  if(a!=ZERO) {
    den = sqrt(a*a+b*b);
    c = a/den;
    s = b/den;
  } else {
    c = ZERO;
    s = ONE;
    if(b<ZERO)
      s = -s;
  }

  /* APPLY THE TRANSFORMATION */
  for(j=0,mij=m+(i*n),mi1j=m+((i+1)*n); j<n; j++,mij++,mi1j++) {
    temp1 = *mij;
    temp2 = *mi1j;
    *mij = c*temp1-s*temp2;
    *mi1j = s*temp1+c*temp2;
  }
}
