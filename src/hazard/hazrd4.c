#include <string.h>
#include <math.h>
#include "hzrinc.h"

#define HAZRD4
#include "hazrd4.h"

#include "hazard.h"

#include "setidxs.h"
#include "phasidx.h"

#include <dcovar.h>
#include <backw.h>
#include <stepw.h>
#include <dfast.h>
#include "hazrerr.h"
#include <dtrsmu.h>

#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzflg1.h>

#include <hzfpag.h>
#include <hzfxpr.h>
#include <hzfxpi.h>
#include <hzflg2.h>


/* Comment out this statement to print debugging messages to the
   stderr stream. This statement effects this file only */
#define NDEBUG 

/* */
void HZ4I(struct common *hzrC){
  C = hzrC;
  H = C->hzrstr;
  O = &H->opt;
}

/* */
void HZ4G(void){
  adone = FALSE;
  H->mxstop = 0;
  hz4stop = FALSE;
  while(!hz4stop)
    HZ4LOOP();
}

/* */
void HZ4LOOP(void){
  int i,i0,i1,j,jvar0,k;

  hz4stop = TRUE;
  if(H->sw!=1 && H->bw!=1 && H->f==1 && H->fdone==1)
    H->done = 1;
  if(H->done==1) {
    adone = TRUE;
    if((H->sw==1 || H->bw==1) && H->mxstop==0) {
      hzfskp(1);
      if(H->sw==1) {
	hzfxpc("No other variables met the ",27,5);
	hzfxpf(H->sle,12,99,0);
	hzfxpc(" significance level for entry",29,0);
	hzfxpc(" into the model",15,0);
      } else {
	hzfxpc("All variables in the model",26,5);
	hzfxpc(" are significant at the ",24,0);
	hzfxpf(H->sls,12,99,0);
	hzfxpc(" level",6,0);
      }
      if(O->nosw)
	hzfxpc(", and NOSTEPWISE was specified.",31,0);
      else
	hzfxpc(".",1,0);
      hzfskp(2);
      if(O->restct!=0) {
	RSTBACK();
	if(H->swadel!=0) {
#ifndef NDEBUG
	  fprintf(stderr, "WARNING: At least one RESTRICTed variable was not removed from the model.\n");
#endif /* NDEBUG */
	  hzflg1("WARNING: At least one RESTRICTed variable was not "
		 "removed from the model.",73);
	  hzfxpc("****** WARNING: At least one RESTRICTed variable "
		 "could not be removed from the model.",85,5);
	  H->swadel = 0;
	}
      }
    }
    if(H->fixg1 && H->fixg3)
      noshap = TRUE;
    else if(C->phase[1]==0 && H->fixg3)
      noshap = TRUE;
    else if(C->phase[3]==0 && H->fixg1)
      noshap = TRUE;
    else
      noshap = FALSE;
    h4nvar = H->N;
    for(i=0; i<h4nvar; i++) {
      H->beta[i] = H->x[i];
      H->indx[i] = H->index[i];
      H->nm1dx[i] = H->index[i];
    }
  } else {
    noshap = TRUE;
    i1 = 0;
    for(i=0; i<H->N; i++) {
      i0 = H->index[i];
      if(i0>=8) {
	H->beta[i1] = H->x[i];
	H->indx[i1] = i0;
	i1++;
      }
    }
    h4nvar = i1;
  }
  DCOVAR(H->beta,h4nvar,noshap,H->opt.fplus,H->indx,C->theta,
	 H->d2ll,H->cov,H->corr,H->sd,H->znorm,H->pnorm,H->flag,&stopt);
  if(C->errorno==1)
    HAZ4TRM(9900);
  else if(C->errorno==2)
    HAZ4TRM(9910);
  for(jvar0=0; jvar0<h4nvar; jvar0++) {
    i = H->indx[jvar0];
    H->pvalue[i] = H->pnorm[jvar0];
    H->stdErr[i] = H->sd[jvar0];
    H->zvalue[i] = H->znorm[jvar0];
    H->flags[i] = H->flag[jvar0];
    H->qx[i] = H->beta[jvar0];
  }
  stepk1 = stepk2 = stepk3 = -1;
  k = 0;
  if(C->phase[1]==1)
    for(i=0; i<H->N; i++) {
      j = H->index[i];
      if(j<=3 || (j>=8 && j<=C->p+8))
	H->nx[k++] = i;
      if(j>=C->p+8 && stepk1<0)
	stepk1 = k-1;
    }
  if(C->phase[2]==1)
    for(i=0; i<H->N; i++) {
      j = H->index[i];
      if(j>=C->p+9 && j<=2*C->p+9)
	H->nx[k++] = i;
      if(j>=2*C->p+9 && stepk2<0)
	stepk2 = k-1;
    }
  if(C->phase[3]==1)
    for(i=0; i<H->N; i++) {
      j = H->index[i];
      if((j>=4 && j<=7) ||
	 (j>=2*C->p+10 && j<=3*C->p+10))
	H->nx[k++] = i;
      if(j>=3*C->p+10 && stepk3<0)
	stepk3 = k-1;
    }
  cflag1 = ' ';
  cflag2 = ' ';
  cflag3 = ' ';
  cflag4 = ' ';
  if(H->done==1)
    STEPPRT();
  if(H->f==1 && H->fdone==0) {
    FAST();
    if(H->mode==0){
      if(H->fixmu==1){
	C->status[8] = 0;
      }else if(H->fixmu==2){
	C->status[C->p+9] = 0;
      }else if(H->fixmu==3){
	C->status[2*C->p+10] = 0;
      }
    }
    return;
  }
  if((H->sw==1 || H->bw==1) && H->done==0)
    STPWISE();
  if(stopt)
    HAZ4TRM(9585);
  if((H->sw==1 || H->bw==1 || H->f==1) &&
     H->done==1 && !adone) {
    for(i=0; i<H->maxn; i++) {
      H->index[i] = 0;
      H->x[i] = ZERO;
      H->nm1dx[i] = 0;
      CLEAR(H->namex[i]);
      CLEAR(H->lablx[i]);
    }
    i = 0;
    if(C->phase[1]==1)
      for(j=0; j<4; j++)
	if(C->status[j]==1)
	  setidxs(i++,j);
    if(C->phase[3]==1)
      for(j=4; j<8; j++)
	if(C->status[j]==1)
	  setidxs(i++,j);
    phasidx(&i,1,8,C->p+8);
    phasidx(&i,2,C->p+9,2*C->p+9);
    phasidx(&i,3,2*C->p+10,3*C->p+10);
    H->N = i;
    hz4stop = FALSE;
  }
}

