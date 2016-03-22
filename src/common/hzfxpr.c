#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "hzf_col.h"

void hzfxpr(char byte,int times,int column){
  int i;

  if(!curr_xpmode) return;
  hzf_col(column);
  for(i=times; i; i--)
    printf("%c",byte);
  printf("\n");
}
