#include <stdio.h>
#include <string.h>
#include "hzdinc.h"
#include "hzf_col.h"
#include "hzf_put.h"

void hzfxpc(char *strg,long leng,long column){
#ifdef DEBUG
     printf("hzfxpc\n");
#endif /*DEBUG*/

  if(!curr_xpmode) return;
  if(!leng)
    leng = strlen(strg);
  hzf_col(column);
  hzf_put(leng,strg);
  printf("\n");
}