void STPWISE(void){
  int i,j,jvar1,nvarm1;
  double horder;

  if(O->restct!=0)
    RSTRICT();
  horder = O->ordmax;
  H->swadel = 0;

  if(O->orderv!=0) {
    if(H->sw==1)
      BACKW(C->status,H->pvalue,H->moves,H->mxmove,H->zvalue,
            H->stdErr,H->flags,H->qx,H->qtols,H->d2ll,h4nvar,H->wk,H->d2llad,
            O->rstvec);  /* to compute Q-statistics only */
    for(i=9; i<C->Ntheta; i++)
      if(O->orders[i]<horder) {
	H->hpsn = i;
	horder = O->orders[i];
      }
    O->orderv--;
    O->orders[H->hpsn] = O->ordmax;
    if(H->sw==1)
      H->swadel = 2;
    else
      H->swadel = 1;
  } else if(O->restct!=0)
    RSTBACK();
  if(H->swadel==0){
    if(H->sw==1){
      STEPW(C->status,H->pvalue,H->moves,H->mxmove,H->zvalue,
            H->stdErr,H->flags,H->qx,H->qtols,H->d2ll,h4nvar,H->wk,H->d2llad,
            O->rstvec);
    }else{ 
      BACKW(C->status,H->pvalue,H->moves,H->mxmove,H->zvalue,
            H->stdErr,H->flags,H->qx,H->qtols,H->d2ll,h4nvar,H->wk,H->d2llad,
            O->rstvec);
    }
  }
  if(C->errorno==1)
    HAZ4TRM(9900);
  else if(C->errorno==2)
    HAZ4TRM(9910);
  if(H->step>=H->mxstep && H->swadel!=0) {
    hzflg1("Note: Program being stopped by MAXSTEPS.",40);
    hzfskp(1);
    hzfxpc("*** Stopping after maximum steps specified.",43,5);
    hzfskp(2);
    H->swadel = 0;
    H->mxstop = 1;
  }
  if(H->nps==0 || (H->nps==1 && H->step==0)) {
    hzfpag(12);
    cflag1 = ' ';
    cflag2 = ' ';
    cflag3 = ' ';
    cflag4 = ' ';
    hzfxpc("Summary of Variables in the Model",33,51);
    hzfskp(1);
    hzfxpc("---------------------------------",33,51);
    hzfskp(2);
    if(H->label)
      SPRT2_L();
    else
      SPRT2();
    hzfskp(2);
  }
  if(H->nps==0 && H->npq==0)
    if((H->sw==1 || H->bw==1) && H->cnt>0) {
      hzfpag(12);
      hzfxpc("Summary of Variables not in the Model",37,36);
      hzfxpc(" (Based on Q Statistics)",24,0);
      hzfskp(2);
      if(H->label)
	SPRT3_L();
      else
	SPRT3();
    }
  if(cflag1!=' ') {
    hzfxpc("*    Negative second derivative indicating",42,5);
    hzfxpc(" possible singularity.",22,0);
    hzfskp(2);
  }
  if(cflag2!=' ') {
    hzfxpc("**   Variable ill conditioned indicating",40,5);
    hzfxpc(" possible singularity.",22,0);
    hzfskp(2);
  }
  if(cflag3!=' ') {
    hzfxpc("***  Tolerance of the Q is zero or",34,5);
    hzfxpc(" negative.",10,0);
    hzfskp(2);
  }
  if(cflag4!=' ') {
    hzfxpc("**** Estimate is larger than |50|.  The",39,5);
    hzfxpc(" model may be going to infinity.",32,0);
    hzfskp(2);
  }
  if(H->mode==0){
    if(H->fixmu==1){
      C->status[8] = 0;
    }else if(H->fixmu==2){
      C->status[C->p+9] = 0;
    }else if(H->fixmu==3){
      C->status[2*C->p+10] = 0;
    }
  }
  if(H->swadel==11) {
    hzfskp(1);
    hzfxpc("***** A variable must be dropped from the model due to "
	   "the MAXVARS specification.",81,5);
    hzfskp(2);
    H->swadel = 1;
  } else if(H->swadel==21) {
    hzfskp(1);
#ifndef NDEBUG
    fprintf(stderr, "***** A variable is being dropped from the model due to RESTRICT specifications.\n");
#endif /* NDEBUG */
    hzfxpc("***** A variable is being dropped from the model due to "
	   "RESTRICT specifications.",78,5);
    hzfskp(2);
    H->swadel = 1;
  }
  if(H->mxvars!=0 && H->swadel==2 && O->orderv==0) {
    j = 0;
    for(i=9; i<C->Ntheta; i++)
      if(C->status[i]==1 && i!=(C->p+9) && i!=(2*C->p+10))
	j++;
    if(j>=H->mxvars) {
      hzflg1("Note: Program being stopped by MAXVARS.",39);
      hzfskp(1);
      hzfxpc("***** Stopping with maximum variables allowed "
	     "into model by MAXVARS.",68,5);
      if(j>H->mxvars) {
	hzflg1("Warning: Unable to remove enough variables to "
	       "satisfy MAXVARS.",62);
	hzfskp(1);
	hzfxpc("***** Program was unable to remove enough variables "
	       "to satisfy MAXVARS.",71,5);
      }
      hzfskp(2);
      H->swadel = 0;
      H->mxstop = 1;
    }
  }
  if(H->swadel==0) 
    H->done = 1;
  else {
    H->pstep = H->pvalue[H->hpsn];
    H->zstep = H->zvalue[H->hpsn];
    O->ostep = horder;
    memcpy(H->vstep,H->names[H->hpsn],VAR_NAME_LENGTH);
    if(H->hpsn>8 && H->hpsn<C->p+9)
      memcpy(H->fstep,"EARLY   ",8);
    else if(H->hpsn>C->p+9 && H->hpsn<2*C->p+10)
      memcpy(H->fstep,"CONSTANT",8);
    else if(H->hpsn>2*C->p+10 && H->hpsn<3*C->p+11)
      memcpy(H->fstep,"LATE   ",8);
    else
      CLEAR(H->fstep);
    if(H->swadel==1) {
      H->moves[H->hpsn]++;
      j = 0;
      jvar1 = 0;
      for(i=0; i<h4nvar; i++)
	if(H->hpsn==H->indx[i])
	  j = i;
	else
	  H->indx2[jvar1++] = H->indx[i];
      DFAST(H->beta,h4nvar,H->cov,j,H->sd,H->znorm,H->pnorm);
      nvarm1 = h4nvar-1;
      for(i=0; i<nvarm1; i++) {
	j = H->indx2[i];
	C->theta[j] = H->beta[i];
      }
    } else if(O->nosw)
      H->moves[H->hpsn]++;
    if(C->status[H->hpsn]==1)
      C->status[H->hpsn] = 0;
    else
      C->status[H->hpsn] = 1;
    C->theta[H->hpsn] = ZERO;
  }
}

