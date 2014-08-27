#include "hzdinc.h"

void xpmode(long setting){
#ifdef DEBUG
     printf("xpmode\n");
#endif /*DEBUG*/

  curr_xpmode = setting;
}
