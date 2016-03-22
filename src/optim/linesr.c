#include <string.h>
#include <math.h>

#include "common.h"

#include "dnmr2.h"
#include "dqdot.h"
#include "rquad.h"
#include "fdgrad.h"

void LINESR(double *xc,double *xplus,double *gc,double *gplus,double fc,
	    double *fplus,double *p,double *wk,double *sx,double maxstp,
	    double stptol,void (*fn)(double *,double *,int),
	    void (*grad)(double *,int),double eta,int n,long *iret,
	    logical *maxtkn,logical angrad,logical newton)

     /***********************************************************************

    THE PURPOSE OF THIS ROUTINE IS TO PERFORM THE LINE SEARCH SUBSTEP
    OF THE CURRENT ITERATION OF THE QUASI-NEWTON METHOD. GIVEN GC'*P<0
    ALP < 0.5  (ALP=1.0E-4 IS USED) AND BETA IN (ALP,1) (BETA = 0.9 IS
    USED),FIND XPLUS = XC + LAMBDA*P , LAMBDA > 0  SUCH THAT

       F(XPLUS) .LE. F(XC) + ALP*LAMBDA*GC'*P      AND

       GRAD(F(XPLUS))*P .GE. BETA*GC'*P

    THIS IS ACCOMPLISHED BY MEANS OF A BACK-TRACKING LINE SEARCH

    THE ARGUMENTS OF THE SUBROUTINE HAVE THE FOLLOWING MEANINGS:

         XC().......INPUT N-VECTOR CONTAINING CURRENT PARAMETER
                    ESTIMATES.

         XPLUS()....OUTPUT N-VECTOR WHICH ON A SUCCESSFUL STEP CONTAINS
                    THE NEW PARAMETER ESTIMATES FOR THE NEXT ITERATION

         GC().......INPUT N-VECTOR,GRADIENT OF THE OBJECTIVE FUNCTION
                    AT XC.

         GPLUS()....OUTPUT N-VECTOR WHICH ON A SUCCESSFUL STEP CONTAINS
                    THE GRADIENT OF THE OBJECTIVE FUNCTION AT XPLUS.

         FC.........ON INPUT,THE VALUE OF THE OBJECTIVE FUNCTION AT XC

         FPLUS......ON OUTPUT, THE VALUE OF THE OBJECTIVE FUNCTION AT
                    XPLUS

         P()........INPUT N-VECTOR CONTAINING CURRENT SEARCH DIRECTION

         WK().......REAL WORK VECTOR OF DIMENSION AT LEAST N

         SX().......INPUT N-VECTOR CONTAINING THE DIAGONAL ELEMENTS OF
                    THE SCALING MATRIX DX.

         MAXSTP.....INPUT PARAMETER WHICH SETS THE MAXIMUM STEP TO BE
                    ATTEMPTED

         STPTOL.....INPUT PARAMETER USED TO CALCULATE THE MINIMUM STEP
                    LENGTH.

         FN.........NAME OF EXTERNAL SUBROUTINE WHICH CALCULATES
                    VALUES OF THE OBJECTIVE FUNCTION

         GRAD.......NAME OF EXTERNAL SUBROUTINE WHICH CALCULATES THE
                    GRADIENT OF THE OBJECTIVE FUNCTION WHEN THE
                    VARIABLE ANGRAD IS .TRUE.

         ETA........ON INPUT, ETA = 10**-DIGITS WHERE DIGITS IS THE
                    NUMBER OF RELIABLE BASE TEN DIGITS IN THE COMPUTED
                    VALUE OF THE OBJECTIVE FUNCTION.

         N..........THE NUMBER OF PARAMETERS BEING OPTIMIZED

         IRET.......RETURN CODE WITH FOLLOWING MEANINGS:
                    IRET = 0, NORMAL RETURN-SATISFACTORY XPLUS FOUND
                    IRET = 1, ROUTINE FAILED TO FIND A SATISFACTORY
                              XPLUS SUFFICIENTLY DIFFERENT FROM XC

         MAXTKN.....ON OUTPUT, A LOGICAL VARIABLE WHICH IS .TRUE. WHEN
                    THE MAXIMUM NEWTON STEP WAS TAKEN (LAMBDA = 1 AND
                    NEWTON STEPLENGTH .LE. MAXSTP). OTHERWISE .FALSE.

         ANGRAD.....LOGICAL VARIABLE USED GLOBALLY TO INDICATE WHEN
                    AN ANALYTIC GRADIENT IS BEING USED.
                    ANGRAD = .TRUE.  ANALYTIC GRADIENT
                    ANGRAD = .FALSE.,FINITE DIFFERENCE GRADIENT

         NEWTON.....LOGICAL VARIABLE USED GLOBALLY TO INDICATE THAT
                    NEWTONS METHOD IS BEING USED.
                    NEWTON = .TRUE. THEN NEWTONS METHOD
                    NEWTON = .FALSE. THEN SECANT METHOD

    REF: DENNIS AND SCHNABEL,"NUMERICAL METHODS FOR UNCONSTRAINED
               OPTIMIZATION AND NONLINEAR EQUATIONS",PRENTICE-HALL
               (1983)

    IMPLEMENTOR - C.R. KATHOLI

    LATEST REVISION - NOVEMBER 1990

    SUBROUTINES/FUNCTIONS CALLED - RQUAD

***********************************************************************/