/* */
void RSTRICT(void){
  for(ri=9; ri<C->Ntheta; ri++){
    O->rstvec[ri] = 0;
  }
  /* Check restricted variables for each phase of the model */
  /* if C->phase[] == 1, we've included that phase */
  /* early = 1, constant = 2, late = 3 */
  if(C->phase[1]==1){
    RSTRPHZ(8);
  }
  if(C->phase[2]==1){
    RSTRPHZ(C->p+9);
  }
  if(C->phase[3]==1){
    RSTRPHZ(2*C->p+10);
  }
}

/* This is where each variable is restricted for each phase.
 * The theory goes, if one of the variables in the RESTRICT 
 * list enters the phase model, then we do not allow the 
 * others to enter the phase.
 */
void RSTRPHZ(int base){
  /* Loop through */
  for(ri=0,rbase=(-1); ri<O->restct; ri++) {
    if(O->rsttbl[ri].rstctr==1 && rbase!=ri) {
      rbase = ri;
      rset = 0;
    }

    /* This if statement could be cleaned up.
     * it either should be rvar == O->rsttbl[ri].rstvar
     * or the entire statement could be changed to 
     */
    rvar = O->rsttbl[ri].rstvar + base;
    
    /* Original statment 
       if(!(rvar = O->rsttbl[ri].rstvar)){
       continue;
       }else{
       rvar += base;
       }
    */
    if(rset==0){
      if(C->status[rvar]==1){
	rset = 1;
	ri = rbase-1;
	continue;
      }
    }else{
      O->rstvec[rvar] = 1;
    }
  }
}

/* */
void RSTBACK(void){
  if(C->phase[1]==1){
    RSTBPHZ(8);
  }
  if(H->swadel){
    return;
  }
  if(C->phase[2]==1){
    RSTBPHZ(C->p+9);
  }
  if(H->swadel){
    return;
  }
  if(C->phase[3]==1){
    RSTBPHZ(2*C->p+10);
  }
}

/* */
void RSTBPHZ(int base){

  H->swadel = 0;
  for(ri=0; ri<O->restct; ri++) {
    if(O->rsttbl[ri].rstctr==1){
      if(rset>1 && rbase){
	break;
      }else{
	rset = rbase = 0;
      }
    }
    if(!(rvar == O->rsttbl[ri].rstvar)){
      continue;
    }else{
      rvar += base;
    }
    if(C->status[rvar]==1){
      rset++;
      if(H->done!=1 && H->moves[rvar]>=H->mxmove[rvar]){
	continue;
      }
      if(!rbase){
	RSTBSET();
      }else{
	if(fabs(H->zvalue[rvar])<rz && H->pvalue[rvar]>=rp){
	  RSTBSET();
	}
      }
    }
  }
  if(rset>1 && rbase) {
    H->swadel = 21;
    H->hpsn = rbase;
  }
}

/* */
void RSTBSET(void){
  rbase = rvar;
  rp = H->pvalue[rbase];
  rz = fabs(H->zvalue[rbase]);
}

void STEPPRT(void){
  hzfpag(15);
  hzfskp(2);
  hzfxpc("Final Results:",14,5);
  hzfskp(1);
  hzfxpc("==============",14,5);
  hzfskp(3);
  hzfxpc("Parameter Estimate Summary",26,51);
  hzfskp(2);
  if(H->label){
    SPRT_L();
  }else{
    SPRT();
  }
  if(cflag1!=' ') {
    hzfxpc("*    Negative second derivative indicating",42,5);
    hzfxpc(" possible singularity.",22,0);
    hzfskp(2);
  }
  if(cflag2!=' ') {
    hzfxpc("**   Variable ill conditioned indicating",40,5);
    hzfxpc(" possible singularity.",22,0);
    hzfskp(2);
  }
  if(stopt){
    HAZ4TRM(9585);
  }
}

void SPRT_L(void){
  int i,j;

  SHDR_L();
  for(j=0; j<H->N; j++) {
    i = H->nx[j];
    hzfpag(3);
    if(C->phase[1]==1 && j==0){
      hzfxpc("Early:   ",9,5);
    }else if((C->phase[2]==1 && j==stepk1+1) ||
	     (C->phase[2]==1 && C->phase[1]==0 && j==0)){
      hzfxpc("Constant:",9,5);
    }else if((C->phase[3]==1 && j==stepk2+1) ||
	     (C->phase[3]==1 && C->phase[2]==0 && j==stepk1+1) ||
	     (C->phase[3]==1 && C->phase[2]==0 &&
	      C->phase[1]==0 && j==0)){
      hzfxpc("Late:    ",9,5);
    }else{
      hzfxpc("         ",9,5);
    }
    hzfxpc(H->namex[i],8,-2);
    hzfxpc(H->lablx[i],40,-4);
    hzfxpf(H->beta[i],12,99,-3);
    if(H->flag[i]==1 || H->flag[i]==2 || stopt) {
      hzfxpc("           .",12,-4);
      hzfxpc("          .",11,-4);
      hzfxpc("         .",10,-5);
      if(H->flag[i]==1) {
	hzfxpc("   *",4,0);
	cflag1 = 'Y';
      } else if(H->flag[i]==2) {
	hzfxpc("  **",4,0);
	cflag2 = 'Y';
      }
    }else{
      hzfxpf(H->sd[i],12,99,-3);
      hzfxpf(H->znorm[i],11,3,-4);
      if(H->pnorm[i]<PMIN){
	hzfxpc("      <.0001",12,-4);
      }else{
	hzfxpf(H->pnorm[i],10,4,-6);
      }
    }
    hzfskp(1);
    if((j==stepk1 && 
	(C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1))) ||
       (j==stepk2 && C->phase[3]==1)) {
      hzfxpr('-',117,16);
      hzfskp(1);
    }
  }
  hzfxpr('-',128,5);
  hzfskp(1);
}

