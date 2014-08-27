#include "structures.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>

void hzd_init_Common(void){
#ifdef DEBUG
     printf("hzd_init_Common\n");
#endif /*DEBUG*/

  /*
    This initializes a large Common block,
    with lots of smaller common areas.
  */
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
