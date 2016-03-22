#include <string.h>
/*------------------------------------------------------------------*/
#include "structures.h"

#include <strings.h>

void hzd_Error(char *msgtext,int errnum){
  strcpy(Common.errflg,"(");
  strcat(Common.errflg,msgtext);
  strcat(Common.errflg,")");
  if(errnum)
    Common.errorno = errnum;
  longjmp(Common.errtrap,(int)Common.errorno);
}