void SPRT(void){
  int i,j;

  SHDR();
  for(j=0; j<H->N; j++) {
    i = H->nx[j];
    hzfpag(3);
    if(C->phase[1]==1 && j==0){
      hzfxpc("Early:   ",9,26);
    }else if((C->phase[2]==1 && j==stepk1+1) ||
	     (C->phase[2]==1 && C->phase[1]==0 && j==0)){
      hzfxpc("Constant:",9,26);
    }else if((C->phase[3]==1 && j==stepk2+1) ||
	     (C->phase[3]==1 && C->phase[2]==0 && j==stepk1+1) ||
	     (C->phase[3]==1 && C->phase[2]==0 &&
	      C->phase[1]==0 && j==0)){
      hzfxpc("Late:    ",9,26);
    }else{
      hzfxpc("         ",9,26);
    }
    hzfxpc(H->namex[i],8,-2);
    hzfxpf(H->beta[i],12,99,-4);
    if(H->flag[i]==1 || H->flag[i]==2 || stopt) {
      hzfxpc("           .",12,-3);
      hzfxpc("          .",11,-4);
      hzfxpc("         .",10,-5);
      if(H->flag[i]==1) {
	hzfxpc("   *",4,0);
	cflag1 = 'Y';
      } else if(H->flag[i]==2) {
	hzfxpc("  **",4,0);
	cflag2 = 'Y';
      }
    } else {
      hzfxpf(H->sd[i],12,99,-3);
      hzfxpf(H->znorm[i],11,3,-4);
      if(H->pnorm[i]<PMIN){
	hzfxpc("      <.0001",12,-4);
      }else{
	hzfxpf(H->pnorm[i],10,4,-6);
      }
    }
    hzfskp(1);
    if((j==stepk1 && (C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1))) ||
       (j==stepk2 && C->phase[3]==1)) {
      hzfxpr('-',74,37);
      hzfskp(1);
    }
  }
  hzfxpr('-',85,26);
  hzfskp(1);
}

void SHDR_L(void){
  hzfxpc("Phase",5,5);
  hzfxpc("Parameter",9,-6);
  hzfxpc("Label",5,-3);
  hzfxpc("Estimate",8,-39);
  hzfxpc("Std error",9,-7);
  hzfxpc("Z",1,-13);
  hzfxpc("Prob>|Z|",8,-10);
  hzfskp(1);
  hzfxpr('-',128,5);
  hzfskp(1);
}

void SHDR(void){
  hzfxpc("Phase",5,26);
  hzfxpc("Parameter",9,-6);
  hzfxpc("Estimate",8,-4);
  hzfxpc("Std error",9,-7);
  hzfxpc("Z",1,-13);
  hzfxpc("Prob>|Z|",8,-10);
  hzfskp(1);
  hzfxpr('-',85,26);
  hzfskp(1);
}

void SPRT2_L(void){
  int j,jvar1;

  SHDR2_L();
  for(j=8; j<C->Ntheta; j++) {
    hzfpag(3);
    if(C->phase[1]==1 && j==8) {
      hzfxpc("Early:   ",9,3);
      C->status[j] = 1;
      for(jvar1=0; jvar1<4; jvar1++){
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpc(H->labl[jvar1],40,-2);
	  hzfxpf(C->theta[jvar1],12,99,-2);
	  hzfskp(1);
	  hzfxpc("         ",9,3);
	}
      }
    } else if(C->phase[2]==1 && j==C->p+9) {
      hzfxpc("Constant:",9,3);
      C->status[j] = 1;
    } else if(C->phase[3]==1 && j==2*C->p+10) {
      hzfxpc("Late:    ",9,3);
      C->status[j] = 1;
      for(jvar1=4; jvar1<8; jvar1++){
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpc(H->labl[jvar1],40,-2);
	  hzfxpf(C->theta[jvar1],12,99,-2);
	  hzfskp(1);
	  hzfxpc("         ",9,3);
	} 
      }
    } else if(C->status[j]==1)
      hzfxpc("         ",9,3);
    if(C->status[j]==1) {
      hzfxpc(H->names[j],8,-2);
      hzfxpc(H->labl[j],40,-2);
      hzfxpf(C->theta[j],12,99,-2);
      movrem = H->mxmove[j]-H->moves[j];
      if(H->flags[j]==1 || H->flags[j]==2 || stopt) {
	hzfxpc("           .",12,-2);
	hzfxpc("          .",11,-3);
	hzfxpc("         .",10,-4);
	if(H->flags[j]==1) {
	  hzfxpc("   *",4,0);
	  cflag1 = 'Y';
	} else if(H->flags[j]==2) {
	  hzfxpc("  **",4,0);
	  cflag2 = 'Y';
	}
      } else {
	hzfxpf(H->stdErr[j],12,99,-2);
	hzfxpf(H->zvalue[j],11,3,-3);
	if(H->pvalue[j]<PMIN)
	  hzfxpc("      <.0001",12,-2);
	else
	  hzfxpf(H->pvalue[j],10,4,-4);
	if(!((C->phase[1]==1 && j==8) ||
	     (C->phase[2]==1 && j==C->p+9) ||
	     (C->phase[3]==1 && j==2*C->p+10))){
	  hzfxpi(movrem,3,-3);
	}
      }
      hzfskp(1);
    }
    if((j==C->p+8 && (C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1))) ||
       (j==2*C->p+9 && C->phase[2]==1 && C->phase[3]==1)) {
      hzfxpr('-',117,14);
      hzfskp(1);
    }
  }
  hzfxpr('-',128,3);
  hzfskp(1);
}

void SPRT2(void){
  int j,jvar1;

  SHDR2();
  for(j=8; j<C->Ntheta; j++) {
    hzfpag(3);
    if(C->phase[1]==1 && j==8) {
      hzfxpc("Early:   ",9,21);
      C->status[j] = 1;
      for(jvar1=0; jvar1<4; jvar1++)
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpf(C->theta[jvar1],12,99,-3);
	  hzfskp(1);
	  hzfxpc("         ",9,21);
	}
    } else if(C->phase[2]==1 && j==C->p+9) {
      hzfxpc("Constant:",9,21);
      C->status[j] = 1;
    } else if(C->phase[3]==1 && j==2*C->p+10) {
      hzfxpc("Late:    ",9,21);
      C->status[j] = 1;
      for(jvar1=4; jvar1<8; jvar1++)
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpf(C->theta[jvar1],12,99,-3);
	  hzfskp(1);
	  hzfxpc("         ",9,21);
	}
    } else if(C->status[j]==1)
      hzfxpc("         ",9,21);
    if(C->status[j]==1) {
      hzfxpc(H->names[j],8,-2);
      hzfxpf(C->theta[j],12,99,-3);
      movrem = H->mxmove[j]-H->moves[j];
      if(H->flags[j]==1 || H->flags[j]==2 || stopt) {
	hzfxpc("           .",12,-3);
	hzfxpc("          .",11,-4);
	hzfxpc("         .",10,-5);
	if(H->flags[j]==1) {
	  hzfxpc("   *",4,0);
	  cflag1 = 'Y';
	} else if(H->flags[j]==2) {
	  hzfxpc("  **",4,0);
	  cflag2 = 'Y';
	}
      } else {
	hzfxpf(H->stdErr[j],12,99,-3);
	hzfxpf(H->zvalue[j],11,3,-4);
	if(H->pvalue[j]<PMIN)
	  hzfxpc("      <.0001",12,-3);
	else
	  hzfxpf(H->pvalue[j],10,4,-5);
	if(!((C->phase[1]==1 && j==8) ||
	     (C->phase[2]==1 && j==C->p+9) ||
	     (C->phase[3]==1 && j==2*C->p+10))){
	  hzfxpi(movrem,3,-3);
	}
      }
      hzfskp(1);
    }
    if((j==C->p+8 && (C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1))) ||
       (j==2*C->p+9 && C->phase[2]==1 && C->phase[3]==1)) {
      hzfxpr('-',80,32);
      hzfskp(1);
    }
  }
  hzfxpr('-',91,21);
  hzfskp(1);
}

