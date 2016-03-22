#include <string.h>
#include "common.h"
#include "notspec.h"
#include "xvname.h"
#include "stmtfldname.h"
#include "hzf_log1.h"

#include "hazpred.h"

void idproc(void){
  if(notspec(9))
    return;
  ID_ns = xvname(stmtfldname(9));
  if(ID_ns==NULL) {
    hzf_log1("ERROR: ID variable not found in input data set.");
    termin = TRUE;
  } else if(ID_ns==T_ns) {
    hzf_log1("ERROR: ID and TIME variable cannot be the same.");
    termin = TRUE;
  } else
    gotID = TRUE;
}
