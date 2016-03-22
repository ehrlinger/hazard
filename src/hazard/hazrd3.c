#include <string.h>
#include <math.h>

#include "hazard.h"
#include "hazrd3.h"
#include "theta_x.h"
#include "hazrerr.h"
#include <pdlike.h>
#include <setobj.h>
#include <setoptim.h>
#include <setcoe.h>
#include <restor.h>

#include <hzfxpc.h>
#include <hzfskp.h> 
#include <hzfxpf.h> 
#include <hzfxpi.h>
#include <hzfpag.h>
#include <hzfxpr.h>
#include <hzflg2.h>
#include <hzflg1.h>

#include <constp.h>

void HAZRD3(void){
  int smode;

  if(H->mode==0 && H->only1!=1)
    O->N = H->N-1;
  else
    O->N = H->N;
  DCEOBJ(&O->typf);
  O->itrcnt = 0;
  H->nfncts = 0;
  O->gradtl = -ONE;
  O->stptol = -ONE;
  O->maxstp = -ONE;
  O->fdigit = -1;
  O->prtcod = 2;
  setoptim();
  if(C->errorno==1)
    HAZ3TRM(9900);
  else if(C->errorno==2)
    HAZ3TRM(9910);
  STPNOTE();
  if(!(H->angrad || H->anhess))
    RESTOR(H->x0,O->N,H->nm1dx,C->theta);
  if(C->errorno==1)
    HAZ3TRM(9900);
  else if(C->errorno==2)
    HAZ3TRM(9910);
  nrow = ceil((H->N+ONE)/7);
  if(prnt==1) {
    OPTHDR();
    theta_x();
    OPTPRTX();
  }
  while(O->itrcnt<O->itrlmt && O->iret==0 && C->errorno==0 &&
	O->trmcod==0) {
    optim();
    if(C->errorno) {
      OPTNOTE();
      if(C->errorno==1)
	HAZ3TRM(9900);
      else if(C->errorno==2)
	HAZ3TRM(9910);
    }
    if(prnt==1) {
      theta_x();
      OPTPRTX();
    }
  }
  if(!(H->angrad || H->anhess)) {
    RESTOR(H->xplus,O->N,H->nm1dx,C->theta);
    if(C->errorno==1)
      HAZ3TRM(9900);
    else if(C->errorno==2)
      HAZ3TRM(9910);
  }
  O->iret++;
  smode = H->mode;
  H->mode = 2;
  constp();
  H->mode = smode;
  if(C->errorno==1)
    HAZ3TRM(9900);
  else if(C->errorno==2)
    HAZ3TRM(9910);
  OPTNOTE();
  switch(O->iret) {
  case 2:
    HAZ3TRM(9110);
    break;
  case 3:
    hzfxpc("Initial estimate appears to be a critical",41,9);
    hzfxpc(" point.  It is possible, however, that it",41,0);
    hzfxpc(" is a maximizer or a saddle point.",34,0);
    hzfskp(1);
    RETRY_T();
    break;
  case 4:
    hzfxpc("The scaled distance between the last two",40,9);
    hzfxpc(" estimates of the parameters is less than ",42,0);
    hzfxpf(O->stptol,16,99,0);
    hzfskp(1);
    hzfxpc("The current set of parameter estimates",38,9);
    hzfxpc(" may be a minimizer.  It is also possible",41,0);
    hzfxpc(" that the optimization code",27,0);
    hzfskp(1);
    hzfxpc("is making slow progress and is not near",39,9);
    hzfxpc(" a minimum.",11,0);
    hzfskp(1);
    RETRY_T();
    break;
  case 5:
    hzfxpc("The last step attempted failed to find a",40,9);
    hzfxpc(" point better than the last estimate. ",38,0);
    hzfxpc(" Either the current estimates",29,0);
    hzfskp(1);
    hzfxpc("are a minimizer and no more accuracy is",39,9);
    hzfxpc(" possible or insufficient accuracy is",37,0);
    hzfxpc(" possible in the gradient calculations",38,0);
    hzfskp(1);
    hzfxpc("due to machine precision.",25,9);
    hzfskp(1);
    RETRY_T();
    break;
  case 6:
    hzfxpc("Maximum iterations of ",22,9);
    hzfxpi(O->itrlmt,5,0);
    hzfxpc("reached no convergence.",23,-1);
    hzfskp(1);
    break;
  case 7:
    hzfxpc("Five consecutive steps of length, ",34,9);
    hzfxpf(O->maxstp,22,99,0);
    hzfxpc(" have been taken.  Either the objective",39,0);
    hzfxpc(" function is unbounded below or has a",37,0);
    hzfxpc(" finite asymptote in some direction.",36,0);
    hzfskp(1);
    hzfxpc("It is also possible that the value used",39,9);
    hzfxpc(" is too small.",14,0);
    hzfskp(1);
    RETRY_T();
    break;
  case 8:
    if(H->retry!=0)
      HAZ3TRM(9136);
    hzfxpc("Problem is so ill-conditioned that a",36,9);
    hzfxpc(" reasonable direction of search cannot",38,0);
    hzfxpc(" be calculated",14,0);
    hzfskp(1);
    hzfxpc("given the current limit for the condition",41,9);
    hzfxpc(" code.",6,0);
    hzfskp(1);
    RETRY_T();
  }
  hzfpag(2);
}

