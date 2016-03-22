#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hzdinc.h"

void *hzf_memget(int leng){
  void *stg;
  size_t leng_t;

  leng_t = (size_t)leng;
  if(leng_t==leng)
    stg = malloc(leng_t);
  else {
    if(curr_xlmode)
      printf("$ERROR: Attempted memory allocation of %ld "
	     "exceeds size limit.\n",leng);
    stg = NULL;
  }
  if(stg==NULL)
    hzf_mem_need += leng;
  else
    memset(stg,0,(size_t)leng);
  return stg;
}
