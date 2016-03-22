#include <string.h>
#include "hzflg1.h"

void hzf_log1(char *strg){
  int leng;

  leng = strlen(strg);
  hzflg1(strg,leng);
}
