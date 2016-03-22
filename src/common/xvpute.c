#include <string.h>
#include "hzdinc.h"

void xvpute(void){
  int gap;

  gap = 80-((xvputcnt*140)%80);
  if(gap!=80)
    for(; gap; gap--)
      fprintf(outputDataFile," ");
  fprintf(outputDataFile,"HEADER RECORD*******");
  fprintf(outputDataFile,"OBS     ");
  fprintf(outputDataFile,"HEADER RECORD!!!!!!!000000");
  fprintf(outputDataFile,"000000000000000000000000  ");
}
