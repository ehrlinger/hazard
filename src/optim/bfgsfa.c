#include <string.h>
#include <math.h>

#include "common.h"

#include "dqdot.h"
#include "dnmr2.h"
#include "qrupdt.h"
#include "hzr_qextd.h"        
#include "hzr_qsum.h"
#include "hzr_qmul.h"     
#include "hzr_qtod.h"

/* 
   static void BFGSFA(double *m,double *xc,double *xplus,
   double *gc,double *gplus, double *s,double *y,double eta,logical
   angrad,int n)
*/
void BFGSFA(double *m,double *gc,double *gplus, double *s,
	    double *y, double eta,logical angrad,int n)

     /**************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO UPDATE THE L*L' FACTORIZATION
    OF THE HESSIAN MATRIX,H,TO THE (L+)*(L+)' FACTORIZATION  OF

    H - HSS'H/S'HS + YY'/Y'S = JJ' WHERE J IS DEFINED TO BE

    J = L +((Y-ALPHA*HS)(ALPHA*L'S)')/(Y'S) ,
    ALPHA =SQRT((Y'S)/(S'HS))

    AND S =XPLUS -XC , Y = GPLUS-GC .

      THE UPDATE IS NOT PERFORMED IF :

     (1). Y'S < SQRT(MACHEP)*(2-NORM(S))*(2-NORM(Y))

     OR

     (2). FOR EVERY I, 
          ABS((Y-L*L'S)(I))<TOL*(ABS(GC(I))+ABS(GPLUS(I)))

                       | ETA WHEN ANGRAD = TRUE (ANALYTIC GRADIENT )
          WHERE  TOL = |
                       | SQRT(ETA) WHEN ANGRAD = FALSE

    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

          M(,)......ON INPUT,THE N X N MATRIX M CONTAINS THE LOWER
                    TRIANGULAR MATRIX L IN THE LOWER TRIANGLE AND
		      MAIN DIAGONAL.
                    ON OUTPUT, M CONTAINS L+

          XC()......N -VECTOR CONTAINING PARAMETER ESTIMATES FROM
                    LAST ITERATION.

          XPLUS()...N -VECTOR CONTAINING NEW PARAMETER ESTIMATES JUST
                    CALCULATED IN THIS ITERATION

          GC()......N -VECTOR CONTAINING GRADIENT VECTOR OF  THE
                    OBJECTIVE FUNCTION AT XC.

          GPLUS()...N-VECTOR CONTAINING GRADIENT VVECTOR OF  THE
                    OBJECTIVE FUNCTION  AT XPLUS

          S(),Y()...N-VECTORS USED AS WORK STORAGE BY THE ROUTINE

          MACHEP....MACHINE EPSILON (SMALLEST FLOATING POINT NUMBER
                    SUCH THAT 1 + MACHEP > 1 )

          ETA.......ESTIMATE OF THE RELATIVE ERROR IN CALCULATING THE
                    OBJECTIVE FUNCTION .(NEVER SMALLER THAN MACHED  )

          ANGRAD....LOGICAL VARIABLE WHERE ANGRAD =
                      .TRUE. - WHEN ANALYTIC GRADIENT IS USED
                      .FALSE.- WHEN A FINITE DIFFERENCE GRADIENT IS
                               USED

          N.........ROW/COLUMN DIMENSION OF M.ALSO DIMENSION OF 
	             XC,ET.

    REF: DENNIS AND SCHNABEL,"NUMERICAL METHODS FOR UNCONSTRAINED
               OPTIMIZATION AND NONLINEAR EQUATIONS",PRENTICE-HALL
               (1983)

    
    IMPLEMENTOR - C.R. KATHOLI

    SOURCE      - optim91/uncmin91/bfgsfa.for

    LATEST REVISION - NOVEMBER 1990

    FUNCTIONS/SUBROUTINES CALLED - QRUPDT,DQDOT,DSQRT,QEXTD,
                                   DMAX1,DNMR2,DABS

********************************************************************/

{
  xtended qsum;
  double temp1,temp2,temp3,tol,gcgpmax,alpha;
  double *mi,*mii,*mji,*mij,*miij,*miji;
  int i,j,n1;
  logical skipud;

  /**
     The statement: s[i] = xplus[i]-xc[i]
     was removed from the following loop because subroutine LINESR 
     now returns S(I)=XPLUS(I)-XC(I) as LAMBDA*P(I) which should
     have better accuracy for XC near the solution.
  **/
  for(i=0; i<n; i++)
    y[i] = gplus[i]-gc[i];
  temp1 = DQDOT(y,s,n);
  temp2 = DNMR2(s,n,1);
  temp3 = DNMR2(y,n,1);

  /* PERFORM FIRST TEST TO SEE IF UPDATE SHOULD BE ATTEMPTED */
  if(temp1<(sqrt(Machn.Eps)*temp2*temp3))
    return;

  /* PERFORM UPDATE */
  n1 = n+1;
  for(i=0,mii=m; i<n; i++,mii+=n1) {
    qsum = QEXTD(ZERO);
    for(j=i,mji=mii; j<n; j++,mji+=n)
      qsum = QSUM(qsum,QMUL(QEXTD(*mji),QEXTD(s[j])));
    s[i] = QTOD(qsum);
  }
  temp2 = DQDOT(s,s,n);
  alpha = sqrt(temp1/temp2);

  if(!angrad)
    tol = sqrt(eta);
  else
    tol = eta;
  skipud = TRUE;

  for(i=0,mi=m; i<n; i++,mi+=n) {
    qsum = QEXTD(ZERO);
    for(j=0,mij=mi; j<=i; j++,mij++)
      qsum = QSUM(qsum,QMUL(QEXTD(*mij),QEXTD(s[j])));
    temp3 = QTOD(qsum);
    gcgpmax = fabs(gc[i])>fabs(gplus[i]) ?
      fabs(gc[i]) : fabs(gplus[i]);
    if(fabs(y[i]-temp3)>=(tol*gcgpmax))
      skipud = FALSE;
    y[i] -= alpha*temp3;
  }
  if(skipud)
    return;

  temp3 = ONE/sqrt(temp1*temp2);
  for(i=0; i<n; i++)
    s[i] *= temp3;

  /**
     COPY L' INTO UPPER TRIANGLE OF M. AT THE SAME TIME ZERO THE
     ELEMENTS BELOW THE DIAGONAL ELEMENTS OF M.THIS IS NECESSARY IN
     ORDER FOR SUBROUTINE QRUPDT TO FUNCTION CORRECTLY.
  **/
  for(i=0,miij=m,miji=m; i<n; i++,miij+=(n+1),miji+=(n+1))
    for(j=i+1,mij=miij+1,mji=miji+n; j<n; j++,mij++,mji+=n) {
      *mij = *mji;
      *mji = ZERO;
    }

  /* CALCULATE THE QR FACTORIZATION OF  L' + SY' */
  QRUPDT(m,s,y,n);

  /* COPY UPPER TRIANGLE OF M TO L */
  for(i=0,miij=m,miji=m; i<n; i++,miij+=(n+1),miji+=(n+1))
    for(j=i+1,mij=miij+1,mji=miji+n; j<n; j++,mij++,mji+=n)
      *mji = *mij;
}
