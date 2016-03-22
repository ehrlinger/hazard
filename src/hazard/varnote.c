#include <string.h>
#include "hazard.h"
#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzflg2.h>
#include <hzfxpi.h>

void varnote(void){
  hzfskp(1);
  hzfxpc("*------*",8,4);
  hzfskp(1);
  hzfxpc("| Note |",8,4);
  hzfskp(1);
  hzfxpc("*------*",8,4);
  hzfskp(2);
  if(c1flag==1) {
    hzfxpc("The Event variable contains non-integer",39,7);
    hzfxpc(" values",7,0);
    hzfskp(1);
    hzflg2("Note: Event variable contains ",30,
	   "non-integer values.",19);
  }
  if(c2flag==1) {
    hzfxpc("The Right Censored variable contains",36,7);
    hzfxpc(" non-integer values",19,0);
    hzfskp(1);
    hzflg2("Note: Right Censored variable contains ",39,
	   "non-integer values.",19);
  }
  if(c3flag==1) {
    hzfxpc("The Interval Censored variable contains",39,7);
    hzfxpc(" non-integer values",19,0);
    hzfskp(1);
    hzflg2("Note: Interval Censored variable contains ",42,
	   "non-integer values.",19);
  }
  if(c2chg>0) {
    hzfxpc("in",2,7);
    hzfxpi(c2chg,4,-1);
    hzfxpc("observation(s) RCENSOR has been changed",39,-1);
    hzfxpc(" from 0 to 1",12,0);
    hzfskp(1);
    hzflg2("Note: RCENSOR has been changed from 0 to 1 ",43,
	   "in one or more observations.",28);
  }
  if(ivent>0) {
    hzfxpi(ivent,4,7);
    hzfxpc("Interval Censored times have equaled",36,-1);
    hzfxpc(" Time. These have been added to Events.",39,0);
    hzfskp(1);
    hzflg2("Note: Some Interval Censored times equaled ",43,
	   "Time, and have been added to Events.",36);
  }
}
