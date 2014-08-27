#include <math.h>
#include <string.h>

#include "common.h"

#include "setobj.h"

#include "setdll.h"
#include "fdhess.h"
#include "restor.h"
#include "chlskm.h"
#include "choslv.h"
#include "normal.h"
#include "hzr_set_error.h"

void DCOVAR(double *beta,long nvar,logical noshap,double fx,long *index,
	    double *theta,double *d2ll,double *cov,double *corr, 
	    double *sd, double *znorm,double *pnorm,long *flag,
	    logical *stopt)

     /**************************************************************
   VARIANCE-COVARIANCE CALCULATIONS

   PURPOSE
     TO CALCULATE THE SECOND DERIVATIVE MATRIX WITH RESPECT TO THE
     LIKELIHOOD FUNCTION; DETERMINE IF IT IS POSITIVE DEFINITE;
     OBTAIN THE VARIANCE-COVARIANCE MATRIX; CALCULATE FROM IT THE
     CORRELATION MATRIX AND STANDARD DEVIATIONS FOR EACH PARAMETER;
     AND DETERMINE THE P-VALUE FOR EACH PARAMETER.

   DESCRIPTION OF PARAMETERS
     INPUT
       BETA   - REAL*8  NVAR-VECTOR OF PARAMETER ESTIMATES, REQUIRED
                        IF THERE ARE SHAPING PARAMETERS, OTHERWISE
                        IT CAN BE A DUMMY VARIABLE
       NVAR   - INTEGER NUMBER OF ESTIMATES IN BETA
       NOSHAP - LOGICAL FLAG INDICATING MODE OF OPERATION
                        .TRUE.  - NO SHAPING PARAMETERS ARE
			             CONSIDERED
                        .FALSE. - CONSIDER SHAPING PARAMETERS
       FX     - REAL*8  CURRENT VALUE OF OBJECTIVE FUNCTION, REQUIRED
                        IF THERE ARE SHAPING PARAMETERS, OTHERWISE
                        IT CAN BE A DUMMY VARIABLE
       INDEX  - INTEGER NVAR-VECTOR OF POINTERS TO THETA
                        CORRESPONDING EXACTLY TO EACH BETA
       THETA  - REAL*8  NTHETA-VECTOR OF PARAMETER ESTIMATES
       STATUS - INTEGER NTHETA-VECTOR OF STATUS FLAGS FOR EACH THETA

     OUTPUT
       D2LL   - REAL*8  NVAR*NVAR MATRIX OF FACTORED DERIVATIVES
       COV    - REAL*8  NVAR*NVAR MATRIX OF COVARIANCE TERMS
       CORR   - REAL*8  NVAR*NVAR MATRIX OF CORRELATION TERMS
       SD     - REAL*8  NVAR-VECTOR OF STANDARD DEVIATIONS
       ZNORM  - REAL*8  NVAR-VECTOR OF TEST STATISTICS
       PNORM  - REAL*8  NVAR-VECTOR OF P-VALUES
       FLAG   - INTEGER NVAR-VECTOR OF FLAGS FOR EACH ESTIMATE:
                        0 - NORMAL STATUS
                        1 - SECOND DERIVATIVE NOT POSITIVE DEFINITE
                        2 - E >0, MEANING THIS VARIABLE MAY BE NEAR
                            SINGULAR IN PRESENCE OF OTHER PARAMETERS
                            AND IS ILL-CONDITIONED
       STOPT  - LOGICAL FLAG INDICATING COMPLETENESS OF TASK
                        .FALSE. - EVERYTHING WAS DONE AND IS O.K.
                        .TRUE.  - WE STOPPED AND FLAG TELLS YOU WHY

   DESCRIPTION OF OTHER PARAMETERS USED BY OR CREATED BY SUBROUTINE
     INPUT
       PHASE  - INTEGER 3-VECTOR OF PHASE FLAGS
       FIXG1  - LOGICAL IF .TRUE. EARLY SHAPING FUNCTION IS FIXED
       FIXG3  - LOGICAL IF .TRUE. LATE SHAPING FUNCTION IS FIXED

     OUTPUT
       ERROR  - INTEGER ERROR FLAG
                        0 - NO ERROR
                        1 - ARITHMETIC OVERFLOW (FATAL ERROR)
                        2 - PROCESSING ERROR (EG, MISMATCHES WITH
                            NVAR OR WITH PHASES)
       ERRFLG - CHARACTER*12 ERROR MESSAGE

   REMARKS
     1.  IF WE ARE IN A STEPWISE MODE OR HAVE ONLY INTERCEPTS AND
         CONCOMITANT VARIABLES, WE USE ANALYTICAL DERIVATIVES AS
         SET UP FOR US USING SETDLL.  HOWEVER, IF THERE ARE
         SHAPING PARAMETERS, WE MUST USE NUMERIC SECOND DERIVATIVES.
         THIS REQUIRES THE USE OF FDHESS WITH ITS VARIOUS
         ASSOCIATED INFORMATION.

     2.  IF WE ARE USING ANALYTICAL DERIVATIVES, THEN SETDLL MUST BE
         PROPERLY INITIALIZED BEFORE CALLING THIS ROUTINE.

     3.  BETA IS NOT DISTURBED BY THE SUBROUTINE, AND IF MODEL
         PARAMETERS HAVE BEEN DISTURBED, THEY ARE RESTORED.

   SUBROUTINES REQUIRED
     CHLSKM - CHOLESKY FACTORIZATION OF SECOND DERIVATIVES
     CHOSLV - SOLUTION OF EQUATIONS GIVEN FACTORED MATRIX
     FDHESS - CALCULATE SECOND DERIVATIVES NUMERICALLY
     MACHIN - DETERMINE FLOATING POINT ENVIRONMENT OF COMPUTER
     NORMAL - P-VALUE FROM NORMAL DISTRIBUTION
     RESTOR - RESTORE MODEL PARAMETERS AFTER BEING DISTURBED
     SETDLL - CALCULATE SECOND DERIVATIVES ANALYTICALLY

   METHOD
     1.  SECOND DERIVATIVE MATRIX IS DETERMINED.  WE CHECK TO SEE IF
         IT IS POSITIVE DEFINITE.

     2.  THE SECOND DERIVATIVE MATRIX IS FACTORED USING CHOLESLY
         FACTORIZATION.  IF THE MATRIX IS NEAR SINGULAR IT IS 'FIXED'
         AND THESE APPEAR IN THE VECTOR 'E'.  IN THIS VERSION, WE
         DO NOT PROCEED FURTHER IF THIS HAPPENS.

     3.  THE VARIANCE COVARIANCE MATRIX IS BUILT UP A TERM AT A TIME
         USING THE FACTORIZATION AND THE SOLUTION SUBPROGRAM.

     4.  THE CORRELATION MATRIX IS BUILT USING THE RELATION THAT

                COV(1,2) = SD(1)*SD(2)*CORR(1,2)

     5.  THE ASYMPTOTIC P-VALUE IS FOUND USING THE NORMAL 
         DISTRIBUTION AND THE RELATION:

                Z = BETA/SD

     6.  THE OUTPUT VECTORS ARE USED INTERNALLY FOR ALL INTERMEDIATE
         WORKING STORAGE.  THEY ARE NOT CLEARED ON OUTPUT.

   IMPLEMENTATION
     E.H.BLACKSTONE, MD   FEBRUARY, 1986
     FROM INSTRUCTIONS OF C.R. KATHOLI, PH.D

   REVISIONS
     SEPT. 14,1987 - REMOVED UNNECESSARY RESTRICTION ON FX

   VERIFICATION
     APRIL 15,1986 - TESTED AGAINST PREVIOUS KATHOLI PROGRAM

     ***************************************************************/

