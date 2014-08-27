#include <common.h>
#include "hazard.h"

#include <hzdinc.h>

#include "readt.h"
#include "readc1.h"
#include "readc2.h"
#include "readc3.h"
#include "readct.h"
#include "readlc.h"
#include "readwt.h"
#include "setobs.h"

#include "isblank.h"
#include <xvgetd.h>
#include <xoget.h>  
#include <xvget.h> 
#include <hzfxpc.h>
#include <hzfskp.h>
#include <hzf_log2.h>
#include <hzfxit.h>



/****************************************************************/
/* READOBS called from main                                     */
void readobs(void){
  long i,j,xocnt;

  H->label = FALSE;
  events = ZERO;
  intcen = ZERO;
  nosubs = ZERO;
  tally = ZERO;
  rtcen = ZERO;
  
  tmin = Machine->absMax;
  c1min = Machine->absMax;
  c2min = Machine->absMax;
  c3min = Machine->absMax;
  ctmin = Machine->absMax;
  lcmin = Machine->absMax;
  wtmin = Machine->absMax;
  H->Tmax = Machine->negMax;
  c1max = Machine->negMax;
  c2max = Machine->negMax;
  c3max = Machine->negMax;
  ctmax = Machine->negMax;
  lcmax = Machine->negMax;
  wtmax = Machine->negMax;
  for(j=0; j<C->p; j++) {
    conmin[j] = Machine->absMax;
    conmax[j] = Machine->negMax;
    contal[j] = ZERO;
    conmis[j] = 0;
  }
  delet = 0;
  mdelet = 0;
  ivent = 0;
  tdel = 0;
  c1del = 0;
  c2del = 0;
  c3del = 0;
  ctdel = 0;
  mtdel = 0;
  mc1del = 0;
  mc2del = 0;
  mc3del = 0;
  mctdel = 0;
  c1flag = 0;
  c2flag = 0;
  c3flag = 0;
  alldel = 0;
  c2chg = 0;
  lcdel = 0;
  lcens = 0;
  wtdel = 0;
  mlcdel = 0;
  mwtdel = 0;
  if(!ISBLANK(tname))
    xvgetd(T_ns,&T);
  if(!ISBLANK(c1name))
    xvgetd(C1_ns,&C1);
  if(!ISBLANK(c2name))
    xvgetd(C2_ns,&C2);
  if(!ISBLANK(c3name)) {
    xvgetd(C3_ns,&C3);
    xvgetd(CT_ns,&CT);
  }
  if(!ISBLANK(lcname))
    xvgetd(LC_ns,&LC);
  if(!ISBLANK(wtname))
    xvgetd(WT_ns,&WT);
  for(j=0; j<C->p; j++)
    xvgetd(risk_ns[j],&concom[j]);
  obsix = 0;
  for(xocnt=C->Nobs; xocnt; xocnt--) {
    xoget();
    ic10 = 0;
    ic20 = 0;
    ic30 = 0;
    del = 0;
    mdel = 0;
    xvget();
    readt();
    readc1();
    readc2();
    readc3();
    readct();
    readlc();
    readwt();
    if(!ISBLANK(c1name) && !ISBLANK(c2name) && ISBLANK(c3name))
      if(ic10==1 && ic20==1) {
	alldel = alldel+1;
	c1del = c1del+1;
	c2del = c2del+1;
	del = 1;
      }
    if(ISBLANK(c1name) && !ISBLANK(c2name) && !ISBLANK(c3name))
      if(ic30==1 && ic20==1) {
	alldel = alldel+1;
	c3del = c3del+1;
	c2del = c2del+1;
	del = 1;
      }
    for(i=0; i<C->p; i++)
      if(ISMISS(concom[i])) {
	conmis[i]++;
	mdel = 1;
      }
    if(del==0 && mdel==0)
      setobs();
    else {
      delet = delet+1;
      if(mdel>0)
	mdelet = mdelet+1;
    }
  }
  C->Nobs = C->Nobs-delet;
  if(C->Nobs<=0) {
    hzfxpc("NO OBSERVATIONS MET THE CRITERIA FOR",36,7);
    hzfxpc(" PROCESSING.   PROCEDURE IS TERMINATED.",38,0);
    hzfskp(1);
    hzf_log2("ERROR: NO OBSERVATIONS MET THE CRITERIA ",
	     "FOR PROCESSING.");
    hzfxit("DATA");
  }
  if(C->Nobs!=0)
    for(j=0; j<C->p; j++)
      comean[j] = contal[j]/C->Nobs;
  if(C->phase[3]==1 && ispec(8)!=1)
    H->l.tau = 0.75e0*H->Tmax;
}
