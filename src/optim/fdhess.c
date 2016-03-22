#include <string.h>
#include <math.h>

#include "common.h"
void FDHESS(double *xc,double *sx,double *H,double *stpsiz,
	    double *fneigh, double fc,
	    void (*fn)(double *,double *,int),int n)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO CALCULATE A FINITE DIFFERENCE
    APPROXIMATION TO THE HESSIAN MATRIX OF THE OBJECTIVE FUNCTION
    BY THE FORWARD DIFFERENCE FORMULA,

    H(I,J)=(F(XC+S(I)*E(I)+S(J)*E(J))-F(XC+S(I)*E(I))-F(XC+S(J)*E(J))

               + F(XC))/(S(I)*S(J))

    WHERE E(I) IS THE I-TH UNIT COORDINATE VECTOR

           S(I)=(ETA**1/3)*MAX(ABS(XC(I)),1/SX(I))*SIGN(XC(I))

    HERE ALSO 1/SX(I) IS THE TYPICAL SIZE OF ABS(XC(I)) INPUT BY USER
    AND ETA = 10**-DIGITS, DIGITS THE NUMBER OF RELIABLE BASE 10
    DIGITS IN F(X)

    NOTE: THE ELEMENTS OF THE CALCULATED HESSIAN WILL TYPICALLY
          AGREE WITH THE TRUE VALUES IN ABOUT THE FIRST DIGITS/3
          BASE 10 DIGITS. (IN THIS SUBROUTINE, DIGITS IS ASSUMED TO BE
          ABOUT 16 AND ETA IS TAKEN TO BE 16**-13)

    THE ARGUMENTS OF THE ROUTINE HAVE THE FOLLOWING MEANINGS:

         XC()------>N-VECTOR CONTAINING THE VALUE OF X AT WHICH THE
                    HESSIAN MATRIX IS REQUIRED
         SX()------>N-VECTOR CONTAINING THE DIAGONAL ELEMENTS OF THE
                    SCALING MATRIX DX.
         H(,)<------N X N MATRIX CONTAINING THE HESSIAN ON OUTPUT.
         STPSIZ()-->N-VECTOR USED INTERNALLY FOR WORKING STORAGE
         FNEIGH()-->N-VECTOR USED INTERNALLY FOR WORKING STORAGE
         FC-------->VALUE OF THE OBJECTIVE FUNCTION AT XC().
         FN---------NAME OF EXTERNAL SUBROUTINE FOR THE CALCULATION
                    OF VALUES OF THE OBJECTIVE FUNCTION.
                           CALL FN(XC,FC,N)
         N--------->DIMENSION OF THE PROBLEM

    REF: DENNIS AND SCHNABEL, "NUMERICAL METHODS FOR UNCONSTRAINED
              OPTIMIZATION AND NONLINEAR EQUATIONS", PRENTICE-HALL,
              1983

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - AUGUST 1984
                      JUN 1996 - Restored XC even if error condition

    FUNCTIONS/SUBROUTINES CALLED -

***********************************************************************/

{
  double sg,f,tempi,tempj;
  double *hii,*hij,*hji;
  int i,j,n1;

  for(i=0; i<n; i++) {
    /* CALCULATE STPSIZ(I) AND FN(XC+STPSIZ(I)*E(I)) */
    sg = ONE;
    if(xc[i]<ZERO)
      sg = -sg;
    stpsiz[i] = Params.fdheta*sg*
      ((fabs(xc[i])>(ONE/sx[i])) ? fabs(xc[i]) : ONE/sx[i]);
    tempi = xc[i];
    xc[i] += stpsiz[i];
    stpsiz[i] = xc[i]-tempi;

    /* THE LAST FEW STATEMENTS REDUCE FINITE PRECISION ERROR SLIGHTLY */
    (*fn)(xc,&f,n);
    xc[i] = tempi;
    if(Common.errorno)
      return;
    fneigh[i] = f;
  }

  /* CALCULATE ROW I OF H */
  n1 = n+1;
  for(i=0,hii=H; i<n; i++,hii+=n1) {
    tempi = xc[i];
    xc[i] += TWO*stpsiz[i];
    (*fn)(xc,&f,n);
    if(Common.errorno){
      xc[i] = tempi;
      return;
    }
    *hii = ((fc-fneigh[i])+(f-fneigh[i]))/(stpsiz[i]*stpsiz[i]);
    xc[i] = tempi+stpsiz[i];
    for(j=i+1,hij=hii+1,hji=hii+n; j<n; j++,hij++,hji+=n) {
      tempj = xc[j];
      xc[j] += stpsiz[j];
      (*fn)(xc,&f,n);
      xc[j] = tempj;
      if(Common.errorno)
	return;
      *hij = ((fc-fneigh[i])+(f-fneigh[j]))/(stpsiz[i]*stpsiz[j]);
      *hji = *hij;
    }
    xc[i] = tempi;
  }
}
