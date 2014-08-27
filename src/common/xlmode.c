#include "hzdinc.h"

void xlmode(long setting){
#ifdef DEBUG
     printf("xlmode\n");
#endif /*DEBUG*/

  curr_xlmode = setting;
}