void SHDR2_L(void){
  hzfxpc("Moves",5,122);
  hzfskp(1);
  hzfxpc("Phase",5,3);
  hzfxpc("Variable",8,-6);
  hzfxpc("Label",5,-2);
  hzfxpc("Estimate",8,-38);
  hzfxpc("Std error",9,-6);
  hzfxpc("Z",1,-12);
  hzfxpc("Prob>|Z|",8,-8);
  hzfxpc("Remaining",9,-3);
  hzfskp(1);
  hzfxpr('-',128,3);
  hzfskp(1);
}

void SHDR2(void){
  hzfxpc("Moves",5,103);
  hzfskp(1);
  hzfxpc("Phase",5,21);
  hzfxpc("Variable",8,-6);
  hzfxpc("Estimate",8,-4);
  hzfxpc("Std error",9,-7);
  hzfxpc("Z",1,-13);
  hzfxpc("Prob>|Z|",8,-9);
  hzfxpc("Remaining",9,-4);
  hzfskp(1);
  hzfxpr('-',91,21);
  hzfskp(1);
}

void SPRT3_L(void){
  int j;

  SHDR3_L();
  stepk1 = 0;
  stepk2 = 0;
  stepk3 = 0;
  for(j=8; j<C->Ntheta; j++) {
    hzfpag(3);
    if(C->status[j]==0 && 
       ((H->phase1==1 && j<C->p+9) ||
	(H->phase2==1 && j>C->p+9 && j<2*C->p+10) ||
	(H->phase3==1 && j>2*C->p+10))) {
      if(H->cntp[1]>0 && stepk1==0 && j<C->p+9) {
	hzfxpc("Early:   ",9,3);
	stepk1 = 1;
      } else if(H->cntp[2]>0 && stepk2==0 && j>C->p+9 &&
		j<2*C->p+10) {
	stepk2 = 1;
	if(stepk1==1) {
	  hzfxpr('-',117,14);
	  hzfskp(1);
	}
	hzfxpc("Constant:",9,3);
      } else if(H->cntp[3]>0 && stepk3==0 && j>2*C->p+10) {
	stepk3 = 1;
	if(stepk2==1 || (stepk1==1 && stepk2==0)) {
	  hzfxpr('-',117,14);
	  hzfskp(1);
	}
	hzfxpc("Late:    ",9,3);
      } else
	hzfxpc("         ",9,3);
      hzfxpc(H->names[j],8,-2);
      hzfxpc(H->labl[j],40,-2);
      hzfxpf(H->qx[j],12,99,-2);
      movrem = H->mxmove[j]-H->moves[j];
      if(H->flags[j]>=1 && H->flags[j]<=4) {
	hzfxpc("           .",12,-2);
	hzfxpc("          .",11,-3);
	hzfxpc("         .",10,-4);
	if(H->flags[j]==1) {
	  hzfxpc("   *",4,0);
	  cflag1 = 'Y';
	} else if(H->flags[j]==2) {
	  hzfxpc("  **",4,0);
	  cflag2 = 'Y';
	} else if(H->flags[j]==3) {
	  hzfxpc(" ***",4,0);
	  cflag3 = 'Y';
	} else if(H->flags[j]==4) {
	  hzfxpc("****",4,0);
	  cflag4 = 'Y';
	}
      } else {
	hzfxpf(H->stdErr[j],12,99,-2);
	hzfxpf(H->zvalue[j],11,3,-3);
	if(H->pvalue[j]<PMIN)
	  hzfxpc("      <.0001",12,-2);
	else
	  hzfxpf(H->pvalue[j],10,4,-4);
	if(!((C->phase[1]==1 && j==8) ||
	     (C->phase[2]==1 && j==C->p+9) ||
	     (C->phase[3]==1 && j==2*C->p+10))){
#ifndef NDEBUG
	  fprintf(stderr, "Restricting O->rstvec[%d] = %d\n",
		  j, O->rstvec[j]);
#endif /* NDEBUG */
	  
	  if(O->rstvec[j]==1){
	    hzfxpc("RESTRICT",8,-3);
#ifndef NDEBUG
	    fprintf(stderr, "Restricted\n");
#endif /* NDEBUG */
	  }else{
	    hzfxpi(movrem,3,-3);
	  }
	}
      }
      hzfskp(1);
    }
  }
  hzfxpr('-',128,3);
  hzfskp(1);
}

void SPRT3(void){
  int j;

  SHDR3();
  stepk1 = 0;
  stepk2 = 0;
  stepk3 = 0;
  for(j=8; j<C->Ntheta; j++) {
    hzfpag(3);
    if(C->status[j]==0 && ((H->phase1==1 && j<C->p+9) ||
			   (H->phase2==1 && j>C->p+9 && j<2*C->p+10) ||
			   (H->phase3==1 && j>2*C->p+10))) {
      if(H->cntp[1]>0 && stepk1==0 && j<C->p+9) {
	hzfxpc("Early:   ",9,21);
	stepk1 = 1;
      } else if(H->cntp[2]>0 && stepk2==0 && j>C->p+9 &&
		j<2*C->p+10) {
	stepk2 = 1;
	if(stepk1==1) {
	  hzfxpr('-',80,32);
	  hzfskp(1);
	}
	hzfxpc("Constant:",9,21);
      } else if(H->cntp[3]>0 && stepk3==0 && j>2*C->p+10) {
	stepk3 = 1;
	if(stepk2==1 || (stepk1==1 && stepk2==0)) {
	  hzfxpr('-',80,32);
	  hzfskp(1);
	}
	hzfxpc("Late:    ",9,21);
      } else
	hzfxpc("         ",9,21);
      hzfxpc(H->names[j],8,-2);
      hzfxpf(H->qx[j],12,99,-3);
      movrem = H->mxmove[j]-H->moves[j];
      if(H->flags[j]>=1 && H->flags[j]<=4) {
	hzfxpc("           .",12,-3);
	hzfxpc("          .",11,-4);
	hzfxpc("         .",10,-5);
	if(H->flags[j]==1) {
	  hzfxpc("   *",4,0);
	  cflag1 = 'Y';
	} else if(H->flags[j]==2) {
	  hzfxpc("  **",4,0);
	  cflag2 = 'Y';
	} else if(H->flags[j]==3) {
	  hzfxpc(" ***",4,0);
	  cflag3 = 'Y';
	} else if(H->flags[j]==4) {
	  hzfxpc("****",4,0);
	  cflag4 = 'Y';
	}
      } else {
	hzfxpf(H->stdErr[j],12,99,-3);
	hzfxpf(H->zvalue[j],11,3,-4);
	if(H->pvalue[j]<PMIN)
	  hzfxpc("      <.0001",12,-3);
	else
	  hzfxpf(H->pvalue[j],10,4,-5);
	if(!((C->phase[1]==1 && j==8) ||
	     (C->phase[2]==1 && j==C->p+9) ||
	     (C->phase[3]==1 && j==2*C->p+10))){
	  if(O->rstvec[j]==1)
	    hzfxpc("RESTRICT",9,-3);
	  else
	    hzfxpi(movrem,3,-3);
	}
      }
      hzfskp(1);
    }
  }
  hzfxpr('-',91,21);
  hzfskp(1);
}

