#include <string.h>
#include <stdio.h>
#include "hzdinc.h"
#include "hzf_put.h"

void hzflg2(char *strg1,int leng1,char *strg2,int leng2){
  if(!curr_xlmode) return;
  if(*strg1!=' ')
    last_xltype = *strg1;
  if(curr_xlmode==1 && last_xltype=='N')
    return;
  printf("$");
  hzf_put(leng1,strg1);
  hzf_put(leng2,strg2);
  printf("\n");
}
