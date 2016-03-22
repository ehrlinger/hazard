#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "hzf_col.h"

void hzfxpi(int value,int places,long column){
  if(!curr_xpmode) return;
  hzf_col(column);
  printf("%*ld\n",(int)places,value);
}
