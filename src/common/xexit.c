#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void xexit(int rc){
  if(rc!=0)
    printf("!%ld\n",rc);
  exit(rc);
}
