#include <string.h>
#include "hazpred.h"

double hzp_calc_XsX(double *V){
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
