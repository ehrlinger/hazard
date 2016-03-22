#include <string.h>
#include "hazard.h"

#include <stdio.h>

#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzfxpi.h>
#include <hzfxpf.h>
#include <hzf_log1.h>
#include <hzfpag.h>

void obsstat(void){
  int idelet;

  hzfpag(8);
  hzfskp(1);
  hzfxpc("There are",9,7);
  hzfxpi(C->Nobs,5,-1);
  hzfxpc("observations available for analysis with:",41,-1);
  hzfskp(1);
  if(intcen>0) {
    hzfxpf(tally,12,99,20);
    hzfxpc("events:",7,-1);
    hzfskp(1);
    hzfxpf(events,12,99,26);
    hzfxpc("Uncensored",10,-1);
    hzfskp(1);
    hzfxpf(intcen,12,99,26);
    hzfxpc("Interval Censored",17,-1);
    hzfskp(1);
  } else {
    hzfxpf(tally,12,99,20);
    hzfxpc("events",6,-1);
    hzfskp(1);
  }
  if(rtcen>0) {
    hzfxpf(rtcen,12,99,20);
    hzfxpc("Right Censored Observations",27,-1);
    hzfskp(1);
  }
  if(lcens>0) {
    hzfxpi(lcens,5,27);
    hzfxpc("Left Censored Observations",26,-1);
    hzfskp(1);
  }
  hzfxpf(nosubs,12,99,20);
  hzfxpc("Total Subjects",14,-1);
  hzfskp(2);
  if(!(mdelet>0 || delet>mdelet ||
       alldel>0 || delet>0)){
    return;
  }
  if(mdelet>0) {
    hzfxpi(mdelet,5,1);
    hzfxpc("observation(s) found with missing values",40,-1);
    hzfskp(1);
    sprintf(msgbfr,"Note: %ld observation(s) with missing values.",
	    mdelet);
    hzf_log1(msgbfr);
  }
  if(delet>mdelet) {
    idelet = delet-mdelet;
    hzfxpi(idelet,5,1);
    hzfxpc("observation(s) found with inadmissable",38,-1);
    hzfxpc(" values",7,0);
    sprintf(msgbfr,"Note: %ld observation(s) with inadmissible values.",
	    idelet);
    hzf_log1(msgbfr);
  }
  if(alldel>0) {
    hzfxpc(" -- ",0,52);
    hzfxpi(alldel,5,-1);
    hzfxpc("of which have no count of events or",35,-1);
    hzfxpc(" censored observations",22,0);
    hzfskp(1);
    sprintf(msgbfr,"      (%ld of which have no count of events or "
	    "censored observations.)",alldel);
    hzf_log1(msgbfr);
  } else if(delet>mdelet)
    hzfskp(1);

  if(delet>0) {
    hzfxpi(delet,5,1);
    hzfxpc("total observations deleted",26,-1);
    hzfskp(1);
    sprintf(msgbfr,"Note: %ld total observation(s) deleted.",delet);
    hzf_log1(msgbfr);
  }
  hzfskp(2);
}
