#include <math.h>

#include "common.h"

#include "choslv.h"

#include "hzr_qextd.h"      
#include "hzr_qsum.h"
#include "hzr_qmul.h"
#include "hzr_qtod.h"
#include "hzr_qneg.h"

void CNDEST(double *l,double *p,double *pm,double *x,
	    double *condno, long n)
     /**************************************************************
    The purpose of this routine is to estimate the L-1 condition 
    number of a symmetric positive definitematrix A = L*(L-TRANSPOSE)
    presented in factored form.

    The arguments of the routine have the following meanings:

         L(,)------>An N X N real array containing the Cholesky
	             factor L of A in the diagonal and lower 
		      triangular piece this array is not modified by 
		      the program.

         P()
         PM()
         X()--------Real work storage arrays of dimension at least N.

         CONDNO<----Real variable estimate of the L-1 condition 
	             number of A

         N--------->Integer variable giving the dimension of L,P,PM,
                    and X.

    AUTHOR/IMPLEMENTOR - Charles R Katholi

    LATEST REVISION - February 1986
                      August 1987 - Clean up xtended precision divide
                      August 1987 - MOve JP1 statement to avoid
                                    addressing exception

    FUNCTIONS/SUBROUTINES CALLED - CHOSLV,QEXTD,DABS,MIN0,

     ***************************************************************/

{
  xtended sum0,sum1,qtemp;
  double *li,*lj,*lii,*lij,*lji,*ljj,*lik,*ljk;
  double xp,xm,xnorm,temp,tempm;
  long i,j,k,n1,kmax;

  /* BEGIN BY CALCULATING THE L-1 NORM OF A. */
  qtemp = QEXTD(ZERO);
  for(j=0,lj=l; j<n; j++,lj+=n) {
    sum1 = QEXTD(ZERO);
    for(i=0,li=l; i<n; i++,li+=n) {
      kmax = i<j ? i : j;
      sum0 = QEXTD(ZERO);
      for(k=0,lik=li,ljk=lj; k<=kmax; k++,lik++,ljk++)
	sum0 = QSUM(sum0,QMUL(QEXTD(*lik),QEXTD(*ljk)));
      if(QTOD(sum0)<ZERO)
	sum0 = QNEG(sum0);
      sum1 = QSUM(sum1,sum0);
    }
    qtemp = QTOD(sum1)>QTOD(qtemp) ? sum1 : qtemp;
  }
  *condno = QTOD(qtemp);

  /**
     NEXT BEGIN THE ESTIMATION OF THE L-1 NORM OF A**-1. BEGIN BY
     CALCULATING L*(L-TRANSPOSE)X=E WHERE THE E(I) ARE CHOSEN BY THE
     ALGORITHM OF CLINE,MOLER,STEWART AND WILKINSON,SIAM J. NUMERICAL
     ANALYSIS(1979),16,PP368-375, EQUATION (3.19). NOTE THAT THIS
     CODE IS BASED ON THE DISCUSSION FOR THE PROCEEDURE WHEN AN L-U
     DECOMPOSITION IS GIVEN. IT SHOULD NOT HOWEVER BE SUBJECT TO THE
     POTENTIAL PROBLEMS OF THAT CASE SINCE HERE U=L-TRANSPOSE.
  **/
  *x = ONE/(*l);
  for(i=1,li=l+n; i<n; i++,li+=n)
    p[i] = (*li)*(*x);

  /* SELECT E(J) AND CALCULATE X(J) */
  n1 = n+1;
  for(j=1,ljj=l+n1; j<n; j++,ljj+=n1) {
    xp = (ONE-p[j])/(*ljj);
    xm = (-ONE-p[j])/(*ljj);
    temp = fabs(xp);
    tempm = fabs(xm);
    for(i=j+1,lij=ljj+n,lii=ljj+n1; i<n; i++,lij+=n,lii+=n1) {
      pm[i] = p[i]+(*lij)*xm;
      tempm += fabs(pm[i])/fabs(*lii);
      p[i] += (*lij)*xp;
      temp += fabs(p[i])/fabs(*lii);
    }
    if(temp>=tempm)
      x[j] = xp;
    else {
      x[j] = xm;
      for(i=j+1; i<n; i++)
	p[i] = pm[i];
    }
  }

  /**
     NOW COMPLETE THE SOLUTION OF L*(L-TRANSPOSE)X=E BY SOLVING
     (L-TRANSPLOE)*X=X WHERE THE RIGHT HAND SIDE IS THE CURRENT
     VECTOR IN X
  **/
  for(i=n-1,lii=l+(n*n)-1; i>=0; i--,lii-=n1) {
    sum0 = QEXTD(ZERO);
    for(j=i+1,lji=lii+n; j<n; j++,lji+=n)
      sum0 = QSUM(sum0,QMUL(QEXTD(*lji),QEXTD(x[j])));
    x[i] = (x[i]-QTOD(sum0))/(*lii);
  }

  /**
     CALCULATE THE L-1 NORM OF X. ALSO SINCE CHOSLV WILL BE USED TO
     COMPLETE THE CALCULATION, SET P(J)=-X(J) FOR ALL J.
  **/
  xnorm = ZERO;
  for(i=0; i<n; i++) {
    xnorm += fabs(x[i]);
    p[i] = -x[i];
  }

  *condno = (*condno)/xnorm;

  /* NEXT CALL CHOSLV TO CALCULATE ((L*(L-TRANSPOSE)**-1)*X */
  CHOSLV(l,p,x,n);

  /* AGAIN CALCULATE THE L-1 NORM OF X */
  xnorm = ZERO;
  for(i=0; i<n; i++)
    xnorm += fabs(x[i]);

  *condno = (*condno)*xnorm;
}
