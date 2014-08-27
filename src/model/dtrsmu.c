#include <math.h>
#include <string.h>
#include <common.h>
#include <hzr_set_error.h>

void DTRSMU_mu_to_theta(void);
void DTRSMU_theta_to_mu(void);

void DTRSMU(short int iop)

     /***********************************************************************

TRANSFORMATION TO AND FROM INPUT/ESTIMATION SCALE PARAMETERS

PURPOSE
TO TRANSFORM TO AND FROM MODEL AND ESTIMATION SCALE PARAMETERS,
DEPENDING ON THE VALUE OF IOP:

IOP = 1, GIVEN MUE,MUC,MUL:
CALCULATE CORRESPONDING THETAS

IOP = 2, GIVEN CORRESPONDING THETAS:
CALCULATE MUE,MUC,MUL

FOR EACH PHASE ADOPT THE LOG-LINEAR APPROACH:

B0
MU  =  E

WHERE

B0 IS THE INTERCEPT TERM (OFTEN OF LINEAR REGRESSION FORM)

DESCRIPTION OF PARAMETERS
INPUT
THETA  - REAL*8  NTHETA-VECTOR OF PARAMETERS
MU     - REAL*8  3-VECTOR OF SCALE FACTORS
IOP    - INTEGER FLAG TO CONTROL TRANSFORMATION (SEE ABOVE)

OUTPUT
NONE

DESCRIPTION OF PARAMETERS INPUT AND OUTPUT TO AND FROM COMMON
INPUT
PHASE  - INTEGER 3-VECTOR INDICATOR FOR PHASES IN MODEL
0 - PHASE NOT IN MODEL
1 - PHASE IN MODEL

OUTPUT
NONE

ERROR
ERROR  - INTEGER ERROR FLAG
0 - NO ERROR
1 - ARITHMETIC OVERFLOW (FATAL)
2 - ILLEGAL VARIABLE VALUES (EG. SIGN)
ERRFLG - CHARACTER*12 ERROR MESSAGE

REMARKS
1.  THETA IS CHANGED ON OUTPUT IF IOP=1 AND NO ERRORS ARE
DETECTED

2.  MU(1) - MU(3) ARE CHANGED ON OUTPUT IF IOP=2 AND NO ERRORS
ARE DETECTED

SUBPROGRAMS REQUIRED
NONE

METHOD
CARE IS TAKEN TO CHECK FOR VALUES OUT OF RANGE

IMPLEMENTATION
E.H. BLACKSTONE,MD     NOVEMBER 1985

REVISIONS
JULY 8, 1986  - OUTPUT OF TRANSFORMATIONS OCCURS ONLY IF NO
ERRORS ARE DETECTED
October 1993  - Converted to HZRCOR C-version 4.0(0)

VERIFICATION
NOVEMBER 21, 1985
TRANSFORM AND UNTRANSFORM OF VALUES FOR MUE,MUC,MUL AND
THESE MATCHED HAND CALCULATED RESULTS.

***********************************************************************/

{
  /* Initialize ... */

  if(iop<1 || iop>2) {
    hzr_set_error("(DTRSMU900)",2);
    return;
  }
  Common.errorno = 0;
  CLEAR(Common.errflg);

  if(iop==1)
    DTRSMU_mu_to_theta();
  else
    DTRSMU_theta_to_mu();
}

void DTRSMU_mu_to_theta(void)
     /**
IOP=1:  TRANSFORM MODEL PARAMETERS TO ESTIMATION PARAMETERS
**/
{
  double muj,tmu[4];
  long j,pp1,pj1;

  pp1 = Common.p+1;
  pj1 = (8-Common.p)-1;
  for(j=1; j<=3; j++) {
    if(Common.phase[j]!=1)
      continue;
    muj = *HZRstr.Mu[j];
    if(muj<=ZERO) {
      hzr_set_error("(DTRSMU910)",2);
      return;
    }
    if(muj>=Machn.absMax || muj<=Machn.absMin) {
      hzr_set_error("(DTRSMU920)",1);
      return;
    }
    tmu[j] = LOGe(muj);
  }

  /* NO ERRORS DETECTED, SO UPDATE THETA */
  for(j=1; j<=3; j++)
    if(Common.phase[j]==1)
      Common.theta[pj1+pp1*j] = tmu[j];
}

void DTRSMU_theta_to_mu(void)
     /**
IOP=2:  TRANSFORM ESTIMATION PARAMETERS TO MODEL PARAMETERS
**/
{
  double lmu,tmu[4];
  long j,pp1,pj1;

  pp1 = Common.p+1;
  pj1 = (8-Common.p)-1;
  for(j=1; j<=3; j++) {
    if(Common.phase[j]!=1)
      continue;
    lmu = Common.theta[pj1+pp1*j];
    if(lmu<=LnLim.absMin || lmu>=LnLim.absMax) {
      hzr_set_error("(DTRSMU930)",1);
      return;
    }
    tmu[j] = ePOW(lmu);
  }

  /* NO ERRORS DETECTED, SO UPDATE MU */
  for(j=1; j<=3; j++)
    if(Common.phase[j]==1)
      *HZRstr.Mu[j] = tmu[j];
}
