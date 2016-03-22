#include <string.h>
#include "hazpred.h"

void hzp_calc_intcp_subr(double c,double **r,short int **s){
  double *v,*o;
  short int *f;

  v = *r;
  f = *s;
  *v++ = c;
  o = Common.obs+1;
  for(i=Common.p,f++; i; i--,v++,o++)
    if((*f++)!=0)
      *v = (*o)*c;
  *r = v;
  *s = f;
}
