#include <math.h>

#include "common.h"

#include "hzr_qextd.h"
#include "hzr_qmul.h"
#include "hzr_qsum.h"
#include "hzr_qneg.h"
#include "hzr_qtod.h"



void CHLSKM(double *a,double *wk1,double *wk2,double *sx,long n,
	    long iop)

     /***************************************************************

      THE PURPOSE OF THIS SUBROUTINE IS TO CALCULATE THE MODIFIED
    CHOLESKY DECOMPOSITION OF AN N X N SYMMETRIC MATRIX A. THE
    DECOMPOSITION IS AS DESCRIBED IN THE PAPER, "NEWTON-TYPE METHODS
    FOR UNCONSTRAINED AND LINEARLY CONSTRAINED OPTIMIZATION", 
    P.E.GILL AND W.MURRAY,MATH. PROG. 7(1974),PP311-350(SECTIONS 2.1
    AND 2.2). THE PROGRAM RETURNS A DECOMPOSITION OF THE MATRIX A + 
    E, WHERE E IS A DIAGONAL MATRIX OF SUFFICIENT SIZE TO GUARENTEE 
    THE POSITIVE DEFINITENESS OF A + E. IF A IS SAFELY POSITIVE 
    DEFINITE THEN E = 0.
      ONE OF TWO FORMS OF THE DECOMPOSITION IS RETURNED DEPENDING ON
    THE VALUE OF IOP. WHEN IOP=0, A + E = L*(L-TRANSPOSE) IS
    RETURNED. WHEN IOP=1, A + E = L*D*(L-TRANSPOSE),L UNIT LOWER
    TRIANGULAR, IS RETURNED. FINALLY WHEN IOP=0, THE MATRIX IS
    SCALED BY THE DIAGONAL SCALING MATRIX SX PRIOR TO DECOMPOSITION 
    AND THE FACTOR L IS UNSCALED BEFORE RETURN. WHEN IOP=1, NO
    SCALING IS PERFORMED.

    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

         A(,)<----->ON INPUT ,REAL SYMMETRIC N X N MATRIX TO BE
                    FACTORED.
                    ON OUTPUT,UPPER TRIANGULAR PART CONTAINS  A, 
		      UNMODIFIED IN THE UPPER TRIANGULAR NONDIAGONAL
		      PART. THE NONDIAGONAL LOWER TRIANGULAR PART
		      CONTAINS THE NONDIAGONAL PORTION OF L. WHEN 
		      IOP=0 THE DIAGONAL CONTAINS THE DIAGONAL OF L.
		      WHEN IOP=1, THE DIAGONAL CONTAINS THE ELEMENTS 
		      OF D. IN THIS CASE THE DIAGONAL ELEMENTS OF L 
		      ARE ALL 1.

         WK1()<-----REAL N-VECTOR OF WORK STORAGE. ON OUTPUT,WK1
                    CONTAINS THE DIAGONAL ELEMENTS OF THE ORIGINAL
                    UNMODIFIED A MATRIX.

         WK2()<-----REAL N-VECTOR OF WORK STORAGE. ON OUTPUT, WK2
                    CONTAINS THE ELEMENTS OF E.

         SX()------>REAL N-VECTOR OF DIAGONAL ELEMENTS OF SCALING
                    MATRIX. ON ENTRANCE A <- (SX**-1)*A*(SX**-1)
                            ON EXIT     A <- SX*A*SX
                                        L <- SX*L
                    SX IS NOT USED WHEN IOP=1 BUT A DUMMY ARRAY MUST
                    BE PASSED.

         MACHEP---->UNIT OF ROUNDING ERROR FOR THE COMPUTER

         N--------->DIMENSION OF THE MATRIX A

         IOP------->OPTION FLAG WITH FOLLOWING MEANINGS:
                      IOP=0, A + E = L*(L-TRANSPOSE)
                      IOP=1, A + E = L*D*(L-TRANSPOSE)

    AUTHOR/IMPLEMENTOR - CHARLES R. KATHOLI

    LATEST REVISION - MARCH 1986

    FUNCTIONS/SUBROUTINES CALLED - DABS,DMAX1,QEXTD

     ***************************************************************/