void STPNOTE(void){
  char fstpnm[20];

  hzfpag(15);
  if(*H->fstep=='E')
    memcpy(fstpnm,"EARLY PHASE         ",20);
  else if(*H->fstep=='C')
    memcpy(fstpnm,"CONSTANT PHASE      ",20);
  else if(*H->fstep=='L')
    memcpy(fstpnm,"LATE PHASE          ",20);
  if(H->step==0 && (H->sw==1 || H->bw==1))
    if(H->sw==1) {
      hzfxpc("*-----------------------------",30,1);
      hzfxpc("---------------------------------",33,0);
      hzfxpc("--------------------*",21,0);
      hzfskp(1);
      hzfxpc("|Step 0.  Intercepts, included",30,1);
      hzfxpc(" variables, and non-fixed shaping",33,0);
      hzfxpc(" parameters entered.|",21,0);
      hzfskp(1);
      hzfxpc("*-----------------------------",30,1);
      hzfxpc("---------------------------------",33,0);
      hzfxpc("--------------------*",21,0);
      hzfskp(2);
    } else {
      hzfxpc("*-----------------------------------",36,1);
      hzfxpc("---------*",10,0);
      hzfskp(1);
      hzfxpc("|Step 0.  All non-excluded variables",36,1);
      hzfxpc(" entered.|",10,0);
      hzfskp(1);
      hzfxpc("*-----------------------------------",36,1);
      hzfxpc("---------*",10,0);
      hzfskp(2);
    }
  else if(H->step>0 && H->swadel==1 && (H->sw==1 || H->bw==1)) {
    hzfskp(2);
    hzfxpc("*",1,1);
    hzfxpr('-',58,0);
    hzfxpc("*",1,0);
    hzfskp(1);
    hzfxpc("|Step ",6,1);
    hzfxpi(H->step,3,0);
    hzfxpc(". ",2,0);
    hzfxpc(H->vstep,8,0);
    hzfxpc(" deleted from ",14,0);
    hzfxpc(fstpnm,20,0);
    hzfxpc("|",1,-6);
    hzfskp(1);
    hzfxpc("|",1,1);
    hzfxpc("with Z = ",9,-19);
    hzfxpf(H->zstep,11,3,0);
    hzfxpc(" and P = ",9,0);
    if(H->pstep<PMIN)
      hzfxpc("<.0001         ",10,0);
    else
      hzfxpf(H->pstep,10,4,0);
    hzfxpc("|",1,0);
    hzfskp(1);
    hzfxpc("*",1,1);
    hzfxpr('-',58,0);
    hzfxpc("*",1,0);
    hzfskp(2);
  } else if(H->step>0 && H->swadel==2 && (H->sw==1 || H->bw==1)) {
    hzfskp(2);
    hzfxpc("*",1,1);
    hzfxpr('-',58,0);
    hzfxpc("*",1,0);
    hzfskp(1);
    hzfxpc("|Step ",6,1);
    hzfxpi(H->step,3,0);
    hzfxpc(". ",2,0);
    hzfxpc(H->vstep,8,0);
    hzfxpc(" added to ",10,0);
    hzfxpc(fstpnm,20,0);
    hzfxpc("|",1,-10);
    hzfskp(1);
    hzfxpc("|",1,1);
    hzfxpc("with Z = ",9,-19);
    hzfxpf(H->zstep,11,3,0);
    hzfxpc(" and P = ",9,0);
    if(H->pstep<PMIN)
      hzfxpc("<.0001         ",10,0);
    else
      hzfxpf(H->pstep,10,4,0);
    hzfxpc("|",1,0);
    hzfskp(1);
    if(O->ostep!=O->ordmax) {
      hzfxpc("|",1,1);
      hzfxpc("ORDER  = ",9,-19);
      hzfxpf(O->ostep,11,99,0);
      hzfxpc("|",1,-19);
      hzfskp(1);
    }
    hzfxpc("*",1,1);
    hzfxpr('-',58,0);
    hzfxpc("*",1,0);
    hzfskp(2);
  }
}

