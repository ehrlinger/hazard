#include <string.h>
#include "hazard.h"

#include <hzfxpc.h>
#include <hzfskp.h>
#include <hzfxpr.h>
#include "vput_l.h"

void vrpt_l(void){
  hzfxpc("Variable   Label",16,38);
  hzfxpc("Min",3,-42);
  hzfxpc("Max",3,-12);
  hzfskp(1);
  hzfxpr('-',101,17);
  hzfskp(1);
  vput_l("Time:",tname,tlabl,tmin,H->Tmax,5);
  vput_l("Event:",c1name,c1labl,c1min,c1max,6);
  vput_l("Left Censored:",lcname,lclabl,lcmin,lcmax,14);
  vput_l("Right Censored:",c2name,c2labl,c2min,c2max,15);
  vput_l("Interval Time:",ctname,ctlabl,ctmin,ctmax,14);
  vput_l("Interval Censored:",c3name,c3labl,c3min,c3max,18);
  vput_l("Weight:",wtname,wtlabl,wtmin,wtmax,7);
  hzfxpr('-',101,17);
  hzfskp(1);
}
