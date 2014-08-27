#include "structures.h"

/*------------------------------------------------------------------*/

double hzd_ln_1min_e_negeX(double lnX){
#ifdef DEBUG
     printf("hzd_ln_1min_e_negeX\n");
#endif /*DEBUG*/

/* for now */
  if(lnX>=-2.25e0 && lnX<=0.834e0)
    return LOGe(ONE-ePOW(-ePOW(lnX)));
  if(lnX<-35.351e0)
    return lnX;
  if(lnX>5.159e0)
    return ZERO;
  return LOGe(ONE-ePOW(-ePOW(lnX)));
}
