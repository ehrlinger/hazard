#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "hzdinc.h"

void *hzf_memget(int leng){
  void *stg;
  size_t leng_t;

  if(leng>=0) {
    leng_t = (size_t)leng;
    stg = malloc(leng_t);
  } else {
    if(curr_xlmode)
      printf("$ERROR: Attempted memory allocation of %d "
	     "exceeds size limit.\n",leng);
    stg = NULL;
  }
  if(stg==NULL) {
    if(leng>0)
      hzf_mem_need += leng;
    else
      hzf_mem_need = INT_MAX;
  }
  else
    memset(stg,0,leng_t);
  return stg;
}
