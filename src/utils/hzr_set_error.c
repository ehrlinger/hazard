#include <string.h>
#include "structures.h"

void hzr_set_error(char *location,long errorset){
  Common.errorno = errorset;
  strncpy(Common.errflg,location,12);
}
