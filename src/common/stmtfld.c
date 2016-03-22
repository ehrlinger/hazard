#include <string.h>
#include "hzdinc.h"

double stmtfld(int fldno){
  return stmtptr->parm[fldno-1].num;
}
