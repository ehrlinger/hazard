#include "hazpred.h"
/*
  hzp_calc_XsX - 
  Used by: hzpo

  Globals: Common.cov
  Common.Ntheta
  p1
  p2
  
  Passed: V

  Returns: sum
*/
double hzp_calc_XsX(double *V){
#ifdef DEBUG
     printf("hzp_calc_XsX\n");
#endif /*DEBUG*/
  double sum,sub;

  p1 = Common.cov;
  sum = ZERO;
  for(i=0; i<Common.Ntheta; i++)
    if(V[i]!=ZERO) {
      for(j=i+1,p2=V,sub=ZERO; j; j--,p1++,p2++)
	sub += (*p1)*(*p2);
      sum += sub*V[i];
    } else
      p1 += (i+1);
  return sum;
}
