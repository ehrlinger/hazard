#include "hzflg1.h"
#include <string.h>

void hzf_log1(char *strg){
#ifdef DEBUG
     printf("hzf_log1\n");
#endif /*DEBUG*/

  long leng;

  leng = strlen(strg);
  hzflg1(strg,leng);
}
