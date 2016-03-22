#include <string.h>
#include "common.h"

#include "dqstat.h"

void SWVARQ(short int *status,double *pvalue,double *zvalue,
	    double *stdErr,int *flags,double *qx, double *qtols,
	    double *d2ll,int nvar,double *wk, double *d2llad)

     /*----------------------------------------------------------------------

      THIS PROGRAM GETS NEW Q STATISTICS FOR STEPWISE VARIABLES.

      AUTHOR/IMPLEMENTOR - L. MANDERSON

      FROM CODE BY       - R.L. KORNEGAY (10/85, 5/86, 7/87)

      WRITTEN            - JULY 1991

      FUNCTIONS/SUBROUTINES CALLED - DQSTAT

 -----------------------------------------------------------------------

    THE SUBROUTINE DQSTAT IS CALLED FOR EACH VARIABLE (IN EACH PHASE)
    THAT IS NOT PRESENTLY IN THE STEPWISE MODEL.  THE Q STATISTICS
    ARE STORED IN VECTORS FOR LATER REFERENCE.

 -----------------------------------------------------------------------
   VARIABLE DICTIONARY:

   VARIABLES PASSED AS ARGUMENTS
      STATUS - INTEGER  11+3*P VECTOR OF 0/1/2 FLAGS
                        STATUS().EQ.0...VARIABLE SELECTED BY USER FOR
                                        THIS PHASE BUT NOT IN THE MODEL
                                        AT THE PRESENT TIME
                        STATUS().EQ.1...VARIABLE SELECTED BY USER FOR
                                        THIS PHASE AND IS IN THE MODEL
                                        AT THIS TIME
                        STATUS().EQ.2...VARIABLE SELECTED BY USER FOR
                                        INCLUSION IN ANOTHER PHASE BUT
                                        WILL NEVER BY CONSIDERED FOR
                                        THIS PHASE
      PVALUE  - REAL*8  11+3*P VECTOR OF P'S FROM DCOVAR AND QP'S FROM
                        DQSTAT
      MOVES  - INTEGER  11+3*P VECTOR OF THE NUMBER OF MOVES OUT OF THE
                        MODEL A PARTICULAR CONCOMITANT VARIABLE HAS MADE
      MXMOVE - INTEGER  11+3*P VECTOR OF THE MAXIMUM NUMBER OF MOVES OUT
                        OF THE MODEL A PARTICULAR CONCOMITANT VARIABLE
                        MAY MAKE
      ZVALUE  - REAL*8  11+3*P VECTOR OF Z'S FROM DCOVAR AND QZ'S FROM
                        DQSTAT
      STDERR  - REAL*8  11+3*P VECTOR OF STANDARD ERRORS FROM DCOVAR AND
                        DQSTAT
      FLAGS  - INTEGER  11+3*P VECTOR OF FLAGS FROM DCOVAR AND QFLAGS
                        FROM DQSTAT
      QX      - REAL*8  11+3*P VECTOR OF QBETA'S
      QTOLS   - REAL*8  11+3*P VECTOR OF QTOL'S
      D2LL    - REAL*8  NVAR BY NVAR MATRIX CONTAINING CHOLESKY
                        FACTORIZATION (NVAR=NUMBER OF ACTIVE
                        PARAMETERS IN THE CONCOMITANT INFORMATION
                        PART OF MODEL)
      NVAR    - INTEGER NUMBER OF ESTIMATED INTERCEPTS AND SCALE
                        FACTORS
      WK      - REAL*8  NVAR VECTOR OF WORKING STORAGE
      D2LLAD  - REAL*8  NVAR+1 VECTOR FOR SECOND DERIVATIVES

   LOCAL VARIABLES
      J       - INTEGER LOOP INDEX VALUE
      INDX    - INTEGER POINTER TO VARIABLE OF INTEREST IN OBS
      QFLAG   - INTEGER DIAGNOSTICS CONCERNING THE QS;  IN EACH
                        INSTANCE QP IS SET TO 1.0:
                        0 - NO MESSAGES
                        1 - FIRST DERIVATIVE IS ZERO;  THIS MEANS THE
                            VARIABLE IS REDUNDANT
                        2 - DIAGONAL ELEMENT OF SECOND DERIVATIVE IS
                            NOT POSITIVE DEFINITE.  THIS IS EITHER
                            A BAD CANDIDATE FOR THE MODEL OR IS SO IN
                            THE CONTEXT OF THE OTHER FACTORS POSSIBLY
                            IN THE MODEL
                        3 - TOLERANCE OF THE Q IS ZERO OR NEGATIVE;
                            THIS ALSO MEANS THAT THE VARIABLE IS A
                            BAD CANDIDATE
                        4 - QBETA IS LARGER THAN |50|; THIS MEANS THAT
                            THE MODEL IS GOING TO INFINITY.  THIS IS
                            LEGITIMATE FOR A VARIABLE THAT IS EITHER
                            POSITIVE OR NEGATIVE WITH RESPECT TO ALL
                            THE REMAINING EVENTS IN ITS PHASE.  HOWEVER
                            THE MODEL CANNOT MANAGE THIS.  PERHAPS THE
                            DATA ARE TOO SPARSE, OR THE DATA SET
                            MIGHT BETTER BE STRATIFIED AND ANALYZED
                            IN SEPARATE SEGMENTS WITH RESPECT TO THIS
                            VARIABLE
      QBETA   - REAL*8  PARAMETER ESTIMATE BASED ON Q
      QSE     - REAL*8  STANDARD ERROR OF QBETA BASED ON Q
      QZ      - REAL*8  SIGNED SQUARE-ROOT OF Q
      QP      - REAL*8  PROBABILITY OF |QZ| BEING DIFFERENT FROM ONE
      QTOL    - REAL*8  POSSIBLE CANDIDATE FOR TOLERANCE NUMBER; A
                        VALUE APPROACHING ZERO INDICATES POTENTIAL
                        SINGULARITY

   VARIABLES FROM COMMON
      ERRFLG  - CHARACTER*12 STRING OF THE SUBROUTINE NAME IN WHICH
                        AN ERROR HAS BEEM DETECTED AND A CODE TO ASSIST
                        IN FINDING THE EXACT PLACE IN THE SUBROUTINE
                        WHERE THE ERROR OCCURRED
      NFNCTS  - INTEGER NUMBER OF FUNCTION EVALUATIONS PERFORMED
                        BY THE OPTIMIZATION ROUTINE
      NOBS    - INTEGER NUMBER OF OBSERVATIONS AVAILABLE FOR EVALUATION
      NTHETA  - INTEGER 11+3*P
      P       - INTEGER NUMBER OF UNIQUE CONCOMITANT VARIABLES
      PHASE   - INTEGER 3 ELEMENT VECTOR OF 0/1 FLAGS
                        PHASE().EQ.0...PHASE NOT IN THE MODEL
                        PHASE().EQ.1...PHASE IN THE MODEL
      ERROR   - INTEGER ERROR FLAGS FROM ALL ROUTINES CALLED
                        0 - NO ERROR DETECTED
                        1 - ARITHEMETIC OVERFLOW DETECTED
                        2 - SIGN VIOLATION DETECTED
      CNT     - INTEGER THE TOTAL NUMBER OF CONCOMITANT VARIABLES
                        NOT IN THE MODEL AT THIS STEP
      CNTP    - INTEGER THE NUMBER OF CONCOMITANT VARIABLES WHICH
                        COULD POSSIBLY BE A PART OF THE MODEL FOR EACH
                        PHASE AND ARE NOT INCLUDED IN THE MODEL FOR THAT
                        PHASE DURING THIS STEP

----------------------------------------------------------------------*/

{
  double qp,qz,qse,qbeta,qtol;
  int qflag;
  int j,jj,j1,j1p1,j1pp,jind,indx;

  /* INITIALIZATION */
  HZRstr.cnt = 0;
  for(j=1; j<=3; j++)
    HZRstr.cntp[j] = 0;

  /* LOOP TO CALL DQSTAT FOR Q STATISTICS. */
  for(jj=1; jj<=3; jj++) {
    if(Common.phase[jj]!=1)
      continue;
    j1 = HZRstr.pj1+(HZRstr.pp1*jj)-1;
    j1p1 = j1+1;
    j1pp = j1+Common.p;
    jind = 7-j1;
    for(j=j1p1; j<=j1pp; j++) {
      if(status[j]!=0)
	continue;
      indx = j+jind;
      HZRstr.cnt++;
      HZRstr.cntp[jj]++;
      DQSTAT(d2ll,nvar,indx,jj,&qbeta,&qse,&qz,&qp,&qflag,&qtol,
	     d2llad,wk);
      if(Common.errorno)
	return;
      pvalue[j] = qp;
      zvalue[j] = qz;
      stdErr[j] = qse;
      qx[j] = qbeta;
      flags[j] = qflag;
      qtols[j] = qtol;
    }
  }
}
