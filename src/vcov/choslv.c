#include <string.h>
#include "common.h"

#include "hzr_qextd.h"
#include "hzr_qsum.h"
#include "hzr_qmul.h"   
#include "hzr_qtod.h"

void CHOSLV(double *l,double *g,double *s,int n)

     /***************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO SOLVE THE CHOLESKY FACTORED
    SYSTEM OF LINEAR EQUATIONS L*L'S = -G FOR THE VECTOR S

    THE ARGUMENTS OF THE SUBROUTINE HAVE THE FOLLOWING MEANINGS:

         L(,)......ON INPUT AN N X N MATRIX CONTAINING THE CHOLESKY
                   FACTOR IN THE LOWER TRIANGULAR PORTION. L IS NOT
                   CHANGED BY THE SUBROUTINE

         G().......ON INPUT, G IS AN N-VECTOR WHICH CONTAINS THE
                   RIGHT HAND SIDE OF THE SYSTEM OF LINEAR EQUATIONS
                   TO BE SOLVED. G IS NOT CHANGED BY THE SUBROUTINE

         S().......ON OUTPUT, S IS AN N-VECTOR WHICH CONTAINS THE
                   SOLUTION TO THE SYSTEM OF LINEAR EQUATIONS

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984
                      AUGUST 1987 - CLEAN UP XTENDED PRECISION DIVIDE

    FUNCTIONS/SUBROUTINES CALLED - QEXTD

     ***************************************************************/
{
  xtended qsum;
  double *li,*lii,*lij,*lji;
  int i,j,n1;

  /* BEGIN BY SOLVING L*S =-G */
  n1 = n+1;
  for(i=0,lii=li=l; i<n; i++,lii+=n1,li+=n) {
    qsum = QEXTD(ZERO);
    for(j=0,lij=li; j<i; j++,lij++)
      qsum = QSUM(qsum,QMUL(QEXTD(*lij),QEXTD(s[j])));
    s[i] = (-g[i]-QTOD(qsum))/(*lii);
  }

  /* NEXT SOLVE L'*S = S */
  for(i=n-1,lii=l+(n*n)-1; i>=0; i--,lii-=n1) {
    qsum = QEXTD(ZERO);
    for(j=i+1,lji=lii+n; j<n; j++,lji+=n)
      qsum = QSUM(qsum,QMUL(QEXTD(*lji),QEXTD(s[j])));
    s[i] = (s[i]-QTOD(qsum))/(*lii);
  }
}
