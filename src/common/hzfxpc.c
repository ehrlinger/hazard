#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "hzf_col.h"
#include "hzf_put.h"

void hzfxpc(char *strg,int leng,int column){
  if(!curr_xpmode) return;
  if(!leng)
    leng = strlen(strg);
  hzf_col(column);
  hzf_put(leng,strg);
  printf("\n");
}

