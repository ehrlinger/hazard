#include <string.h>
#include <stdio.h>
#include "hazpred.h"
#include "hzf_log1.h"
#include "hzfxit.h"

void obsverr(int obsnum){
  snprintf(msgbfr,sizeof(msgbfr),
	   "ERROR: INHAZ data set observation %d is incorrect.",obsnum);
  hzf_log1(msgbfr);
  hzfxit("DATA");
}

