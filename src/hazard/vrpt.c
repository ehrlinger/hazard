#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfskp.h>
#include <hzfxpr.h>
#include "vput.h"

void vrpt(void){
  hzfxpc("Variable",8,58);
  hzfxpc("Min",3,-8);
  hzfxpc("Max",3,-12);
  hzfskp(1);
  hzfxpr('-',59,37);
  hzfskp(1);
  vput("Time:",tname,tmin,H->Tmax,5);
  vput("Event:",c1name,c1min,c1max,6);
  vput("Left Censored:",lcname,lcmin,lcmax,14);
  vput("Right Censored:",c2name,c2min,c2max,15);
  vput("Interval Time:",ctname,ctmin,ctmax,14);
  vput("Interval Censored:",c3name,c3min,c3max,18);
  vput("Weight:",wtname,wtmin,wtmax,7);
  hzfxpr('-',59,37);
  hzfskp(1);
}
