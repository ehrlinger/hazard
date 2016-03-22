#include <string.h>
#include <strings.h>
#include "structures.h"

void hzr_set_parm_err(char *location,short int parmno){
  Common.errorno = 1;
  strncpy(Common.errflg,location,12);
  memset(Common.errflg+(parmno*3+12),'*',3);
}
