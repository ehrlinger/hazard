#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "hzf_put.h"

void hzflg1(char *strg,int leng){
  if(!curr_xlmode) return;
  if(*strg!=' ')
    last_xltype = *strg;
  if(curr_xlmode==1 && last_xltype=='N')
    return;
  printf("$");
  hzf_put(leng,strg);
  printf("\n");
}
