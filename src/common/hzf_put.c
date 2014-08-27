#include <stdio.h>

void hzf_put(long leng,char *strg){
#ifdef DEBUG
     printf("hzf_put\n");
#endif /*DEBUG*/

  long i;

  for(i=leng; i>0 && *strg; i--)
    putchar(*strg++);
  for(; i>0; i--)
    putchar(' ');
}
