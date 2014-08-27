#include <math.h>

#include "common.h"

double DNMR2(double *sx,long n,long incx)

/***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO CALCULATE THE EUCLIDEAN NORM OF
    THE N-VECTOR STORED IN SX() WITH STORAGE INCREMENT INCX. THE
    METHOD USED IS THAT USED BY LAWSON,ET AL. IN THE BLAS PACKAGE -
    TOMS 1979.

    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

          SX()......DOUBLE PRECISION   VECTOR FOR WHICH THE NORM IS
                    TO BE CALCULATED.

          N.........NUMBER OF POINTS FROM SX() TO BE USED.

          INCX......SPACING BETWEEN POINTS IN SX() FOR CALCULATION

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - JULY 1984

    FUNCTIONS/SUBROUTINES CALLED - DABS,DFLOAT,DSQRT

***********************************************************************/

{
   double sum,xmax,hitest;
   long i,j,next,nn;
   logical do85;

   if(n<=0)
      return ZERO;

   sum = ZERO;
   hitest = Params.dnmrhi/(double)n;
   nn = n*incx;
   xmax = ZERO;
   do85 = FALSE;
   for(i=0,next=50; i<nn; i+=incx) {
      switch(next) {
         case 50:
            if(sx[i]==ZERO)
               continue;
            if(fabs(sx[i])>Params.dnmrlo) {
               do85 = TRUE;
               break;
            }
            next = 70;
            xmax = fabs(sx[i]);
            break;
         case 70:
            if(fabs(sx[i])>Params.dnmrlo) {
               sum = (sum*xmax)*xmax;
               do85 = TRUE;   
            }
	    break;
         case 110:
            if(fabs(sx[i])>xmax) {
               sum = ONE+sum*((xmax/sx[i])*(xmax/sx[i]));
               xmax = fabs(sx[i]);
               continue;
            }
      }
      if(do85) {
         for(j=i; j<nn; j+=incx) {
            if(fabs(sx[j])>=hitest) {
               i = j;
               next = 110;
               sum = (sum/sx[i])/sx[i];
               xmax = fabs(sx[i]);
               break;
            }
            sum += sx[j]*sx[j];
         }
         if(next!=110)
            return sqrt(sum);
         do85 = FALSE;
      }
      sum += (sx[i]/xmax)*(sx[i]/xmax);
   }
   return xmax*sqrt(sum);
}
