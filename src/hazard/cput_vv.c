#include "hazard.h"

void cput_vv(char *phasnm,long *iary,void (*cputfn)(char *,long)){
  logical lbld;
  long j,*iptr;

  lbld = FALSE;
  for(j=0,iptr=iary; j<C->p; j++,iptr++){
    if(*iptr==1){
      if(lbld){
	(*cputfn)("         ",j);
      }else{
	(*cputfn)(phasnm,j);
	lbld = TRUE;
      }
    }
  }
}
