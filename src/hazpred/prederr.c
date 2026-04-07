#include <string.h>
#include <stdio.h>
#include "hazpred.h"
#include "hzf_log1.h"
#include "hzfxit.h"

void prederr(void){
  if(C->errorno==1) {
    snprintf(msgbfr,sizeof(msgbfr),
	     "ERROR: Computation has exceeded limit %12.12s.",C->errflg);
    hzf_log1(msgbfr);
  } else if(C->errorno==2) {
    snprintf(msgbfr,sizeof(msgbfr),
	     "ERROR: Internal sign violation, possibly data dependent %12.12s.\n",
	     C->errflg);
    hzf_log1(msgbfr);
  }
  hzf_log1("ERROR: Procedure terminated due to error in model "
	   "specification computation.");
  hzfxit("SEMANTIC");
}

