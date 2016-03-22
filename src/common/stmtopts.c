#include <string.h>
#include "structures.h"
#include "hzdinc.h"

logical stmtopts(int optno){
  if(stmtptr->opts[optno-1]=='Y')
    return TRUE;
  else
    return FALSE;
}
