#include <stdio.h>
#include "hazpred.h"
#include "hzf_log1.h"
#include "hzfxit.h"

void obsverr(long obsnum){
  sprintf(msgbfr,"ERROR: INHAZ data set observation %ld is incorrect.",
	  obsnum);
  hzf_log1(msgbfr);
  hzfxit("DATA");
}

