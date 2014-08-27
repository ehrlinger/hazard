#include "hazpred.h"
/*
  hzp_calc_intcp_subr
  
  Used by: hzp_calc_intcp_drv.c

  Uses:

  Globals: 
  Common.obs
  Common.p

  Passed: 
  double c,
  double **r,
  short int **s
  
  Returns:
*/
void hzp_calc_intcp_subr(double c,double **r,short int **s){
#ifdef DEBUG
     printf("hzp_cal_intp_subr\n");
#endif /*DEBUG*/

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
