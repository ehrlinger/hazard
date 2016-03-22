#include <string.h>
#include <strings.h>
#include "structures.h"

void hzr_set_error(char *location,int errorset){
  Common.errorno = errorset;
  strncpy(Common.errflg,location,12);
}
