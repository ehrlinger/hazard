#include <string.h>
#include "common.h"

void SWVARX(short int *status,double *pvalue,int *moves,int *mxmove)

     /*----------------------------------------------------------------------

      THIS PROGRAM IS THE STEPWISE BACKWARD OPTION OF THE HAZARD SYSTEM

      AUTHOR/IMPLEMENTOR - L. MANDERSON

      FROM CODE BY       - R.L. KORNEGAY (10/85, 5/86, 7/87)

      WRITTEN            - JULY 1991

      FUNCTIONS/SUBROUTINES CALLED - NONE

 -----------------------------------------------------------------------

   VARIABLES ARE TESTED ON A PHASE-BY-PHASE BASIS, TO DETERMINE IF THEY
   ARE SUITABLE FOR EXCLUSION FROM THE MODEL.  HERE ARE THE CRITERIA
   FOR EXCLUSION:

      1. MUST BE UNDER CONSIDERATION FOR THE PARTICULAR PHASE (STATUS).
     2a. P-VALUE MUST GREATER THAN LIMIT SET FOR STAYING (SLS). *OR*
     2b. EXCLUSION OF VARIABLE IS REQUIRED TO SATISFY MAXVARS.
      3. MOVEMENT OF THE VARIABLE MUST BE PERMITTED (MOVES/MXMOVE).
      4. MUST HAVE HIGHEST P-VALUE OF VARIABLES MEETING CRITERIA 1-3.

   IF A VARIABLE IS SELECTED, ITS POSITION IS RETURNED IN HPSN.

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
      FLAGS  - INTEGER  11+3*P VECTOR OF FLAGS FROM DCOVAR AND QFLAGS
                        FROM DQSTAT

   LOCAL VARIABLES
      J       - INTEGER LOOP INDEX VALUE
      HVAL    - REAL*8  VALUE SELECTED AS A LIKELY CANDIDATE FOR
                        MOVEMENT

   VARIABLES FROM COMMON
      VSTEP   - CHARACTER*8  NAME OF THE VARIABLE THAT HAS BEEN
                        ADDED TO OR DELETED FROM THE MODEL
      FSTEP   - CHARACTER*8  NAME OF THE PHASE TO WHICH THE VARIABLE
                        HAS BEEN ADDED OR DELETED
      NFNCTS  - INTEGER NUMBER OF FUNCTION EVALUATIONS PERFORMED
                        BY THE OPTIMIZATION ROUTINE
      NOBS    - INTEGER NUMBER OF OBSERVATIONS AVAILABLE FOR EVALUATION
      NTHETA  - INTEGER 11+3*P
      P       - INTEGER NUMBER OF UNIQUE CONCOMITANT VARIABLES
      PHASE   - INTEGER 3 ELEMENT VECTOR OF 0/1 FLAGS
                        PHASE().EQ.0...PHASE NOT IN THE MODEL
                        PHASE().EQ.1...PHASE IN THE MODEL
      SW      - INTEGER STEPWISE SELECTION FLAG
      BW      - INTEGER BACKWARD SELECTION FLAG
      F       - INTEGER FAST SELECTION FLAG
      MOV     - INTEGER OVERALL NUMBER OF MOVES ALLOWED EACH PIECE
                        OF CONCOMITANT INFORMATION IF A VALUE IS NOT
                        ENTERED FOR AN INDIVIDUAL NUMBER OF MOVES
                        ALLOWED WHEN THE VARIABLE IS SELECTED
      NPS     - INTEGER PRINT FLAG FOR STEPS, IF SELECTED NEITHER
                        A LIST OF VARIABLES IN THE MODEL NOR A LIST
                        OF VARIABLES OUT OF THE MODEL ARE PRINTED,
                        HOWEVER, A SUMMARY OF EACH STEP IS PRINTED
      NPQ     - INTEGER PRINT FLAG FOR Q'S, IF SELECTED ONLY A SUMMARY
                        OF VARIABLES IN THE MODEL AND A SUMMARY OF
                        EACH STEP IS PRINTED
      DONE    - INTEGER STEPWISE COMPLETION FLAG
                        DONE.EQ.0 - EITHER A STEPWISE PROCESS HAS NOT
                                    BEEN SELECTED OR THE STEPWISE
                                    PROCESS IS INCOMPLETE
                        DONE.EQ.1 - STEPWISE PROCESS IS COMPLETE
      ONLY1   - INTEGER ONLY1 WILL BE NON-ZERO ONLY IF THE NUMBER
                        OF PARAMETERS BEING ESTIMATED IS 1 AND
                        SOME SORT OF CONSERVATION OF EVENTS IS SELECTED
      STEP    - INTEGER COUNT OF THE NUMBER OF STEPS PERFORMED DURING
                        THE STEPWISE PROCESS
      RETRY   - INTEGER FLAG TO INFORM THE SYSTEM WHEN NON-ZERO STARTING
                        VALUES HAVE BEEN USED, ALLOWING FOR A SECOND
                        TRY AT THE STEP AFTER ZEROING OUT THE STARTING
                        VALUES FOR ALL CONCOMITANT VARIABLES
      LSTVAR  - INTEGER INDEX OF LAST VARIABLE MOVED INTO OR OUT OF
                        MODEL
      SWADEL  - INTEGER FLAG TO PASS TO CALLING PROGRAM WHICH FUNCTION
                        HAS BEEN PERFORMED DURING THE STEPWISE/BACKWARD
                        PROCESS:
                            SWADEL.EQ.0 - NO MOVE HAS BEEN MADE
                            SWADEL.EQ.1 - VARIABLE DELETED FROM MODEL
                            SWADEL.EQ.2 - VARIABLE ADDED TO MODEL
                           SWADEL.EQ.11 - VARIABLE DELETED DUE TO MAXVARS
      HPSN    - INTEGER POSITION OF VARIABLE SELECTED AS A CANDIDATE
                FOR A MOVE
      FDONE   - INTEGER FLAG USED TO PASS COMPLETION STATUS OF FAST
                        STEPWISE OPTION
                        FDONE.EQ.0 - FAST EITHER NOT SELECTED OR
                                     FAST PROCESS NOT COMPLETED
                        FDONE.EQ.1 - FAST PROCESS COMPLETED
      SLE     - REAL*8  UPPER BOUND ON P-VALUE FROM Q-STATISTICS ALLOWED
                        INTO THE MODEL
      SLS     - REAL*8  UPPER BOUND ON P-VALUE FROM DCOVAR ALLOWED TO
                        REMAIN IN THE MODEL
      ZSTEP   - REAL*8  ZVALUE OF THE VARIABLE ADDED TO/DELETED FROM THE
                        MODEL
      PSTEP   - REAL*8  PVALUE OF THE VARIABLE ADDED TO/DELETED FROM THE
                        MODEL

----------------------------------------------------------------------*/

