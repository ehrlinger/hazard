#include "hzdinc.h"
#include <string.h>

void hzfmis(void *var){
#ifdef DEBUG
     printf("hzfmis\n");
#endif /*DEBUG*/

  memcpy(var,MISSING,sizeof(double));
}
