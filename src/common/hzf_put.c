#include <string.h>
#include <stdio.h>

void hzf_put(int leng,char *strg){
  int i;

  for(i=leng; i>0 && *strg; i--)
    putchar(*strg++);
  for(; i>0; i--)
    putchar(' ');
}
