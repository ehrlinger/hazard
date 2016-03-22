#include <string.h>
#include <math.h>

#include "common.h"

#include "q1.h"
#include "normal.h"
#include "setdll.h"


#include "hzr_set_error.h"

void DQSTAT(double *chol,int nvar,int indx,long iphas,double *qbeta,
	    double *qse,double *qz,double *qp,int *qflag,double *qtol,
	    double *d2llad,double *wk)

     /***********************************************************************

   Q-STATISTICS FOR CONCOMITANT VARIABLES NOT IN MODEL (DRIVER)

   PURPOSE
     TO CALCULATE Q-STATISTICS ASSOCIATED WITH BRINGING A NEW VARIABLE
     INTO THE MAXIMUM LIKELIHOOD MODEL (DRIVER SUBROUTINE)

   DESCRIPTION OF PARAMETERS
     INPUT
       CHOL   - REAL*8  NVAR BY NVAR MATRIX CONTAINING CHOLESKY
                        FACTORIZATION (NVAR=NUMBER OF ACTIVE
                        PARAMETERS IN THE CONCOMITANT INFORMATION
                        PART OF MODEL)
       NVAR   - INTEGER NUMBER OF ESTIMATED INTERCEPTS AND SCALE
                        FACTORS
       INDX   - INTEGER POINTER TO VARIABLE OF INTEREST IN OBS
       IPHAS  - INTEGER PHASE IN WHICH VARIABLE IS TO BE TESTED

     OUTPUT
       QBETA  - REAL*8  PARAMETER ESTIMATE BASED ON Q
       QSE    - REAL*8  STANDARD ERROR OF QBETA BASED ON Q
       QZ     - REAL*8  SIGNED SQUARE-ROOT OF Q
       QP     - REAL*8  PROBABILITY OF |QZ| BEING DIFFERENT FROM ONE
       QFLAG  - INTEGER DIAGNOSTICS CONCERNING THE QS;  IN EACH
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
       QTOL   - REAL*8  POSSIBLE CANDIDATE FOR TOLERANCE NUMBER; A
                        VALUE APPROACHING ZERO INDICATES POTENTIAL
                        SINGULARITY

     TEMPORARY
       D2LLAD - REAL*8  NVAR+1 VECTOR FOR SECOND DERIVATIVES
       WK     - REAL*8  NVAR VECTOR OF WORKING STORAGE

   DESCRIPTION OF PARAMETERS FROM COMMON /ERROR/
     OUTPUT
       ERROR  - INTEGER ERROR FLAG
                0 - NO ERROR
                1 - ARITHMETIC OVERFLOW CONDITION
                2 - INCORRECT SIGN
       ERRFLG - CHARACTER*12 ERROR MESSAGE

   REMARKS
     1.  FOR CALCULATING Q-STATISTICS, WE CAN GET A GOOD APPROXIMATION
         BY FIXING ALL SHAPING PARAMETERS AND CONSIDERING ONLY THE
         INTERCEPTS AND CONCOMITANT INFORMATION.  THEREFORE THE
         FACTORED MATRIX MUST BE OF THE CONCOMITANT INFORMATION ONLY.

     2.  PRIOR TO CALLING FOR Q-STATISTICS, THE SECOND DERIVATIVES
         WITH RESPECT TO THE LOG LIKELIHOOD FUNCTION MUST BE FORMED
         (SUBROUTINE SETDLL) AND FACTORED (SUBROUTINE CHLSKM).

     3.  BECAUSE OF THE SETUP FOR Q1, WE WILL ACTUALLY USE MINUS THE
         DERIVATIVES.

   SUBPROGRAMS REQUIRED
     SETDLL - AT ENTRY DLLADD, CALCULATE FIRST AND SECOND DERIVATIVES
     NORMAL - CALCULATION OF P-VALUE GIVEN Z (NORMAL DISTRIBUTION)
     Q1     - CALCULATION OF Q-STATISTICS

   METHOD
     SEE METHOD FOR OBTAINING Q IN FUNCTION Q1.

     GIVEN Q AND THE FIRST DERIVATIVE OF THE LIKELIHOOD FUNCTION WITH
     RESPECT TO VARIABLE BETA (D1LLAD)

       QBETA = SQRT(Q)*QSE'

       QSE'= SQRT(Q)/D1LLAD

       QSE = ABS(QSE')

   IMPLEMENTATION
     E.H. BLACKSTONE,MD MARCH 1986

   REVISIONS
     DECEMBER 16,1986 - CHECK FOR ABSOLUTE VALUE OF QBETA BEING
                        50 OR MORE
     JULY, 1991       - ADD LOGIC TO CHANGE SIGN OF RETURNED Z IF
                        ESTIMATE WAS NEGATIVE.

   VERIFICATION
     APRIL 15,1986 - USING WORKED EXAMPLE FROM KALBFLEISCH AND
                     PRENTICE, PP 2, 52, AND 56

***********************************************************************/
{
  double d1llad,q;

  /* INITIALIZATION */
  Common.errorno = 0;
  CLEAR(Common.errflg);
  *qbeta = ZERO;
  *qse = ZERO;
  *qz = ZERO;
  *qp = ONE;
  *qflag = 0;
  if(indx<=0 || iphas<1 || iphas>3) {
    hzr_set_error("(DQSTAT900)",2);
    return;
  }

  /**
     GET FIRST DERIVATIVE AND VECTOR OF SECOND DERIVATIVES FOR THIS
     VARIABLE
  **/
  DLLADD(indx,iphas,&d1llad,d2llad);
  if(Common.errorno)
    return;

  /* GET Q */
  q = Q1(chol,d1llad,d2llad,qtol,qflag,wk,nvar);
  if(*qflag>0)
    return;

  /**
     DO CALCULATIONS IF WE HAVE A GOOD Q
     NOTE:  RECALL THAT THE SIGN OF THE FIRST DERIVATIVE IS OPPOSITE TO
     THAT FOR THE LOG LIKELIHOOD FUNCTION.
  **/
  *qz = sqrt(q);
  *qse = -(*qz)/d1llad;
  *qbeta = (*qz)*(*qse);
  *qse = fabs(*qse);
  if(*qbeta<0)
    *qz = -(*qz);
  if(fabs(*qbeta)>50.0e0) {
    *qflag = 4;
    return;
  }
  *qp = TWO*NORMAL(fabs(*qz));
}