void SHDR3_L(void){
  hzfxpc("Moves",5,122);
  hzfskp(1);
  hzfxpc("Phase",5,3);
  hzfxpc("Variable",8,-6);
  hzfxpc("Label",5,-2);
  hzfxpc("Estimate",8,-38);
  hzfxpc("Std error",9,-6);
  hzfxpc("Z",1,-12);
  hzfxpc("Prob>|Z|",8,-8);
  hzfxpc("Remaining",9,-3);
  hzfskp(1);
  hzfxpr('-',128,3);
  hzfskp(1);
}

void SHDR3(void){
  hzfxpc("Moves",5,103);
  hzfskp(1);
  hzfxpc("Phase",5,21);
  hzfxpc("Variable",8,-6);
  hzfxpc("Estimate",8,-4);
  hzfxpc("Std error",9,-7);
  hzfxpc("Z",1,-13);
  hzfxpc("Prob>|Z|",8,-9);
  hzfxpc("Remaining",9,-4);
  hzfskp(1);
  hzfxpr('-',91,21);
  hzfskp(1);
}

void HAZ4TRM(int errcode){
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
    hzflg2("ERROR: Internal sign violation ",33,C->errflg,12);
    break;
  case 9585:
    hzfxpc("Due to above singularity",24,5);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Possible singularity detected...",39,
	   "see listing below.",18);
  }
  hazrerr();
}

void FAST(void){
  stepf = 0;
  faststop = FALSE;
  FASTPRT();
  while(!faststop)
    FASLOOP();
}

void FASLOOP(void){
  int i,j,jvar1;

  faststop = TRUE;
  H->hpsn = 0;
  hval = H->sls;
  if(H->phase1==1)
    FASTSET(9,C->p+8);
  if(H->phase2==1)
    FASTSET(C->p+10,2*C->p+9);
  if(H->phase3==1)
    FASTSET(2*C->p+11,3*C->p+10);
  if(H->hpsn!=0) {
    stepf++;
    FASNOTE();
    C->status[H->hpsn] = 0;
    C->theta[H->hpsn] = ZERO;
    H->moves[H->hpsn]++;
    j = 0;
    jvar1 = 0;
    for(i=0; i<H->N; i++)
      if(H->hpsn==H->indx[i])
	j = i;
      else
	H->indx2[jvar1++] = H->indx[i];
    DFAST(H->beta,H->N,H->cov,j,H->sd,H->znorm,H->pnorm);
    H->N--;
    if(C->errorno!=0)
      FASTTRM();
    for(i=0; i<H->N; i++) {
      j = H->indx2[i];
      H->indx[i] = H->indx2[i];
      C->theta[j] = H->beta[i];
      H->pvalue[j] = H->pnorm[i];
      H->stdErr[j] = H->sd[i];
      H->zvalue[j] = H->znorm[i];
    }
    FASTPRT();
    faststop = FALSE;
  } else {
    H->fdone = 1;
    hzfpag(12);
    hzfxpc("Fast completed.",15,5);
    hzfskp(2);
    if(H->sw==1)
      hzfxpc("Forward stepwise selection",26,53);
    else
      hzfxpc("Backward stepwise selection",27,53);
    hzfskp(2);
    hzfxpc("*--------*",10,1);
    hzfskp(1);
    hzfxpc("|Step  1.|",10,1);
    hzfskp(1);
    hzfxpc("*--------*",10,1);
    hzfskp(2);
  }
}

void FASTSET(int ixf,int ixt){
  int j;

  for(j=ixf; j<=ixt; j++)
    if(C->status[j]==1)
      if(H->pvalue[j]>H->sls)
	if(H->moves[j]<H->mxmove[j])
	  if(H->pvalue[j]>hval) {
	    hval = H->pvalue[j];
	    H->hpsn = j;
	  }
}

void FASNOTE(void){
  char fstpnm[20];

  hzfpag(18);
  H->pstep = H->pvalue[H->hpsn];
  H->zstep = H->zvalue[H->hpsn];
  memcpy(H->vstep,H->names[H->hpsn],VAR_NAME_LENGTH);
  hzfskp(2);
  hzfxpc("*",1,5);
  hzfxpr('-',63,0);
  hzfxpc("*",1,0);
  hzfskp(1);
  hzfxpc("|Fast step ",11,5);
  hzfxpi(stepf,3,0);
  hzfxpc(". ",2,0);
  hzfxpc(H->vstep,8,0);
  hzfxpc(" deleted from ",14,0);
  if(H->hpsn>8 && H->hpsn<C->p+9)
    memcpy(fstpnm,"EARLY PHASE         ",20);
  else if(H->hpsn>C->p+10 && H->hpsn<2*C->p+11)
    memcpy(fstpnm,"CONSTANT PHASE      ",20);
  else if(H->hpsn>2*C->p+11 && H->hpsn<3*C->p+12)
    memcpy(fstpnm,"LATE PHASE          ",20);
  else
    CLEAR(fstpnm);
  hzfxpc(fstpnm,20,0);
  hzfxpc("|",1,-6);
  hzfskp(1);
  hzfxpc("|",1,5);
  hzfxpc("with Z =",8,-24);
  hzfxpf(H->zstep,11,3,-1);
  hzfxpc(" and P =",8,0);
  hzfxpf(H->pstep,10,4,-1);
  hzfxpc("|",1,0);
  hzfskp(1);
  hzfxpc("*",1,5);
  hzfxpr('-',63,0);
  hzfxpc("*",1,0);
  hzfskp(2);
}

