#include "hzdinc.h"

logical xMISSgot(void *value,int len){
#ifdef DEBUG
     printf("xMISSgot\n");
#endif /*DEBUG*/

  char *bfr;

  bfr = value;
  if(*bfr=='\0')
    return FALSE;
  for(len--; len; len--)
    if(*++bfr!='\0')
      return FALSE;
  return TRUE;	
}
