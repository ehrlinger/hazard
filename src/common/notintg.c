#include <string.h>
#include <math.h>
#include "hzdinc.h"

logical notintg(double value){
  if(floor(value)==value)
    return FALSE;
  else
    return TRUE;
}
