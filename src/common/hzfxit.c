#include <stdio.h>
#include "hzdinc.h"
#include "xexit.h"

void hzfxit(char *type){
#ifdef DEBUG
     printf("hzfxit\n");
#endif /*DEBUG*/

  if(curr_xlmode)
    printf("$Note: EXITING DUE TO %s ERROR\n",type);
  xexit(16);
}
