#include <stdio.h>
#include "hzdinc.h"
#include "hzf_col.h"

void hzfxpi(long value,long places,long column){
#ifdef DEBUG
     printf("hzfxpi\n");
#endif /*DEBUG*/

  if(!curr_xpmode) return;
  hzf_col(column);
  printf("%*ld\n",(int)places,value);
}
