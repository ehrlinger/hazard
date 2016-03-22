#include <string.h>
#include "hzdinc.h"

char *stmtfldname(int fldno){
  return stmtptr->parm[fldno-1].var;
}
