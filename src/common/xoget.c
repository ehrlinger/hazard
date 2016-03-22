#include <string.h>
#include <stdio.h>
#include "hzdinc.h"

void xoget(void){
  fread(xvobsbfr,(size_t)xvobslen,1,inputDataFile);
}
