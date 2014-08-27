#include <stdio.h>
#include "hzdinc.h"

void hzfpag(long count){
#ifdef DEBUG
     printf("hzfpag\n");
#endif /*DEBUG*/

  if(!curr_xpmode) return;
  printf("<%ld\n",count);
}
