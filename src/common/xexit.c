#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void xexit(int rc){
  if(rc!=0)
    printf("!%d\n",rc);
  exit(rc);
}
