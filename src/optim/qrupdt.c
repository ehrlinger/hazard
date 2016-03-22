#include <string.h>
#include <math.h>
#include "hzdinc.h"
#include "jacrot.h"

void QRUPDT(double *m,double *u,double *v,int n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO CALCULATE THE QR FACTORIZATION
    (Q+)(R+) OF A+ = Q(R + UV') GIVEN A = QR.

    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

         M(,).......ON INPUT, THE N X N MATRIX M  CONTAINS THE FACTOR
                    R IN THE UPPER TRIANGULAR PART.
                    ON OUTPUT, M CONTAINS R+ IN THE UPPPER TRIANGULAR
                    PORTION. THE FIRST LOWER SUBDIAGONAL IS ALSO
                    CHANGED BY THE ROUTINE.

         U(),V()....N-VECTORS USED TO INPUT UPDATE INFORMATION

         N..........ROW/COLUMN DIMENSION OF M.


    REF: DENNIS AND SCHNABEL,"NUMERICAL METHODS FOR UNCONSTRAINED
                OPTIMIZATION AND NONLINEAR EQUATIONS",PRENTICE-HALL
                (1983)

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984

    SUBROUTINES CALLED - JACROT,DSQRT,DABS

***********************************************************************/
{
  double a,b;
  double *mii,*mi1i,*mj;
  int i,j,k,l,n1;

  /* FIND LARGEST K SUCH THAT U(K) .NE. ZERO) */
  for(k=n-1; k>=0 && u[k]==ZERO; k--);

  for(i=0; i<k; i++) {
    l = (k-i)-1;
    a = u[l];
    b = -u[l+1];
    JACROT(m,a,b,l,n);
    if(u[l]==ZERO)
      u[l] = fabs(u[l+1]);
    if(u[l]!=ZERO)
      u[l] = sqrt(u[l]*u[l]+u[l+1]*u[l+1]);
  }
  for(j=0,mj=m; j<n; j++,mj++)
    *mj += (*u)*v[j];

  /* TRANSFORM UPPER HESSENBERG MATRIX TO UPPER TRIANGULAR */
  n1= n+1;
  for(i=0,mii=m,mi1i=m+n; i<k; i++,mii+=n1,mi1i+=n1) {
    a = *mii;
    b = -(*mi1i);
    JACROT(m,a,b,i,n);
  }
}
