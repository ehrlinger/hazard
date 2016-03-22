#include <string.h>
#include "hazard.h"
#include "finprmx.h"
/****************************************************************/
/* THETA_X called from hazrd2.c:HZ2LOOP, hazrd3.c:HAZRD3        */
void theta_x(void){
  int i,j;

  i = 0;
  if(C->phase[1]==1)
    for(j=0; j<4; j++)
      if(C->status[j]==1)
	H->x[i++] = C->theta[j];
  if(C->phase[3]==1)
    for(j=4; j<8; j++)
      if(C->status[j]==1)
	H->x[i++] = C->theta[j];
  finprmx(&i,1,8,C->p+8);
  finprmx(&i,2,C->p+9,2*C->p+9);
  finprmx(&i,3,2*C->p+10,3*C->p+10);
}
