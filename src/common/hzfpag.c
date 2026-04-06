#include <string.h>
#include <stdio.h>
#include "hzdinc.h"

void hzfpag(int count){
  if(!curr_xpmode) return;
  printf("<%d\n",count);
}
