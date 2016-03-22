#include <string.h>
#include "hzdinc.h"

struct namestr *xvname(char *var){
  struct namestr *nsp;
  int i;

  for(i=numvars,nsp=ns; i; i--,nsp++) {
    if(memcmp(var,nsp->nname,VAR_NAME_LENGTH)==0)
      return nsp;
  }
  return NULL;
}
