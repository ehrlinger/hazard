#include <math.h>
#include <string.h>

#include "common.h"

#include "hzdinc.h"
#include "hzr_set_error.h"

double RQUAD(double a,double b,double c)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO FIND THE ROOT ,

                  (-B + SQRT(B**2 - 3*A*C))/ 3A  , A .NE. 0

        AND
                   - C/(I*B) ,  A = 0


    OF THE POLYNOMIAL 3*A*X**2 + 2*B*X + C


    THE ARGUMENTS OF THE FUNCTION HAVE THE FOLLOWING MEANINGS:

         A..........ON INPUT, A IS ONE THIRD THE COEFFICIENT OF THE
                    QUADRATIC TERM OF THE POLYNOMIAL

         B..........ON INPUT, B IS ONE HALF THE COEFFICIENT OF THE
                    LINEAR TERM OF THE POLYNOMIAL

         C..........ON INPUT, C IS THE CONSTANT TERM OF THE POLYNOMIAL


    IMPLEMENTOR - C.R. KATHOLI

    FUNCTIONS/SUBROUTINES CALLED  - QSQRT,QEXTD

    LATEST REVISION -  JULY 1984
                       MAY 1986 USE LOGS TO AVOID OVERFLOWS
                       AUGUST 1987 REMOVE EXTENDED PRECISION DIVIDE

***********************************************************************/

{
  double avb,d,temp,lnarg,disc;

  /* INITIALIZE */
  Common.errorno = 0;
  CLEAR(Common.errflg);

  if(a==ZERO)
    return -(c/b)/TWO;

  /**
     A NOT  = 0
     DISC =B*B - 3*A*C
     2
     EXPRESS B  - 3AC AS B(B - 3AC/B)
  **/
  avb = a/b;
  d = b-3.0e0*c*avb;

  /**
     2
     GET SQRT(B - 3AC) USING LOGS
     BOTH TERMS POSITIVE
  **/
  if(b>ZERO && d>ZERO)
    lnarg = 0.5e0*(LOGe(b)+LOGe(d));
  else if(b<ZERO && d<ZERO)
    lnarg = 0.5e0*(LOGe(-b)+LOGe(-d));
  else {
    hzr_set_error("(RQUAD900)",2);
    return ZERO;
  }

  if(lnarg>LnLim.absMax || lnarg<LnLim.absMin) {
    hzr_set_error("(RQUAD910)",1);
    return ZERO;
  }

  disc = ePOW(lnarg);
  temp = disc;
  temp += TWO*(disc-temp);
  temp = -b-(((temp>0 && b>0) || (temp<0 && b<0)) ? temp : -temp);
  if(b>ZERO)
    return c/temp;
  else
    return (temp/a)/3.0e0;
}
