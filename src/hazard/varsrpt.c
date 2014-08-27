#include "hazard.h"
#include "isblank.h"
#include <hzfmis.h>
#include <hzfxpc.h>
#include <hzfskp.h>

#include "vrpt.h"
#include "vrpt_d.h"
#include "vrpt_l.h"
#include "vrpt_dl.h"
#include "obsstat.h"
#include "varnote.h"
#include "varterm.h"

void varsrpt(void){
  long j;

  if(!ISBLANK(tlabl) || !ISBLANK(c1labl) || !ISBLANK(c2labl) || 
     !ISBLANK(c3labl) || !ISBLANK(ctlabl))
    H->label = TRUE;
  if(!lcens) {
    MKMISS(lcmin);
    MKMISS(lcmax);
  }
  if(ctmin==Machine->absMax && ctmax==Machine->negMax && c3min==ZERO && 
     c3max==ZERO) {
    MKMISS(ctmin);
    MKMISS(ctmax);
  }
  hzfxpc("Initial Summary:",16,5);
  hzfskp(1);
  hzfxpc("================",16,5);
  hzfskp(1);
  obsstat();
  if(mtdel>0 || tdel>0 || mc1del>0 || c1del>0 || mc2del>0 || c2del>0 || 
     mc3del>0 || c3del>0 || mctdel>0 || ctdel>0 ||
     mlcdel>0 || lcdel>0 || mwtdel>0 || wtdel>0)
    if(H->label)
      vrpt_dl();
    else
      vrpt_d();
  else
    if(H->label)
      vrpt_l();
    else
      vrpt();
  if(c1flag==1 || c2flag==1 || c3flag==1 || ivent>0 ||
     c2chg>0)
    varnote();
  hzfskp(4);
  for(j=0; j<C->p; j++) {
    if(iearly[j]!=1)
      C->status[j+9] = 2;
    if(iconst[j]!=1)
      C->status[j+10+C->p] = 2;
    if(ilate[j]!=1)
      C->status[j+11+2*C->p] = 2;
  }
  if(ISBLANK(tname) || (ISBLANK(c1name) && ISBLANK(c3name)) ||
     C->Nobs<=0)
    varterm();
}
