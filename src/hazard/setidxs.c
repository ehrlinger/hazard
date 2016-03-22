#include <string.h>
#include "hazard.h"
/****************************************************************/
/* SETIDXS called from hazrd2.c:HZ2LOOP, hazrd4.c:HZ4LOOP       */
void setidxs(int i,int j){
  H->index[i] = j;
  H->x[i] = C->theta[j];
  memcpy(H->namex[i],H->names[j],VAR_NAME_LENGTH);
  memcpy(H->lablx[i],H->labl[j],LABEL_LENGTH);
}
