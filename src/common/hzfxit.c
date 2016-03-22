#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "xexit.h"

void hzfxit(char *type){
  if(curr_xlmode)
    printf("$Note: EXITING DUE TO %s ERROR\n",type);
  xexit(16);
}
