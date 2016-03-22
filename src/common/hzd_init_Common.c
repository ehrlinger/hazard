#include <string.h>
#include "structures.h"
#include <stdlib.h>

void hzd_init_Common(void){
  Common.version = &Version;
  Common.early = &Early;
  Common.constant = &Constant;
  Common.late = &Late;
  Common.machn = &Machn;
  Common.intfc = &Intfc;
  Common.errorno = 0;
  if(setjmp(Common.errtrap))
    abort();
  Version.hzdc_ver = HZDVER;
  Intfc.malloc = malloc;
}
