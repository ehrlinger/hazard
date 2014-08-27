#include "common.h"
#include <string.h>
logical notspec(long fldno){
#ifdef DEBUG
     printf("notspec\n");
#endif /*DEBUG*/

  if(ISMISS(stmtptr->parm[fldno-1].num))
    return TRUE;
  else
    return FALSE;
}