void OPTNOTE(void){
  double dlike,llike,diff;
  double ncons;

  hzfpag(15);
  llike = -(O->fplus);
  hzfskp(2);
  hzfxpc("Optimization terminated after",29,9);
  hzfxpi(O->itrcnt,5,0);
  hzfxpc("iterations and ",15,-1);
  hzfxpi(H->nfncts,5,0);
  hzfxpc("function evaluations",20,-1);
  hzfskp(1);
  if(O->iret==1) {
    hzfxpc("Normal exit: Convergence attained",33,9);
    hzfskp(1);
  }
  hzfxpc("Log base 10 of condition code = ",32,9);
  hzfxpf(O->rcondn,12,99,0);
  hzfskp(1);
  ncons = tally*H->factor;
  hzfxpc("Number of events conserved = ",29,9);
  hzfxpf(ncons,12,99,0);
  hzfskp(2);
  hzfxpc("Log likelihood = ",17,9);
  hzfxpf(llike,12,99,0);
  hzfskp(2);
  if(H->step>0) {
    diff = fabs(2*(H->slike-llike));
    dlike = fabs(PDLIKE(H->slike,llike,1));
    hzfxpc("-2(Difference in log likelihood) = ",35,9);
    hzfxpf(diff,12,99,0);
    hzfskp(1);
    hzfxpc("                         P(diff) = ",35,9);
    if(dlike<PMIN)
      hzfxpc("<.0001",6,0);
    else
      hzfxpf(dlike,10,4,0);
    hzfskp(2);
  }
  H->slike = llike;
}

void OPTHDR(void){
  int j,n1,pos;

  hzfpag(nrow*2+2);
  hzfxpc("Iteration",9,5);
  hzfxpc("Log Likelihood",14,-3);
  for(j=0,n1=0,pos=37; j<H->N; j++,n1++,pos+=14) {
    if(n1==7) {
      hzfskp(1);
      n1 = 0;
      pos = 37;
    }
    hzfxpc(H->namex[j],8,pos);
  }
  hzfskp(1);
  hzfxpr('-',125,5);
  hzfskp(1);
}

void OPTPRTX(void){
  int j,n1,pos;

  hzfpag(nrow);
  if(nrow>1)
    hzfskp(1);
  hzfxpi(O->itrcnt,5,5);
  hzfxpf(-(O->fplus),12,99,-7);
  for(j=0,n1=0,pos=34; j<H->N; j++,n1++,pos+=14) {
    if(n1==7) {
      hzfskp(1);
      n1 = 0;
      pos = 34;
    }
    hzfxpf(H->x[j],12,99,pos);
  }
  hzfskp(1);
}

void RETRY_T(void){
  int j;

  if(H->retry!=0)
    return;
  hzfxpc("The starting values for concomitant variables",45,9);
  hzfxpc(" will be replaced by zeros and optimization",43,0);
  hzfxpc(" will be tried again.",21,0);
  hzfskp(2);
  H->retry = 2;
  if(C->phase[1]==1)
    for(j=9; j<C->p+9; j++)
      C->theta[j] = ZERO;
  if(C->phase[2]==1)
    for(j=C->p+10; j<2*C->p+10; j++)
      C->theta[j] = ZERO;
  if(C->phase[3]==1)
    for(j=2*C->p+11; j<3*C->p+11; j++)
      C->theta[j] = ZERO;
}

void HAZ3TRM(int errcode){
  C->errorno = errcode;
  hzfpag(10);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  switch(errcode) {
  case 9900:
    hzfxpc("A computation has exceeded its limits",37,5);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Computation has exceeded limit ",40,
	   C->errflg,12);
    break;
  case 9910:
    hzfxpc("Internal sign violation, possibly data or",41,5);
    hzfxpc("specification dependent",23,-1);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: internal sign violation ",33,C->errflg,12);
    break;
  case 9110:
    hzfxpc("Terminal error: No active parameters",36,9);
    hzfxpc(" specified.",11,0);
    hzfskp(1);
    hzflg1("ERROR: No active parameters specified.",38);
    break;
  case 9136:
    hzfxpc("Problem is so ill-conditioned that a",36,9);
    hzfxpc(" reasonable direction of search cannot",38,0);
    hzfxpc(" be calculated",14,0);
    hzfskp(1);
    hzfxpc("given the current limit for the condition",41,9);
    hzfxpc(" code.",6,0);
    hzfskp(1);
    hzflg2("ERROR: Problem is so ill conditioned that a ",44,
	   "reasonable direction of search cannot be calculated.",52);
  }
  hazrerr();
}
