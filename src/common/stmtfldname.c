#include "hzdinc.h"

char *stmtfldname(long fldno){
#ifdef DEBUG
     printf("stntfldname\n");
#endif /*DEBUG*/

  return stmtptr->parm[fldno-1].var;
}
