#include "hazpred.h"

#include "stmtopts.h"
#include "notspec.h"
#include "stmtfld.h"

void hazpprc(void){
  if(stmtopts(1))
    H->noCL = TRUE;
  if(stmtopts(2))
    H->Sflag = FALSE;
  if(stmtopts(3))
    H->Hflag = FALSE;
  if(!notspec(4))
    H->CLimit = stmtfld(4);
}
