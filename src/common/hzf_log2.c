#include "hzflg2.h"
#include <string.h>

void hzf_log2(char *strg1,char *strg2){
#ifdef DEBUG
     printf("hzf_log2\n");
#endif /*DEBUG*/

  long leng1,leng2;

  leng1 = strlen(strg1);
  leng2 = strlen(strg2);
  hzflg2(strg1,leng1,strg2,leng2);
}
