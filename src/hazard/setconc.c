#include <hzdinc.h>
#include "hazard.h"
#include <setstat.h>

/****************************************************************/
/* SETCONC called from main                                     */
void setconc(void){
  long i,j;

  H->phase1 = 0;
  H->phase2 = 0;
  H->phase3 = 0;
  iexc = 0;
  icxc = 0;
  ilxc = 0;
  if(C->p==0)
    return;
  for(i=0; i<k; i++) {
    for(j=0; j<C->p && memcmp(risk[j],varnam[i],VAR_NAME_LENGTH); j++);
    if(memcmp(faze[i],"EARLY   ",8)==0)
      setstat(i,j+9,&H->phase1,&iearly[j],&iexc);
    else if(memcmp(faze[i],"CONSTANT",8)==0)
      setstat(i,j+10+C->p,&H->phase2,&iconst[j],&icxc);
    else
      setstat(i,j+11+2*C->p,&H->phase3,&ilate[j],&ilxc);
  }
}
