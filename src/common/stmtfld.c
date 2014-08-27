#include "hzdinc.h"

double stmtfld(long fldno){
#ifdef DEBUG
     printf("stmtfld\n");
#endif /*DEBUG*/

  return stmtptr->parm[fldno-1].num;
}
