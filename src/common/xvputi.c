#include <string.h>
#include <stdio.h>
#include "hzdinc.h"

void xvputi(int nvar){
  fprintf(outputDataFile,"HEADER RECORD*******");
  fprintf(outputDataFile,"NAMESTR HEADER RECORD!!!!!!!000000");
  fprintf(outputDataFile,"%04ld",nvar);
  fprintf(outputDataFile,"00000000000000000000  ");
}
