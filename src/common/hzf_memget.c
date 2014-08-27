#include <string.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "hzdinc.h"
#include <stdlib.h>
void *hzf_memget(long leng){
#ifdef DEBUG
     printf("hzf_memget\n");
#endif /*DEBUG*/

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
