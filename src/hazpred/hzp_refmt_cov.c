#include <string.h>
#include "hazpred.h"

void hzp_refmt_cov(void){
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
