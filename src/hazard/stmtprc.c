#include "hazard.h"

#include <hzdinc.h>

#include "hazpprc.h"
#include "timeprc.h"
#include "evntprc.h"
#include "rcnsprc.h"
#include "icnsprc.h"
#include "lcnsprc.h"
#include "wghtprc.h"
#include "parmprc.h"
#include "stpwprc.h"

/****************************************************************/
/* STMTPRC called from main routine                             */
void stmtprc(void){
  long i;

  H->weibul = FALSE;
  H->angrad = TRUE;
  H->anhess = TRUE;
  H->ihpass = FALSE;
  H->truhes = TRUE;
  H->newton = TRUE;
  swnewt = TRUE;
  swhess = TRUE;
  E->delta0 = FALSE;
  H->e.delta = ZERO;
  H->e.tHalf = ONE;
  H->e.nu = TWO;
  H->e.m = ONE;
  H->l.tau = ZERO;
  H->l.gamma = ONE;
  H->l.alpha = ONE;
  H->l.eta = TWO;
  H->e.muE = ZERO;
  H->c.muC = ZERO;
  H->l.muL = ZERO;
  E->mNuOne = FALSE;
  L->g_two = FALSE;
  L->ga_two = FALSE;
  H->fixg1 = FALSE;
  H->fixg3 = FALSE;
  E->g1flag = 0;
  L->g3flag = 0;
  CLEAR(tname);
  CLEAR(c1name);
  CLEAR(c2name);
  CLEAR(c3name);
  CLEAR(ctname);
  CLEAR(lcname);
  CLEAR(wtname);
  H->sle = 0.3e0;
  H->fixmu = 0;
  H->sw = 0;
  H->bw = 0;
  H->f = 0;
  H->swadel = 0;
  H->step = 0;
  H->fdone = 0;
  H->mov = 1;
  H->mode = 1;
  H->nfncts = 0;
  C->p = 0;
  prnt = 0;
  H->nps = 0;
  H->npq = 0;
  O->nosw = 0;
  H->nocov = 0;
  H->nocor = 0;
  O->itrlmt = 50;
  O->condno = ZERO;
  for(i=20; i; i--)
    ispec(i) = 0;
  for(i=8; i; i--)
    t_status(i) = 1;
  CLEAR(c1labl);
  CLEAR(c2labl);
  CLEAR(c3labl);
  CLEAR(ctlabl);
  CLEAR(tlabl);
  CLEAR(lcname);
  CLEAR(wtname);
  for(i=3; i; i--)
    C->phase[i] = 0;
  hazpprc();
  timeprc();
  evntprc();
  rcnsprc();
  icnsprc();
  lcnsprc();
  wghtprc();
  parmprc();
  stpwprc();
  if(C->phase[1]==1){
    if(ispec(2)==0 || ispec(2)==2){
      t_status(1) = 0;
    }
  }else {
    E->delta = ZERO;
    E->tHalf = ZERO;
    E->rho = ZERO;
  
    E->nu = ZERO;
    E->m = ZERO;
    t_status(1) = 0;
    t_status(2) = 0;
    t_status(3) = 0;
    t_status(4) = 0;
  }
  if(C->phase[3]==0) {
    L->tau = ZERO;
    L->gamma = ZERO;
    L->alpha = ZERO;
    L->eta = ZERO;
    t_status(5) = 0;
    t_status(6) = 0;
    t_status(7) = 0;
    t_status(8) = 0;
  }
  if(ispec(17)==1 && ispec(18)==0){
    H->mode = 0;
  }else if(ispec(17)==0 && ispec(18)==1){
    H->mode = 2;
  }
  if(ispec(19)==0 && ispec(20)==0) {
    swhess = H->truhes;
    swnewt = H->newton;
  }
  if(ispec(19)==1 && ispec(20)==1){
    swhess = FALSE;
  }
  if(H->sw==1 || H->bw==1 || H->f==1) {
    if(H->sw==1 && H->bw==1){
      H->sw = 0;
    }
    H->done = 0;
  }else{
    H->done = 1;
  }
  if(ispec(15)==0){
    if(H->sw==1) {
      H->sls = 0.2e0;
      H->bw = 0;
    } else if(H->bw==1 || H->f==1){
      H->sls = 0.05e0;
    }
  }
}
