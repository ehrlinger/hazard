#include <string.h>
#include "common.h"

#include "swvarq.h"
#include "swvari.h"
#include "swvarx.h"

void STEPW(short int *status,double *pvalue,int *moves,int *mxmove,
	   double *zvalue,double *stdErr,int *flags,double *qx,
	   double *qtols,double *d2ll,int nvar,double *wk,double *d2llad,
	   int *rstvec)

     /*----------------------------------------------------------------------

      THIS PROGRAM IS THE STEPWISE OPTION OF THE HAZARD SYSTEM

      AUTHOR/IMPLEMENTOR - L. MANDERSON

      FROM CODE BY       - R.L. KORNEGAY (10/85, 5/86, 7/87)

      WRITTEN            - JULY 1991

      FUNCTIONS/SUBROUTINES CALLED - SWVARI,SWVARQ,SWVARX

 -----------------------------------------------------------------------

    IF NOSTEPWISE HAS *NOT* BEEN SPECIFIED, SWVARX IS CALLED TO 
    DETERMINE IF ANY VARIABLES SHOULD BE EXCLUDED FROM THE MODEL.
    NOTE THAT THE NOSTEPWISE TEST IS STRICTLY FOR PROGRAM OPTIMIZATION;
    MOVES ARE ADJUSTED UNDER NOSTEPWISE TO ENSURE THAT VARIABLES CANNOT
    LEAVE ONCE THEY HAVE COME IN.

    SWVARQ IS CALLED (IF NPQ=0) TO RECOMPUTE Q STATISTICS.  IF ANY
    ERROR IS FOUND, THE SUBROUTINE ENDS.

    IF SWVARX DID NOT FIND A VARIABLE TO EXCLUDE, SWVARI IS CALLED TO
    DETERMINE IF ANY VARIABLES SHOULD BE INCLUDED INTO THE MODEL.

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
      RSTVEC  - INTEGER 11+3*P VECTOR OF CURRENT INCLUSION RESTRICTIONS

   VARIABLES FROM COMMON
      VSTEP   - CHARACTER*8  NAME OF THE VARIABLE THAT HAS BEEN
                        ADDED TO OR DELETED FROM THE MODEL
      FSTEP   - CHARACTER*8  NAME OF THE PHASE TO WHICH THE VARIABLE
                        HAS BEEN ADDED OR DELETED
      ERRFLG  - CHARACTER*12 STRING OF THE SUBROUTINE NAME IN WHICH
                        AN ERROR HAS BEEM DETECTED AND A CODE TO ASSIST
                        IN FINDING THE EXACT PLACE IN THE SUBROUTINE
                        WHERE THE ERROR OCCURRED
      ERROR   - INTEGER ERROR FLAGS FROM ALL ROUTINES CALLED
                        0 - NO ERROR DETECTED
                        1 - ARITHEMETIC OVERFLOW DETECTED
                        2 - SIGN VIOLATION DETECTED
      NPQ     - INTEGER PRINT FLAG FOR Q'S, IF SELECTED ONLY A SUMMARY
                        OF VARIABLES IN THE MODEL AND A SUMMARY OF
                        EACH STEP IS PRINTED
      SWADEL  - INTEGER FLAG TO PASS TO CALLING PROGRAM WHICH FUNCTION
                        HAS BEEN PERFORMED DURING THE STEPWISE/BACKWARD
                        PROCESS:
                            SWADEL.EQ.0 - NO MOVE HAS BEEN MADE
                            SWADEL.EQ.1 - VARIABEL DELETED FROM MODEL
                            SWADEL.EQ.2 - VARIABLE ADDED TO MODEL
      HPSN    - INTEGER POSITION OF VARIABLE SELECTED AS A CANDIDATE
                FOR A MOVE
      FDONE   - INTEGER FLAG USED TO PASS COMPLETION STATUS OF FAST
                        STEPWISE OPTION
                        FDONE.EQ.0 - FAST EITHER NOT SELECTED OR
                                     FAST PROCESS NOT COMPLETED
                        FDONE.EQ.1 - FAST PROCESS COMPLETED
      ZSTEP   - REAL*8  ZVALUE OF THE VARIABLE ADDED TO/DELETED FROM THE
                        MODEL
      PSTEP   - REAL*8  PVALUE OF THE VARIABLE ADDED TO/DELETED FROM THE
                        MODEL
      NOSW    - INTEGER NOSTEPWISE SELECTION FLAG
      RESTCT  - INTEGER COUNT OF RESTRICTED VARIABLE SPECIFICATIONS

----------------------------------------------------------------------*/

{
  /* INITIALIZATION */
  HZRstr.swadel = 0;
  HZRstr.hpsn = 0;

  /**
     CALL SWVARX TO LOOK FOR VARIABLES TO BE EXCLUDED, IF NOSTEPWISE
     HAS NOT BEEN SPECIFIED.
  **/
  if(!HZRstr.opt.nosw)
    SWVARX(status,pvalue,moves,mxmove);

  /**
     THE Q'S WILL BE COMPUTED ONLY IF THE USER WANTS TO PRINT THEM (NPQ=0)
     OR NO VARIABLE WAS FOUND FOR REMOVAL FROM THE MODEL (HPSN=0)
  **/
  if(!HZRstr.hpsn || !HZRstr.npq) {
    SWVARQ(status,pvalue,zvalue,stdErr,flags,qx,qtols,d2ll,nvar,wk,d2llad);
    if(Common.errorno)
      return;
  }
   
  /**
     CALL SWVARI TO LOOK FOR VARIABLES TO BE INCLUDED, IF SWVARX DID NOT
     FIND ONE TO BE EXCLUDED.
  **/
  if(!HZRstr.hpsn)
    SWVARI(status,pvalue,moves,mxmove,zvalue,flags,rstvec);
}
