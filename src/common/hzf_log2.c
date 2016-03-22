#include <string.h>
#include "hzflg2.h"

void hzf_log2(char *strg1,char *strg2){
  int leng1,leng2;

  leng1 = strlen(strg1);
  leng2 = strlen(strg2);
  hzflg2(strg1,leng1,strg2,leng2);
}
