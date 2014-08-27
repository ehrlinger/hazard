
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "hazpred.h"
/*
  hzp_refmt_cov

  Used By: hzp_init_CL

  Uses: 
  
  Globals: 
  p1
  p2
  Common.cov
  Common.Ntheta
  

  Passed: 

  Returns: 



*/
void hzp_refmt_cov(void){
#ifdef DEBUG
     printf("hzp_refmt_cov\n");
#endif /*DEBUG*/
  p1 = p2 = Common.cov;
  for(i=0; i<Common.Ntheta; i++) {
    for(j=i; j; j--,p1++)
      if((*p1)==ZERO)
	*p2++ = ZERO;
      else
	*p2++ = (*p1)+(*p1);
    if((*p1)==ZERO)
      *p2++ = ZERO;
    else
      *p2++ = *p1;
    p1 += (Common.Ntheta-i);
  }
}
