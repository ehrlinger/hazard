#include <stdio.h>
#include "hzdinc.h"

void xoget(void){
#ifdef DEBUG
     printf("xoget\n");
#endif /*DEBUG*/

  fread(xvobsbfr,(size_t)xvobslen,1,inputDataFile);
}
