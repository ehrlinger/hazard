#include <math.h>
#include <string.h>

#include "common.h"

#include "normal.h"

#include "hzr_set_error.h"

void DFAST(double *beta,long nvar,double *cov,long ivar,double *sd,
	   double *znorm,double *pnorm)

     /***********************************************************************

   FAST BACKWARD ELIMINATION

   PURPOSE
     TO EFFICIENTLY SCREEN A REGRESSION MODEL BY APPROXIMATE UPDATING
     OF THE COEFFICIENTS AFTER ELIMINATION OF THE IVAR'TH VARIABLE
     FROM THE MODEL.

   DESCRIPTION OF PARAMETERS
     INPUT
       BETA   - REAL*8  NVAR-VECTOR OF PARAMETER ESTIMATES, CONSISTING
                        OF INTERCEPTS AND COEFFICIENTS FOR RISK
                        COVARIATES.  ON OUTPUT, IT IS THE NVAR-1
                        VECTOR OF NEW ESTIMATES.
       NVAR   - INTEGER NUMBER OF ESTIMATES IN BETA
       COV    - REAL*8  NVAR*NVAR VECTOR OF VARIANCE-COVARIANCE
                        ESTIMATES FOR THE BETA'S.  ON OUTPUT IT WILL
                        CONTAIN A COMPACTED VERSION OF COV OF SIZE
                        (NVAR-1)*(NVAR-1).
       IVAR   - INTEGER INDEX NUMBER OF THE BETA TO ELIMINATE (IT MUST
                        HAVE A VALUE BETWEEN 1 AND NVAR)

     OUTPUT
       SD     - REAL*8  (NVAR-1)-VECTOR OF STANDARD DEVIATION OF BETA
       ZNORM  - REAL*8  (NVAR-1)-VECTOR OF TEST STATISTICS
       PNORM  - REAL*8  (NVAR-1) VECTOR OF P-VALUES

   DESCRIPTION OF OTHER PARAMETERS USED BY OR CREATED BY SUBROUTINE
     INPUT
       NONE

     OUTPUT
       ERROR  - INTEGER ERROR FLAG
                        0 - NO ERROR
                        1 - ARITHMETIC OVERFLOW (FATAL ERROR)
                        2 - PROCESSING ERROR (EG, MISMATCHES WITH
                            NVAR OR WITH PHASES)
       ERRFLG - CHARACTER*12 ERROR MESSAGE

   REMARKS
     1.  BETA AND COV ARE UPDATED BY THE SUBROUTINE.  THIS MEANS THAT
         THE ARRAY COV SHOULD NOT HAVE FIXED TWO DIMENSIONAL
         ARGUMENTS, BUT SHOULD BE A SINGLE DIMENSIONED ARRAY OF
         AT LEAST NVAR*NVAR LENGTH.  THIS PERMITS THE VARIABLE
         DIMENSIONING IMPLICIT IN THIS SUBROUTINE.

     2.  SUGGESTED USAGE.
         A.  USE THIS ROUTINE TO UPDATE BETAS FOR STARTING VALUES IN
             IN A STANDARD STEPWISE BACKWARD ELIMINATION SCHEME.

         B.  USE THIS ROUTINE AS A FAST SCREENING PROCEDURE.  THE
             COMPLETE MODEL WILL ALL VARIABLES OF INTEREST WOULD
             BE ESTIMATED IN THE USUAL FASHION.  THEN, THE PROCEDURE
             WOULD BE PROGRAMMED TO DECIDE (E.G. ON THE BASIS OF THE
             P-VALUE) WHICH OF THE BETAS SHOULD BE ELIMINATED (IVAR).
             DFAST WOULD THEN BE CALLED TO UPDATE THE REMAINING
             BETAS AND TO CALCULATE NEW P-VALUES FOR THEN.  THE
             PROCESS WOULD BE REPEATED UNTIL SOME STOPPING CRITERION
             HAS BEEN MET.

     3.  N O T E:  IT IS THE RESPONSIBILITY OF THE CALLING PROGRAM TO
                   UPDATE, EVENTUALLY, THE THETA AND STATUS VECTOR
                   AND, IF NECESSARY TO CALL DTRNFM TO UPDATE MODEL
                   PARAMETERS.

   SUBROUTINES REQUIRED
     NORMAL - P-VALUE FROM NORMAL DISTRIBUTION

   METHOD
     1.  SEE LAWLESS JF, SINGHAL K:  EFFICIENT SCREENING OF NONNORMAL
         REGRESSION MODELS.  BIOMETRICS 34:318-327, 1978  AND
         KUK AYC:  ALL SUBSET REGRESSION IN A PROPORTIONAL HAZARDS
         MODEL.  BIOMETRIKA 71:587-592, 1984.

     2.  BETA(I,NEW) = BETA(I,OLD) - COV(I,IVAR)*BETA(IVAR)/VAR(IVAR)

     3.  IT IS ASSUMED THAT THE IVAR'TH ROW OF THE VARIANCE-COVARIANCE
         MATRIX IS SIMPLY SET TO ZERO, SO THAT THE OLD STANDARD
         DEVIATIONS ARE APPROPRIATE TO FORM NEW Z-STATISTICS AND
         NEW P-VALUES.

   IMPLEMENTATION
     E.H.BLACKSTONE, MD   APRIL, 1986

   REVISIONS
     NONE

   VERIFICATION
     APRIL 26,1986 - TESTED AGAINST NAFTEL HAND CALCULATIONS

***********************************************************************/

{
  double *iivar,*covii,*covij,*coviijj;
  double betai,vari,bivi,absz;
  long i,j,im1var,nvar1;

  /* INITIALIZATION */
  if(nvar<=0 || ivar<0 || ivar>nvar) {
    hzr_set_error("(DFAST900)",2);
    return;
  } else {
    Common.errorno = 0;
    CLEAR(Common.errflg);
  }
  im1var = ivar*nvar;
  nvar1 = nvar+1;


  /*-------------------------------------------------------------------
    CALCULATE NEW BETAS, REFORM THE STANDARD DEVIATION, CALCULATE NEW
    Z-STATISTICS, AND CALCULATION NEW P-VALUES.  STORE THESE IN
    VECTORS OF LENGTH NVAR-1.
    -------------------------------------------------------------------*/

  betai = beta[ivar];
  i = ivar+im1var;
  vari = cov[i];
  if(vari<=ZERO) {
    hzr_set_error("(DFAST910)",2);
    return;
  }
  bivi = betai/vari;

  /* NOW LOOP OVER ALL BETA */
  j = 0;
  for(i=0,iivar=cov+im1var,covii=cov; i<nvar;
      i++,iivar++,covii+=nvar1) {
    if(i==ivar)
      continue;
    beta[j] = beta[i]-(*iivar)*bivi;
    if((*covii)<=ZERO) {
      hzr_set_error("(DFAST920)",2);
      return;
    }
    sd[j] = sqrt(*covii);
    znorm[j] = beta[j]/sd[j];
    absz = fabs(znorm[i]);
    pnorm[j] = TWO*NORMAL(absz);
    j++;
  }

  /*-------------------------------------------------------------------
    NOW COMPACT THE COVARIANCE MATRIX TO (NVAR-1)*(NVAR-1)
    -------------------------------------------------------------------*/

  covij = coviijj = cov;
  for(j=0; j<nvar; j++)
    for(i=0; i<nvar; i++,coviijj++)
      if(j!=ivar && i!=ivar)
	*(covij++) = *coviijj;
}
