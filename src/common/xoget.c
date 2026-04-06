#include <string.h>
#include <stdio.h>
#include "hzdinc.h"

void xoget(void){
  if(fread(xvobsbfr,(size_t)xvobslen,1,inputDataFile)!=1) {
    /* Preserve historical behavior: callers handle downstream state. */
  }
}
