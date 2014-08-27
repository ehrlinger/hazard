#include "hzdinc.h"
#include <string.h>

struct namestr *xvname(char *var){
#ifdef DEBUG
     printf("xvname\n");
#endif /*DEBUG*/

  struct namestr *nsp;
  long i;

  for(i=numvars,nsp=ns; i; i--,nsp++) {
    if(memcmp(var,nsp->nname,VAR_NAME_LENGTH)==0)
      return nsp;
  }
  return NULL;
}
