#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>

void xexit(long rc){
#ifdef DEBUG
     printf("xexit\n");
#endif /*DEBUG*/

  if(rc!=0)
    printf("!%ld\n",rc);
  exit(rc);
}
