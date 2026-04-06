#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "hzf_col.h"

void hzfxpi(int value,int places,long column){
  if(!curr_xpmode) return;
  hzf_col(column);
  /* 'value' is int throughout the call chain; keep format consistent. */
  printf("%*d\n",(int)places,value);
}