{
  double hval;
  int i,j = 0,jj,j1,j1p1,j1pp,reqx;

  /* INITIALIZATION */
  hval = ZERO;

  /* CHECK FOR MAXVARS REQUIREMENT */
  reqx = 0;
  if(HZRstr.mxvars!=0) {
    j = 0;
    for(i=9; i<Common.Ntheta; i++)
      if(status[i]==1 && i!=(Common.p+9) && i!=(2*Common.p+10))
	j++;
    if(j>HZRstr.mxvars)
      reqx = 1;
  }

  /* MAIN TESTING LOOP */
  for(jj=1; jj<=3; jj++) {
    if(Common.phase[jj]!=1)
      continue;
    j1 = HZRstr.pj1+(HZRstr.pp1*jj)-1;
    j1p1 = j1+1;
    j1pp = j1+Common.p;
    for(j=j1p1; j<=j1pp; j++)
      if(status[j]==1 && (reqx || pvalue[j]>HZRstr.sls) &&
	 moves[j]<mxmove[j] && pvalue[j]>hval) {
	hval = pvalue[j];
	HZRstr.hpsn = j;
      }
  }

  /* IF VARIABLE WAS SELECTED, SAVE LAST POSITION AND SET ADD/DELETE FLAG */
  if(HZRstr.hpsn!=0) {
    if(reqx && pvalue[j]<=HZRstr.sls)
      HZRstr.swadel = 11;
    else
      HZRstr.swadel = 1;
    HZRstr.lstvar = HZRstr.hpsn;
  }
}
