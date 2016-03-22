#include <string.h>
#include <math.h>

#include "common.h"

#include "dqdot.h"
#include "hzr_qextd.h"

#include "hzr_qsum.h"
#include "hzr_qmul.h"
        
#include "hzr_qtod.h"
#include "hzr_qdiv.h"
#include "hzr_qneg.h"

double Q1(double *a,double grad,double *b1,double *qtol,int *err,
	  double *wk,int n)

     /***********************************************************************

    THE PURPOSE OF THIS FUNCTION IS TO CALCULATE THE Q-STATISTIC
    ASSOCIATED WITH BRINGING A NEW VARIABLE INTO THE MAXIMUM LIKELIHOOD
    MODEL. GIVEN THE GRADIENT OF THE NEW MODEL WITH RESPECT TO THE
    ADDITIONAL VARIABLE (GRAD) AND THE HESSIAN MATRIX  H   OF THE NEW
    MODEL WHERE

               -                   -
               |  A             H1  |
       H  =    |                    |
               |  H1-TRANSPOSE  H22 |
               -                    -

    THEN Q1=GRAD*GRAD*I22 WHERE I22 IS THE HIGHEST ORDER DIAGONAL
    ELEMENT OF H**-1. IT IS EASILY SHOWN THAT

             I22 = 1/(H22*(1-H22**-1*(H1-TRANSPOSE)*A**-1*H1))

                 = 1/(H22*(1-(L**-1*H1)-TRANSPOSE*(L**-1*H1)/H22)

    WHERE L IS THE CHOLESKY FACTOR OF A.

    THE ARGUMENTS OF THE FUNCTION SUBPROGRAM HAVE THE FOLLOWING
    MEANINGS:

         A(,)------>N X N MATRIX WHICH CONTAINS THE CHOLESKY FACTOR
                    L OF THE PRINCIPAL N X N SUBMATRIX OF THE
                    (N+1) X (N+1) HESSIAN MATRIX FOR THE EXPANDED
                    MODEL IN THE DIAGONAL AND LOWER DIAGONAL PORTION

         GRAD------>REAL SCALAR CONTAINING THE GRADIENT OF THE
                    OBJECTIVE FUNCTION WITH RESPECT TO THE PROPOSED NEW
                    VARIABLE.

         B1-------->VECTOR OF LENGTH (N+1) CONTAINING THE (N+1)-ST
                    ROW(COLUMN) OF THE HESSIAN MATRIX. B1 IS NOT
                    MODIFIED BY THE PROGRAM.

         QTOL------>REAL SCALER CONTAINING DENOMINATOR OF Q

         ERR------->INTEGER SCALER CONTAINING ERROR MESSAGES

         WK---------REAL N-VECTOR OF WORKING STORAGE

         N--------->DIMENSION OF THE MATRIX A.

    NOTE:THE PROGRAM ASSUMES THAT  H  IS SUFFICIENTLY POSITIVE DEFINITE
    THAT ITS CHOLESKY FACTORIZATION EXISTS IN A PRACTICAL SENSE.IF NOT,
    IT IS POSSIBLE THAT THE PROGRAM WILL RETURN A NEGATIVE Q VALUE. IF
    THIS SHOULD OCCUR, THE USER SHOULD USE THE MORE EXPENSIVE Q2 AS AN
    ALTERNATIVE.

    AUTHOR/IMPLEMENTOR - CHARLES R. KATHOLI

    LATEST REVISION - MARCH 1986
                      AUGUST 1987 - CLEAN UP XTENDED PRECISION DIVIDE

***********************************************************************/

{
  xtended qsum;
  double diag,scale,temp;
  double *ai,*aii,*aij;
  int i,j,n1;

  *err = 0;
  if(grad==ZERO) {
    *err = 1;
    return ZERO;
  }
  diag = b1[n];
  if(diag<=ZERO) {
    *err = 2;
    return ZERO;
  }
  scale = ONE/sqrt(diag);
  for(i=0; i<n; i++)
    wk[i] = b1[i]*scale;

  /* CALCULATE (L**-1)*B1/SQRT(H22) */
  n1 = n+1;
  for(i=0,ai=a,aii=a; i<n; i++,ai+=n,aii+=n1) {
    qsum = QEXTD(ZERO);
    for(j=0,aij=ai; j<i; j++,aij++)
      qsum = QSUM(qsum,QMUL(QEXTD(*aij),QEXTD(wk[j])));
    wk[i] = QTOD(QDIV(QSUM(QEXTD(wk[i]),QNEG(qsum)),QEXTD(*aii)));
  }

  /* CALCULATE (WK-TRANSPOSE)*WK */
  temp = DQDOT(wk,wk,n);
  *qtol = ONE-temp;
  if(*qtol<=ZERO) {
    *err = 3;
    return ZERO;
  }
  return scale*scale*(grad*grad)/(*qtol);
}