{
  double *cov_i,*cov_ij,*corr_i,*corr_j,*corr_ij,*corr_ji,*d2ll_ii;
  double absz;
  long i,j,nvar1,stmode;

  /* INITIALIZATION */
  *stopt = FALSE;
  if(nvar<=0) {
    hzr_set_error("(DCOVAR900)",2);
    return;
  } else {
    Common.errorno = 0;
    CLEAR(Common.errflg);
  }
  nvar1 = nvar+1;

  /*-----------------------------------------------------------------
    CALCULATE THE SECOND DERIVATIVES WITH RESPECT TO THE LOG
    LIKELIHOOD FUNCTION
    ---------------------------------------------------------------*/

  if(noshap) {
    /*
      IF THERE ARE NO SHAPING PARAMETERS, GET ANALYTIC 
      DERIVATIVES
    */
    d2lcon(d2ll);
    if(Common.errorno)
      return;
  } else {
    /* OTHERWISE SET UP FOR NUMERIC DERIVATIVES */
    for(i=0; i<nvar; i++) {
      if(beta[i]==ZERO) {
	hzr_set_error("(DCOVAR920)",2);
	return;
      }
      /**
	 SAVE INCOMING BETAS, SINCE THEY WILL BE CHANGED IN FINDING
	 DERIVATIVES
      **/
      cov[i] = beta[i];
      corr[i] = ONE/fabs(beta[i]);
    }

    /* TURN OFF CONSERVATION OF EVENTS (WILL BE RESTORED LATER) */
    stmode = HZRstr.mode;
    HZRstr.mode = 2;

    /* GET NUMERIC DERIVATIVES */
    FDHESS(cov,corr,d2ll,znorm,pnorm,fx,DHAZRD,nvar);

    /**
       RESTORE PROCEDURE TO ITS ORIGINAL STATE, AND DO SO BEFORE
       SEEING IF THERE WAS AN ERROR.
    **/
    RESTOR(beta,nvar,index,theta);
    HZRstr.mode = stmode;

    if(Common.errorno)
      return;
  }

  /**
     CHECK TO SEE IF ANY OF THE DIAGONAL ELEMENTS OF THE MATRIX IS
     ZERO OR NEGATIVE.  IF SO, THE MATRIX IS SINGULAR.  FLAG THE
     POSSIBLE CULPRITS.  ALSO SET UP SIMULTANEOUSLY THE SCALING 
     MATRIX SX OF ONES.
  **/
  for(i=0,d2ll_ii=d2ll; i<nvar; i++,d2ll_ii+=nvar1) {
    corr[i] = ONE;
    if((*d2ll_ii)<=ZERO) {
      flag[i] = 1;
      *stopt = TRUE;
    } else
      flag[i] = 0;
  }

  /*----------------------------------------------------------------
    DO CHOLESKY FACTORIZATION OF THE MATRIX OF SECOND DERIVATIVES
    FUNCTION AND OBTAIN THE VARIANCE-COVARIACE MATRIX
    --------------------------------------------------------------*/

  CHLSKM(d2ll,sd,znorm,corr,nvar,0);

  /**
     DETERMINE IF ANY E>0, INDICATIVE OF HAVING TO FIX UP THE MATRIX
     TO MAKE IT LESS ILL-CONDITIONED
  **/
  for(i=0; i<nvar; i++)
    if(znorm[i]>ZERO) {
      *stopt = TRUE;
      if(flag[i]==0)
	flag[i] = 2;
    }
  if(*stopt)
    return;

  /* NOW, COLUMN-BY-COLUMN, GET THE VARIANCE-COVARIANCE MATRIX */
  for(i=0,cov_i=cov; i<nvar; i++,cov_i+=nvar) {
    for(j=0; j<nvar; j++)
      if(j==i)
	pnorm[j] = -ONE;
      else
	pnorm[j] = ZERO;
    CHOSLV(d2ll,pnorm,cov_i,nvar);
  }


  /*----------------------------------------------------------------
    NOW WE CAN GET STANDARD DEVIATIONS, CORRELATION MATRIX, AND
    P-VALUES
    ---------------------------------------------------------------*/

  /* GET STANDARD DEVIATIONS */
  for(i=0,cov_i=cov; i<nvar; i++,cov_i+=nvar1)
    sd[i] = sqrt(*cov_i);

  /* FORM CORRELATION MATRIX */
  for(i=0,cov_i=cov,corr_i=corr,corr_j=corr; i<nvar;
      i++,cov_i+=nvar,corr_i+=nvar,corr_j++)
    for(j=0,cov_ij=cov_i,corr_ij=corr_i,corr_ji=corr_j; j<nvar;
	j++,cov_ij++,corr_ij++,corr_ji+=nvar)
      if(j==i)
	*corr_ij = ONE;
      else
	*corr_ij = *corr_ji = (*cov_ij)/(sd[i]*sd[j]);

  /* FORM TEST STATISTIC Z AND ITS P-VALUE */
  for(i=0; i<nvar; i++) {
    znorm[i] = beta[i]/sd[i];
    absz = fabs(znorm[i]);
    pnorm[i] = TWO*NORMAL(absz);
  }
}
