#include <string.h>
#include <math.h>

#include <common.h>
#include <hzd_ln_eX_plus1.h>

double NORMAL(double z)

     /***********************************************************************

   EVALUATE THE SINGLE TAIL AREA OF THE NORMAL DISTRIBUTION BEYOND Z

   PURPOSE
     TO COMPUTE THE TAIL AREA OF THE NORMAL DISTRIBUTION USING A
     5-TERM POLYNOMIAL APPROXIMATION:
                                 2       3       4       5
     P(Z) = 1 - F(Z)*(B1*T + B2*T  + B3*T  + B4*T  + B5*T ) + E

     WHERE:
                             2
                  1       -(X /2)
       F(Z) = ---------- E
              SQRT(2*PI)

               1
       T = ---------
           1 + R*|Z|

                   -8
       |E| < 7.5X10

       R = 0.2316419

       B1 = 0.319381530   B2 = -0.356563782  B3 = 1.781477937

       B4 = -1.821255978  B5 = 1.330274429


   DESCRIPTION OF PARAMETERS
     INPUT
       Z      - REAL*8  NORMALLY DISTRIBUTED Z

     OUTPUT
       NORMAL - REAL*8  NORMAL INTEGRAL BEYOND Z (ONE-TAILED)

   REMARKS
     1.  SEE ABRAMOWITZ, M AND STEGUN, I A:  HANDBOOK OF MATHEMATICAL
         FUNCTIONS.  DOVER, 1972, P932, EQ 26.2.16

     2.  IF A TWO-TAILED TEST IS DESIRED, INPUT |Z| AND DOUBLE THE
         OUTPUT P-VALUE (NORMAL)

   SUBPROGRAMS REQUIRED
     DLEXP1 - LOGE(EXP(X)+1) TO HIGH ACCURACY

   METHOD
     USE ABOVE SERIES APPROXIMATION TO THE NORMAL INTEGRAL.

   IMPLEMENTATION
     E.H. BLACKSTONE, MD     APRIL 1986

   REVISIONS
     NONE

   VERIFICATION
     APRIL 1, 1986
     TESTED IN ALL RANGES AGAINST HP-65/HP-67 PROGRAM.

***********************************************************************/

{
  double t,absz,lnx,lnt,mlnfz,sum,lsum,lnr;
  int j;

  static double normal = 0;

  t = ONE;
  if(z!=ZERO) {
    absz = fabs(z);
    lnx = LOGe(Params.normlr*absz);
    lnt = hzd_ln_eX_plus1(lnx);
    t = ePOW(-lnt);
  }

  mlnfz = (z*z*0.5e0)+Params.lsr2pi;

  sum = ZERO;
  for(j=4; j>=0; j--)
    sum = t*(Params.normlb[j]+sum);
  lsum = LOGe(sum);

  lnr = lsum-mlnfz;
  if(normal>LnLim.absMin)
    normal = ePOW(lnr);
  else
    normal = ZERO;
  if(z<ZERO)
    normal = ONE-normal;
  return normal;
}
