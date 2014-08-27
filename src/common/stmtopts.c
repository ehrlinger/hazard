#define NDEBUG
#include "structures.h"
#include "hzdinc.h"

logical stmtopts(long optno){
#ifndef NDEBUG
     printf("stmtops\n");
#endif /*NDEBUG*/

  if(stmtptr->opts[optno-1]=='Y')
    return TRUE;
  else
    return FALSE;
}
