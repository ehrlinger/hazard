#include <stdio.h>
#include "hzdinc.h"
#include "hzf_col.h"

void hzfxpr(char byte,long times,long column){
#ifdef DEBUG
     printf("hzfxpr\n");
#endif /*DEBUG*/

  long i;

  if(!curr_xpmode) return;
  hzf_col(column);
  for(i=times; i; i--)
    printf("%c",byte);
  printf("\n");
}
