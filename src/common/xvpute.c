#include "hzdinc.h"

void xvpute(void){
#ifdef DEBUG
     printf("xvpute\n");
#endif /*DEBUG*/

  long gap;

  gap = 80-((xvputcnt*140)%80);
  if(gap!=80)
    for(; gap; gap--)
      fprintf(outputDataFile," ");
  fprintf(outputDataFile,"HEADER RECORD*******");
  fprintf(outputDataFile,"OBS     ");
  fprintf(outputDataFile,"HEADER RECORD!!!!!!!000000");
  fprintf(outputDataFile,"000000000000000000000000  ");
}
