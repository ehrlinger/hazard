#include "common.h"

#include "hzr_qextd.h"  
#include "hzr_qsum.h"
#include "hzr_qmul.h"
#include "hzr_qtod.h"
#include "hzr_qneg.h"

double DQDOT(double *x,double *y,long n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO CALCULATE THE INNER PRODUCT X'*Y
    OF TWO N-VECTORS X() AND Y().
    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

          X(),Y()...DOUBLE PRECISION N-VECTORS FOR WHICH THE INNER
                    PRODUCT IS TO BE CALCULATED.

          N.........LENGTH OF EACH VECTOR

    IMPLEMENTOR - C.R. KATHOLI

    FUNCTIONS/SUBROUTINES CALLED - QEXTD

    LATEST REVISION - JULY 1984

***********************************************************************/

{
  xtended qsum,qtemp;
  double temp;
  long i,m;

  qsum = QEXTD(ZERO);
  m = n-(n/5)*5;
  if(m!=0)
    for(i=0; i<m; i++)
      qsum = QSUM(qsum,QMUL(QEXTD(x[i]),QEXTD(y[i])));
  if(n>=5)
    for(i=m; i<n; i+=5)
      qsum = QSUM(qsum,
		  QSUM(QMUL(QEXTD(x[i]),QEXTD(y[i])),
		       QSUM(QMUL(QEXTD(x[i+1]),QEXTD(y[i+1])),
			    QSUM(QMUL(QEXTD(x[i+2]),QEXTD(y[i+2])),
				 QSUM(QMUL(QEXTD(x[i+3]),QEXTD(y[i+3])),
				      QMUL(QEXTD(x[i+4]),QEXTD(y[i+4])))))));

  /* ROUND RESULT TO DOUBLE PRECISION */
  temp = QTOD(qsum);
  qtemp = QEXTD(temp);
  return temp+QTOD(QMUL(QEXTD(TWO),QSUM(qsum,QNEG(qtemp))));
}
