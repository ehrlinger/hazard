#include "hazard.h"
#include <hzfxpc.h>
#include <hzfskp.h>
#include <hzfxpr.h>
#include "vput_dl.h"

void vrpt_dl(void){
  hzfxpc("Deleted Observations",20,81);
  hzfskp(1);
  hzfxpc("----------------------",22,80);
  hzfskp(1);
  hzfxpc("Missing   Inadmissable",22,80);
  hzfskp(1);
  hzfxpc("Variable   Label",16,26);
  hzfxpc("Data",4,-38);
  hzfxpc("Values",6,-6);
  hzfxpc("Min",3,-13);
  hzfxpc("Max",3,-11);
  hzfskp(1);
  hzfxpr('-',126,5);
  hzfskp(1);
  vput_dl("Time:",tname,tlabl,mtdel,tdel,tmin,H->Tmax,5);
  vput_dl("Event:",c1name,c1labl,mc1del,c1del,c1min,c1max,6);
  vput_dl("Left Censored:",lcname,lclabl,mlcdel,lcdel,lcmin,lcmax,14);
  vput_dl("Right Censored:",c2name,c2labl,mc2del,c2del,c2min,c2max,15);
  vput_dl("Interval Time:",ctname,ctlabl,mctdel,ctdel,ctmin,ctmax,14);
  vput_dl("Interval Censored:",c3name,c3labl,mc3del,c3del,c3min,c3max,18);
  vput_dl("Weight:",wtname,wtlabl,mwtdel,wtdel,wtmin,wtmax,7);
  hzfxpr('-',126,5);
  hzfskp(1);
}
