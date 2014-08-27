#include <stdio.h>
#include "hzdinc.h"

void hzfskp(long count){
#ifdef DEBUG
     printf("hzfskp\n");
#endif /*DEBUG*/

  if(!curr_xpmode) return;
  printf("/%ld\n",count);
}
