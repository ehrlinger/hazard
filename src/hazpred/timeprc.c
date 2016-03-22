#include <string.h>
#include "common.h"
#include "notspec.h"
#include "hzf_log1.h"
#include "hazpred.h"
#include "xvname.h"
#include "stmtfldname.h"

void timeprc(void){
  if(notspec(11)) {
    hzf_log1("ERROR: TIME variable not specified.");
    termin = TRUE;
    return;
  }
  ns = data_ns;
  numvars = infilect;
  T_ns = xvname(stmtfldname(11));
  if(T_ns==NULL) {
    hzf_log1("ERROR: TIME variable not found in input data set.");
    termin = TRUE;
  } else if(T_ns->ntype!=1) {
    hzf_log1("ERROR: TIME variable is not numeric.");
    termin = TRUE;
  } else
    memcpy(tname,T_ns->nname,VAR_NAME_LENGTH);
}
