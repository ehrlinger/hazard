#include <string.h>
#include <stdio.h>
#include "hazpred.h"
#include "hzf_log1.h"
#include "hzfxit.h"

void fmtverr(int varnum,char *name,char *namv){
  sprintf(msgbfr,"ERROR: INHAZ data set is not formatted correctly "
	  "(%ld,%s,%s).\n",varnum+1,name,namv);
  hzf_log1(msgbfr);
  hzfxit("DATA");
}