{
  xtended qsum,qtemp;
  double *ai,*aij,*aik,*aj,*ajj,*ajk,*ak,*ajx;
  double temp,xce,gamma,sum,norma,delta,beta2,theta2,dbar,phi;
  long i,j,k,n1;

  if(iop<0 || iop>1)
    iop = 0;

  if(iop==0)
    for(i=0,ai=a; i<n; i++,ai+=n)
      for(j=0,aj=ai; j<=i; j++,aj++)
	(*aj) /= sx[i]*sx[j];

  /* FIND THE LARGEST DIAGONAL AND OFFDIAGONAL ELEMENTS OF A. */
  xce = gamma = norma = ZERO;
  for(i=0,ai=a,ajx=a+n; i<n; i++,ai+=n,ajx+=n) {
    sum = ZERO;
    for(j=0,aj=ai; j<=i; j++,aj++) {
      temp = fabs(*aj);
      sum += temp;
      if(j<i){
	if(temp>xce) xce = temp;
      }else if(temp>gamma){
	gamma = temp;
      }
    }
    for(j=i+1,aj=ajx+i; j<n; j++,aj+=n) {
      temp = fabs(*aj);
      sum += temp;
      if(temp>xce) xce = temp;
    }
    norma = (norma>sum) ? norma : sum;
  }
  temp = xce/(double)n;
  beta2 = (gamma>temp) ? gamma : temp;
  beta2 = (beta2>Machn.Eps) ? beta2 : Machn.Eps;
  temp = Machn.Eps*norma;
  delta = (Machn.Eps>temp) ? Machn.Eps : temp;

  /**
     SET  UP  MAIN  LOOP
  **/
  for(j=0,aj=a; j<n; j++,aj+=n) {
    qsum = QEXTD(ZERO);
    if(j>0)
      for(k=0,ak=aj; k<j; k++,ak++) {
	temp = (*ak)/wk1[k];
	qtemp = QMUL(QEXTD(temp),QEXTD(*ak));
	qsum = QSUM(qsum,qtemp);
	*ak = temp;
      }
    qtemp = QSUM(QEXTD(*(aj+j)),QNEG(qsum));
    phi = QTOD(qtemp);
    dbar = (delta>fabs(phi)) ? delta : fabs(phi);

    /* UPDATE J-TH COLUMN BELOW THE DIAGONAL */
    theta2 = ZERO;
    for(i=j+1,(ai=aj+n),aij=ai+j; i<n; i++,ai+=n,aij+=n) {
      qsum = QEXTD(ZERO);
      for(k=0,ajk=aj,aik=ai; k<j; k++,ajk++,aik++) {
	qtemp = QMUL(QEXTD(*ajk),QEXTD(*aik));
	qsum = QSUM(qsum,qtemp);
      }
      qtemp = QSUM(QEXTD(*aij),QNEG(qsum));
      *aij = QTOD(qtemp);
      theta2 = (theta2>fabs(*aij)) ? theta2 : fabs(*aij);
    }
    theta2 *= theta2;
    temp = theta2/beta2;
    wk1[j] = (dbar>temp) ? dbar : temp;
    if(delta>((fabs(phi)>temp) ? fabs(phi) : temp))
      wk2[j] = delta-phi;
    if(temp>((delta>fabs(phi)) ? delta : fabs(phi)))
      wk2[j] = temp-phi;
    if(fabs(phi)>((temp>delta) ? temp : delta)) {
      wk2[j] = fabs(phi)-phi;
      if(fabs(wk2[j])<=TWO*Machn.Eps*fabs(phi))
	wk2[j] = ZERO;
    }
  }

  /* CHECK IOP AND END APPROPRIATELY */

  n1 = n+1;
  if(iop==0)
    /* L <- SX*L*D**0.5 */
    for(j=0,ai=a+n,ajj=a; j<n; j++,ai+=n,ajj+=n1) {
      temp = sqrt(wk1[j]);
      wk1[j] = (sx[j]*sx[j])*(*ajj);
      *ajj = temp*sx[j];
      for(i=j+1,aij=ai+j; i<n; i++,aij+=n)
	*aij *= temp*sx[i];
    }
  else if(iop==1)
    /* PLACE D IN DIAGONAL ELEMENTS OF L AND DIAGONAL OF A IN WK1 */
    for(j=0,ajj=a; j<n; j++,ajj+=n1) {
      temp = wk1[j];
      wk1[j] = *ajj;
      *ajj = temp;
    }
}