void FASTPRT(void){
  hzfpag(10);
  hzfxpc("Summary of Variables in the Model (Fast option)",47,45);
  hzfskp(1);
  hzfxpc("-----------------------------------------------",47,45);
  hzfskp(2);
  if(H->nps==0){
    if(H->label){
      FPRT_L();
    }else{
      FPRT();
    }
  }
}

void FPRT_L(void){
  int i,j,jvar1;

  FHDR_L();
  for(i=0; i<H->N; i++) {
    hzfpag(3);
    j = H->indx[i];
    if((j==C->p+9 && (C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1))) ||
       (j==2*C->p+10 && C->phase[2]==1 && C->phase[3]==1)) {
      hzfxpr('-',117,14);
      hzfskp(1);
    }
    if(C->phase[1]==1 && j==8) {
      hzfxpc("Early:   ",9,3);
      C->status[j] = 1;
      for(jvar1=0; jvar1<4; jvar1++)
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpc(H->labl[jvar1],40,-2);
	  hzfxpf(C->theta[jvar1],12,99,-2);
	  hzfskp(1);
	  hzfxpc("         ",9,3);
	}
    } else if(C->phase[2]==1 && j==C->p+9) {
      hzfxpc("Constant:",9,3);
      C->status[j] = 1;
    } else if(C->phase[3]==1 && j==2*C->p+10) {
      hzfxpc("Late:    ",9,3);
      C->status[j] = 1;
      for(jvar1=4; jvar1<8; jvar1++)
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpc(H->labl[jvar1],40,-2);
	  hzfxpf(C->theta[jvar1],12,99,-2);
	  hzfskp(1);
	  hzfxpc("         ",9,3);
	}
    } else
      hzfxpc("         ",9,3);
    if(C->status[j]==1) {
      hzfxpc(H->names[j],8,-2);
      hzfxpc(H->labl[j],40,-2);
      hzfxpf(H->beta[i],12,99,-2);
      hzfxpf(H->sd[i],12,99,-2);
      hzfxpf(H->znorm[i],11,3,-3);
      if(H->pnorm[i]<PMIN)
	hzfxpc("      <.0001",12,-2);
      else
	hzfxpf(H->pnorm[i],10,4,-4);
      movrem = H->mxmove[j]-H->moves[j];
      if(!((C->phase[1]==1 && j==8) ||
	   (C->phase[2]==1 && j==C->p+9) ||
	   (C->phase[3]==1 && j==2*C->p+10))){
	hzfxpi(movrem,3,-3);
      }
      hzfskp(1);
    }
  }
  hzfxpr('-',128,3);
  hzfskp(1);
}

void FPRT(void){
  int i,j,jvar1;

  FHDR();
  for(i=0; i<H->N; i++) {
    hzfpag(3);
    j = H->indx[i];
    if((j==C->p+9 && (C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1))) ||
       (j==2*C->p+10 && C->phase[2]==1 && C->phase[3]==1)) {
      hzfxpr('-',80,32);
      hzfskp(1);
    }
    if(C->phase[1]==1 && j==8) {
      hzfxpc("Early:   ",9,21);
      C->status[j] = 1;
      for(jvar1=0; jvar1<4; jvar1++)
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpf(C->theta[jvar1],12,99,-3);
	  hzfskp(1);
	  hzfxpc("         ",9,21);
	}
    } else if(C->phase[2]==1 && j==C->p+9) {
      hzfxpc("Constant:",9,21);
      C->status[j] = 1;
    } else if(C->phase[3]==1 && j==2*C->p+10) {
      hzfxpc("Late:    ",9,21);
      C->status[j] = 1;
      for(jvar1=4; jvar1<8; jvar1++)
	if(C->status[jvar1]==1) {
	  hzfxpc(H->names[jvar1],8,-2);
	  hzfxpf(C->theta[jvar1],12,99,-3);
	  hzfskp(1);
	  hzfxpc("         ",9,21);
	}
    } else
      hzfxpc("         ",9,21);
    if(C->status[j]==1) {
      hzfxpc(H->names[j],8,-2);
      hzfxpf(H->beta[i],12,99,-3);
      hzfxpf(H->sd[i],12,99,-3);
      hzfxpf(H->znorm[i],11,3,-4);
      if(H->pnorm[i]<PMIN)
	hzfxpc("      <.0001",12,-3);
      else
	hzfxpf(H->pnorm[i],10,4,-5);
      movrem = H->mxmove[j]-H->moves[j];
      if(!((C->phase[1]==1 && j==8) ||
	   (C->phase[2]==1 && j==C->p+9) ||
	   (C->phase[3]==1 && j==2*C->p+10))){
	hzfxpi(movrem,3,-3);
      }
      hzfskp(1);
    }
  }
  hzfxpr('-',91,21);
  hzfskp(1);
}

void FHDR_L(void){
  hzfxpc("Moves",5,122);
  hzfskp(1);
  hzfxpc("Phase",5,3);
  hzfxpc("Variable",8,-6);
  hzfxpc("Label",5,-2);
  hzfxpc("Estimate",8,-38);
  hzfxpc("Std error",9,-6);
  hzfxpc("Z",1,-12);
  hzfxpc("Prob>|Z|",8,-8);
  hzfxpc("Remaining",9,-3);
  hzfskp(1);
  hzfxpr('-',128,3);
  hzfskp(1);
}

void FHDR(void){
  hzfxpc("Moves",5,103);
  hzfskp(1);
  hzfxpc("Phase",5,21);
  hzfxpc("Variable",8,-6);
  hzfxpc("Estimate",8,-4);
  hzfxpc("Std error",9,-7);
  hzfxpc("Z",1,-13);
  hzfxpc("Prob>|Z|",8,-9);
  hzfxpc("Remaining",9,-4);
  hzfskp(1);
  hzfxpr('-',91,21);
  hzfskp(1);
}

void FASTTRM(void){
  hzfpag(10);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  if(C->errorno==1) {
    hzfxpc("A computation has exceeded its limits",37,5);
    hzfskp(1);
    hzflg1("ERROR: Computation has exceeded limit.",38);
  } else if(C->errorno==2) {
    hzfxpc("Possible incorrect internal parameter specification..."
	   "please send a copy of this output to the Foundation",105,5);
    hzfskp(1);
  }
  hazrerr();
}