{
  double scale,newtln,initsl,rellen,minlam,lambda,ltemp;
  double a,b,newslp,maxlam,lamlo,lamdif,laminc,flo,fhi;
  double svect[3],smat[3][3];
  int i;
  logical qmodel;

  static double lamprv = 0,fpprv = 0;

  qmodel = FALSE;
  *maxtkn = FALSE;
  *iret = 2;

  /* CALCULATE NEWTLN =(2-NORM(SX*P))...USE XPLUS AS A WORK SPACE */
  for(i=0; i<n; i++){
    xplus[i] = sx[i]*p[i];
  }
  newtln = DNMR2(xplus,n,1);
  if(newtln>maxstp) {
    /* STEP XPLUS=XC+P TOO LONG...RESCALE P */
    scale = maxstp/newtln;
    for(i=0; i<n; i++)
      p[i] *= scale;
    newtln = maxstp;
  }
  initsl = DQDOT(gc,p,n);
  rellen = -ONE;
  for(i=0; i<n; i++) {
    scale = fabs(p[i])/
      (fabs(xc[i])>(ONE/sx[i]) ? fabs(xc[i]) : (ONE/sx[i]));
    if(scale>rellen)
      rellen = scale;
  }

  /* MINLAM IS THE SMALLEST ALLOWABLE STEP */
  minlam = stptol/rellen;

  /**
     BEGIN OUTER LOOP TO DETERMINE LAMBDA WHICH SATISFIES BOTH THE
     ALPHA AND BETA CONDITIONS.
  **/
  lambda = ONE;
  for(;;) {
    for(i=0; i<n; i++)
      xplus[i] = xc[i]+lambda*p[i];
    (*fn)(xplus,fplus,n);
    if(Common.errorno)
      return;


    if(*fplus<=(fc+Params.lsralp*lambda*initsl))
      break;/* EXIT OUTER LOOP */
    /* ELSE ALPHA CONDITION FAILED */

    /* Changed lambda<=minlam to lambda<minlam */
    if(lambda<minlam) {
      /* NO STEP CAN BE TAKEN...ABORT */
      *iret = 1;
      for(i=0; i<n; i++)
	xplus[i] = xc[i];
      return;
    }

    if(lambda==ONE) {
      qmodel = TRUE;
      ltemp = -initsl/(TWO*((*fplus-fc)-initsl));
    } else {
      smat[1][1] = (ONE/lambda)/lambda;
      smat[1][2] = (-ONE/lamprv)/lamprv;
      smat[2][1] = -lamprv*smat[1][1];
      smat[2][2] = -lambda*smat[1][2];
      svect[1] = (*fplus-fc)-lambda*initsl;
      svect[2] = (fpprv-fc)-lamprv*initsl;
      scale = ONE/(lambda-lamprv);
      a = scale*(smat[1][1]*svect[1]+
		 smat[1][2]*svect[2]);
      b = scale*(smat[2][1]*svect[1]+
		 smat[2][2]*svect[2]);

      /**
	 SOLVE QUADRATIC POLYNOMIAL 3*A*W**2+2*B*W+INITSL = 0
	 FOR  W=(-B+SQRT(B*B-3*A*INITSL))/(3*A) WHEN  A.NE.0 AND
	 W=  -INITSL /(2*B) WHEN  A = 0
      **/
      ltemp = RQUAD(a,b,initsl);
      if(Common.errorno)
	return;
      if(ltemp>0.5e0*lambda)
	ltemp = 0.5e0*lambda;
    }

    lamprv = lambda;
    fpprv = *fplus;
    if(ltemp<=0.1e0*lambda)
      lambda = 0.1e0*lambda;
    else
      lambda = ltemp;
  }/* END OUTER LOOP */

  /**
     AT STMNT 300, ALPHA CONDITION SATISFIED, TRY TO
     SATISFY BETA CONDITION
  **/

  if(angrad)
    (*grad)(gplus,n);
  else
    FDGRAD(xplus,*fplus,sx,gplus,fn,eta,n);
  if(Common.errorno)
    return;
  newslp = DQDOT(gplus,p,n);
  if(newslp<Params.lsrbta*initsl) {
    if(lambda==ONE && newtln<maxstp) {
      maxlam = maxstp/newtln;
      /* BEGIN REPEATED LOOP  REF # 10.3A.4.1.2 */
      do {
	lamprv = lambda;
	fpprv = *fplus;
	lambda = (TWO*lambda<maxlam) ? TWO*lambda : maxlam;
	for(i=0; i<n; i++)
	  xplus[i] = xc[i]+lambda*p[i];
	(*fn)(xplus,fplus,n);
	if(Common.errorno)
	  return;
	if(*fplus>(fc+Params.lsralp*lambda*initsl))
	  /* IT APPEARS THAT THE FOLLOWING SHOULD BE CONTINUE NOT BREAK */
	  /*             break; */
	  continue;
	if(angrad)
	  (*grad)(gplus,n);
	else
	  FDGRAD(xplus,*fplus,sx,gplus,fn,eta,n);
	if(Common.errorno)
	  return;
	newslp = DQDOT(gplus,p,n);
      } while((*fplus<=(fc+Params.lsralp*lambda*initsl)) &&
              (newslp<Params.lsrbta*initsl) &&
              (lambda<maxlam));
    }
    if((lambda<ONE) ||
       ((lambda>ONE) && (*fplus>(fc+Params.lsralp*lambda*initsl)))){
      qmodel = TRUE;
      lamlo = lambda<lamprv ? lambda : lamprv;
      lamdif = fabs(lamprv-lambda);
      if(lambda<lamprv) {
	flo = *fplus;
	fhi = fpprv;
      } else {
	flo = fpprv;
	fhi = *fplus;
      }

      /* BEGIN REPEAT LOOP,REF #10.3A.4.2.4 */
      do {
	scale = ONE/(TWO*(fhi-(flo+newslp*lamdif)));
	laminc = -newslp*(lamdif*lamdif)*scale;
	scale = 0.1e0*(lamdif+lamdif);
	if(laminc<scale)
	  laminc = scale;
	lambda = lamlo+laminc;
	for(i=0; i<n; i++)
	  xplus[i] = xc[i]+lambda*p[i];
	(*fn)(xplus,fplus,n);
	if(Common.errorno)
	  return;

	if(*fplus>(fc+Params.lsralp*lambda*initsl)) {
	  lamdif = laminc;
	  fhi = *fplus;
	} else {
	  if(angrad)
	    (*grad)(gplus,n);
	  else
	    FDGRAD(xplus,*fplus,sx,gplus,fn,eta,n);
	  if(Common.errorno)
	    return;
	  newslp = DQDOT(gplus,p,n);
	  if(newslp<(Params.lsrbta*initsl)) {
	    lamlo = lambda;
	    lamdif = lamdif-laminc;
	    flo = *fplus;
	  }
	}
      } while(!((newslp>=Params.lsrbta*initsl) || (lamdif<minlam)));
    }
  }

  /**
     NORMAL EXIT - Alpha and Beta conditions satisfied.
     If LAMBDA=1 and .NOT.Newton method check to see if a single quadratic
     interpolation will improve Fn and have Alpha and Beta still satisfied.
     This modification necessary so that true quadratic model is efficiently
     found.
  **/
  *iret = 0;
  if(newton) {
    if((lambda*newtln)>(0.99e0*maxstp))
      *maxtkn = TRUE;
    return;
  }
  /* AT THIS POINT WE REMOVE THE DEBUG AND UNCOMMENT THE FOLLOWING * 
   *debug* 
   ADDITIONALLY WE ADD AN ELSE STATEMENT AND MODIFY CODE TO MORE
   CLOSELY FOLLOW THE FORTRAN SOURCE
  */
  if(!qmodel) {

    if(lambda==ONE)
      ltemp = -initsl/(TWO*((*fplus-fc)-initsl));
    else {
      lamdif = lamprv-lambda;
      scale = ONE/(TWO*((fpprv-(*fplus))-newslp*lamdif));
      laminc = -newslp*(lamdif*lamdif)*scale;
      ltemp = lambda+laminc;
    }
    fpprv = *fplus;
    for(i=0; i<n; i++)
      xplus[i] = xc[i]+ltemp*p[i];
    (*fn)(xplus,fplus,n);
    if(Common.errorno)
      return;
    if(*fplus<fpprv) {
      if(angrad)
	(*grad)(wk,n);
      else
	FDGRAD(xplus,*fplus,sx,wk,fn,eta,n);
      if(Common.errorno)
	return;
      newslp = DQDOT(wk,p,n);
      if(newslp>=(Params.lsrbta*initsl)) {
	lambda = ltemp;
	for(i=0; i<n; i++){
	  gplus[i] = wk[i];
	  p[i]*=ltemp;
	}
      } else{
	*fplus = fpprv;
	for(i=0; i<n; i++)
	  p[i]*=lambda;
      }
    } else{
      *fplus = fpprv;
      for(i=0; i<n; i++) 
	p[i]*=lambda;
    }
  }else
    for(i=0; i<n; i++) p[i]*=lambda;

  for(i=0; i<n; i++)
    xplus[i] = xc[i]+p[i];   

  if((lambda*newtln)>(0.99e0*maxstp))
    *maxtkn = TRUE;
}
