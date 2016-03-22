#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfskp.h>
#include <hzfxpr.h>
#include "vput_d.h"

void vrpt_d(void){
  hzfxpc("Deleted Observations",20,63);
  hzfskp(1);
  hzfxpc("--------------------",20,63);
  hzfskp(1);
  hzfxpc("Missing   Inadmissable",22,62);
  hzfskp(1);
  hzfxpc("Variable",8,47);
  hzfxpc("Data",4,-7);
  hzfxpc("Values",6,-6);
  hzfxpc("Min",3,-12);
  hzfxpc("Max",3,-12);
  hzfskp(1);
  hzfxpr('-',87,26);
  hzfskp(1);
  vput_d("Time:",tname,mtdel,tdel,tmin,H->Tmax,5);
  vput_d("Event:",c1name,mc1del,c1del,c1min,c1max,6);
  vput_d("Left Censored:",lcname,mlcdel,lcdel,lcmin,lcmax,14);
  vput_d("Right Censored:",c2name,mc2del,c2del,c2min,c2max,15);
  vput_d("Interval Time:",ctname,mctdel,ctdel,ctmin,ctmax,14);
  vput_d("Interval Censored:",c3name,mc3del,c3del,c3min,c3max,18);
  vput_d("Weight:",wtname,mwtdel,wtdel,wtmin,wtmax,7);
  hzfxpr('-',87,26);
  hzfskp(1);
}
