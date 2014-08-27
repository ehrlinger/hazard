#include <stdio.h>
#include "hazpred.h"
#include "hzf_log1.h"
#include "hzfxit.h"

void hazverr(long varnum){
  sprintf(msgbfr,"ERROR: INHAZ data set variable %ld is incorrect.",varnum);
  hzf_log1(msgbfr);
  hzfxit("DATA");
}