void HZ5G(void){
  H->ierr = C->errorno;
  DTRSMU(2);
  if(C->errorno!=0 && H->ierr==0)
    HAZ5TRM();
  hzfpag(20);
  if(C->errorno!=0 || H->ierr!=0) {
    hzfxpc("Results at Termination:",23,5);
    hzfskp(1);
    hzfxpc("=======================",23,5);
  }
  hzfskp(2);
  hzfxpc("Estimates for Model Parameters",30,51);
  hzfskp(2);
  hzfxpc("Phase",5,43);
  hzfxpc("Parameter",9,-7);
  hzfxpc("Fixed?",6,-5);
  hzfxpc("Estimate",8,-5);
  hzfskp(1);
  hzfxpr('-',48,43);
  hzfskp(1);
  if(C->phase[1]==1)
    FINEMOD();
  if(C->phase[1]==1 && (C->phase[2]==1 || C->phase[3]==1)) {
    hzfxpr('-',36,55);
    hzfskp(1);
  }
  if(C->phase[2]==1)
    FINCMOD();
  if(C->phase[2]==1 && C->phase[3]==1) {
    hzfxpr('-',36,55);
    hzfskp(1);
  }
  if(C->phase[3]==1)
    FINLMOD();
  hzfxpr('-',48,43);
  hzfskp(1);
  if(C->errorno!=0 || H->ierr!=0)
    return;
  nrow = (int) ceil((H->N+ONE)/8);
  if(H->nocov!=1 && H->N!=1) {
    hzfpag(nrow*2+9);
    hzfskp(2);
    hzfxpc("Asymptotic Variance-Covariance Matrix",37,49);
    hzfskp(2);
    MTRXPRT(H->cov);
  }
  if(H->nocor!=1 && H->N!=1) {
    hzfpag(nrow*2+9);
    hzfskp(2);
    hzfxpc("Asymptotic Correlation Matrix",29,54);
    hzfskp(2);
    MTRXPRT(H->corr);
  }
}

void FINEMOD(void){
  struct early *E;

  E = C->early;
  FINPARM("Early:",0,E->delta);
  FINPARM(" ",1,E->tHalf);
  hzfxpc(" (RHO",5,55);
  hzfxpf(E->rho,12,99,-19);
  hzfxpc(")",1,0);
  hzfskp(1);
  FINPARM(" ",2,E->nu);
  FINPARM(" ",3,E->m);
  hzfxpc("         ",9,43);
  hzfxpc("MUE     ",8,-3);
  hzfxpc("   ",3,-6);
  hzfxpf(E->muE,12,99,-7);
  hzfskp(1);
}

void FINCMOD(void){
  hzfxpc("Constant:",9,43);
  hzfxpc("MUC     ",8,-3);
  hzfxpc("   ",3,-6);
  hzfxpf(C->constant->muC,12,99,-7);
  hzfskp(1);
}

void FINLMOD(void){
  struct late *L;

  L = C->late;
  FINPARM("Late:",4,L->tau);
  FINPARM(" ",5,L->gamma);
  FINPARM(" ",6,L->alpha);
  FINPARM(" ",7,L->eta);
  hzfxpc("         ",9,43);
  hzfxpc("MUL     ",8,-3);
  hzfxpc("   ",3,-6);
  hzfxpf(L->muL,12,99,-7);
  hzfskp(1);
}

void FINPARM(char *phase,int parmno,double useval){
  short int len;

  len = strlen(phase);
  hzfxpc(phase,len,43);
  hzfxpc("         ",9-len,0);
  len = strlen(parms[parmno]);
  hzfxpc(parms[parmno],len,55);
  hzfxpc("        ",8-len,0);
  if(C->status[parmno]==0)
    hzfxpc("Yes",3,-6);
  else
    hzfxpc("No ",3,-6);
  hzfxpf(useval,12,99,-7);
  hzfskp(1);
}

void MTRXPRT(double *mtrx){
  int i,i1,j,n1,pos;

  MTRXHDR();
  stepk1 = 0;
  stepk2 = 0;
  stepk3 = 0;
  for(i=0; i<H->N; i++) {
    i1 = H->nx[i];
    hzfpag(5);
    if(stepk1==0 && C->phase[1]==1 &&
       (memcmp(H->namex[i1],"E1      ",8)==0 ||
	memcmp(H->namex[i1],"E2      ",8)==0 ||
	memcmp(H->namex[i1],"E3      ",8)==0 ||
	memcmp(H->namex[i1],"E4      ",8)==0 ||
	memcmp(H->namex[i1],"E0      ",8)==0)) {
      hzfxpc("Early:   ",9,5);
      hzfskp(2);
      stepk1 = 1;
    } else
      if(stepk2==0 && C->phase[2]==1 &&
	 memcmp(H->namex[i1],"C0      ",8)==0) {
	stepk2 = 1;
	if(stepk1==1) {
	  hzfxpr('-',122,8);
	  hzfskp(1);
	}
	hzfxpc("Constant:",9,5);
	hzfskp(2);
      } else
	if(stepk3==0 && C->phase[3]==1 &&
	   (memcmp(H->namex[i1],"L1      ",8)==0 ||
	    memcmp(H->namex[i1],"L2      ",8)==0 ||
	    memcmp(H->namex[i1],"L3      ",8)==0 ||
	    memcmp(H->namex[i1],"L4      ",8)==0 ||
	    memcmp(H->namex[i1],"L0      ",8)==0)) {
	  stepk3 = 1;
	  if(stepk2==1 || (stepk1==1 && stepk2==0)) {
	    hzfxpr('-',122,8);
	    hzfskp(1);
	  }
	  hzfxpc("Late:    ",9,5);
	  hzfskp(2);
	}
    hzfxpc(H->namex[i1],8,8);
    for(j=0,n1=0,pos=21; j<H->N; j++,n1++,pos+=14) {
      if(n1==8) {
	hzfskp(2);
	n1 = 0;
	pos = 21;
      }
      hzfxpf(mtrx[i1*H->N+H->nx[j]],12,99,pos);
    }
    hzfskp(2);
  }
  hzfxpr('-',125,5);
  hzfskp(1);
}

void MTRXHDR(void){
  int j,n1,pos;

  for(j=0,n1=0,pos=24; j<H->N; j++,n1++,pos+=14) {
    if(n1==8) {
      hzfskp(1);
      n1 = 0;
      pos = 24;
    }
    hzfxpc(H->namex[H->nx[j]],8,pos);
  }
  hzfskp(1);
  hzfxpr('-',125,5);
  hzfskp(1);
}

void HAZ5TRM(void){
  hzfpag(10);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  if(C->errorno==1) {
    hzfxpc("A computation has exceeded its limits",37,5);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Computation has exceeded limit #.",40,
	   C->errflg,12);
  } else if(C->errorno==2) {
    hzfxpc("Internal sign violation, possibly data or",41,5);
    hzfxpc("specification dependent",23,-1);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Internal sign violation #.",33,C->errflg,12);
  }
}
