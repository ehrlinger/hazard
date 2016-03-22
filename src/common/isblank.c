#include <string.h>
#include "hzdinc.h"

logical isblanck(char *strg,int leng){
  for(; leng; leng--,strg++)
    if(*strg!=' ')
      return FALSE;
  return TRUE;
}
