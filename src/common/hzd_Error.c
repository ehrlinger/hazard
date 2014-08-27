/*------------------------------------------------------------------*/
#include "structures.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>

void hzd_Error(char *msgtext,long errnum){
#ifdef DEBUG
  printf("hzd_Error\n");
#endif /*DEBUG*/

  strcpy(Common.errflg,"(");
  strcat(Common.errflg,msgtext);
  strcat(Common.errflg,")");
  if(errnum)
      Common.errorno = errnum;
  longjmp(Common.errtrap,(int)Common.errorno);
}
