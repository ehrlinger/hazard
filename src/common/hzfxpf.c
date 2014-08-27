#include <stdio.h>
#include <math.h>
#include <string.h>

#include "common.h"
#include "hzdinc.h"

#include "hzf_col.h"

void hzfxpf(double value,long places,long precsn,long column){
#ifdef DEBUG
     printf("hzfzpf\n");
#endif /*DEBUG*/

  double prtval;
  long i;

  if(!curr_xpmode) return;
  hzf_col(column);
  prtval = value;
  if(ISMISS(prtval)) {
    printf("%*.*s\n",(int)places,(int)places,".");
    return;
  }
  if(precsn!=99) {
    for(i=precsn; i; i--)
      prtval = prtval*10.0;
    prtval = floor(prtval+0.5e0);
    for(i=precsn; i; i--)
      prtval = prtval/10.0e0;
    printf("%*.*f\n",(int)places,(int)precsn,prtval);
  } else if(prtval<ZERO)
    printf("%*.6G\n",(int)places,prtval);
  else
    printf("%*.7G\n",(int)places,prtval);
}
