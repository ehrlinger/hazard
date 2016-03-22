#include <string.h>
#include "hazard.h"

void cput_vv(char *phasnm,int *iary,void (*cputfn)(char *,int)){
  logical lbld;
  int j,*iptr;

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
