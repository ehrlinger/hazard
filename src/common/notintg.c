#include <math.h>
#include "hzdinc.h"

logical notintg(double value){
#ifdef DEBUG
     printf("notintg\n");
#endif /*DEBUG*/

  if(floor(value)==value)
    return FALSE;
  else
    return TRUE;
}
