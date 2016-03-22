#include <string.h>
#include "common.h"

logical notspec(int fldno){
  if(ISMISS(stmtptr->parm[fldno-1].num))
    return TRUE;
  else
    return FALSE;
}
