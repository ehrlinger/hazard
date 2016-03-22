#include <string.h>
#include <stdio.h>
#include "hzdinc.h"

void hzfskp(int count){
  if(!curr_xpmode) return;
  printf("/%ld\n",count);
}
