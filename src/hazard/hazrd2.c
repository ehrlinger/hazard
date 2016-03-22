#include <string.h>
#include <math.h>

#include "hazard.h"
#include "hazrd2.h"
#include "theta_x.h"
#include "setidxs.h"
#include "phasidx.h"
#include "hazrerr.h"
#include "hazrd3.h"
#include "hazrd4.h"
#include <setcoe.h>
#include <setlik.h>
#include <setobj.h>
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>
      
#include <hzfpag.h>
#include <hzflg2.h>

#include <constp.h>

void HAZRD2(void){
  CLEAR(H->vstep);
  CLEAR(H->fstep);
  H->zstep = ZERO;
  H->pstep = ZERO;
  O->ostep = O->ordmax;
  H->lstvar = 0;
  svnewt = H->newton;
  svhess = H->truhes;
  H->newton = swnewt;
  H->truhes = swhess;
  hz2stop = FALSE;
  while(!hz2stop)
    HZ2LOOP();
  if(C->errorno!=0)
    H->nocov = 1;
  HZ5G();
}

void HZ2LOOP(void){
  int i,j;

  hz2stop = TRUE;
  for(i=0; i<H->maxn; i++) {
    H->index[i] = 0;
    H->x[i] = 0;
    H->nm1dx[i] = 0;
    CLEAR(H->namex[i]);
    CLEAR(H->lablx[i]);
  }
  H->retry = 1;
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
  if(H->N>tally)
    HAZ2TRM(7104);
  H->only1 = (H->N==1 && H->mode!=2);
  if(H->only1==0 && O->itrlmt>0)
    DOOPTIM();
  else
    NOOPTIM();
  theta_x();
  HZ4G();
  if(C->errorno==999)
    hazrerr();
  if((H->sw==1 || H->bw==1 || H->f==1) && H->done==0) {
    if(H->mode!=2)
      constp();
    if(C->errorno==1)
      HAZ2TRM(9900);
    else if(C->errorno==2)
      HAZ2TRM(9910);
    H->step++;
    H->newton = svnewt;
    H->truhes = svhess;
    hz2stop = FALSE;
  }
}

void DOOPTIM(void){
  int j,j1;

  if(H->mode==0) {
    j1 = 0;
    for(j=0; j<H->N; j++){
      if(!((C->phase[1]==1 && H->fixmu==1 && H->index[j]==8) ||
	   (C->phase[2]==1 && H->fixmu==2 && H->index[j]==C->p+9) ||
	   (C->phase[3]==1 && H->fixmu==3 && H->index[j]==2*C->p+10))){
	SETOPTX(j,j1++);
      }
    }
  } else{
    for(j=0; j<H->N; j++){
      SETOPTX(j,j);
    }
  }
  HAZRD3();
  if(C->errorno==999)
    hazrerr();
  if(H->retry==2) {
    if(H->mode!=2)
      constp();
    hz2stop = FALSE;
  }
}

void SETOPTX(int ixf,int ixt){
  H->nm1dx[ixt] = H->index[ixf];
  H->x0[ixt] = H->x[ixf];
  H->typx[ixt] = fabs(H->x[ixf]);
  if(H->x[ixf]==ZERO)
    H->typx[ixt] = ONE;
  if(H->index[ixf]==H->hpsn && H->swadel==2)
    H->typx[ixt] = fabs(H->qx[H->hpsn]);
}

void NOOPTIM(void){
  double llike;

  if(O->itrlmt==0)
    dllike(&llike);
  else {
    DCEOBJ(&llike);
    llike = -llike;
  }
  if(C->errorno==1)
    HAZ2TRM(9900);
  else if(C->errorno==2)
    HAZ2TRM(9910);
  H->slike = llike;
  hzfxpc("Log Likelihood = ",17,9);
  hzfxpf(llike,12,99,0);
  hzfskp(2);
}

void HAZ2TRM(int errcode){
  hzfpag(10);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  switch(errcode) {
  case 7104:
    hzfxpc("The number of parameters to be estimated",40,7);
    hzfxpc(" is greater than the number of events. ",39,0);
    hzfxpc(" Procedure is terminated.",25,0);
    hzfskp(1);
    hzflg2("ERROR: Num of parms to estimate is greater",42,
	   " than the num of events.",24);
    break;
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
  }
  hazrerr();
}
