#include <string.h>
#include "hzdinc.h"

void hzfmis(void *var){
  memcpy(var,MISSING,sizeof(double));
}
