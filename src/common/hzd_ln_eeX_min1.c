#include "structures.h"

/*------------------------------------------------------------------*/

double hzd_ln_eeX_min1(double lnX){   /* for now */
#ifdef DEBUG
     printf("hzd_ln_eeX_min1\n");
#endif /*DEBUG*/

  ErrorReset;
  if(lnX>-2.35e0 && lnX<4.31e0)
    return LOGe(ePOW(ePOW(lnX))-ONE);
  if(lnX<=-174.0e0)
    return LnLim.absMin;
  if(lnX>=174.0e0) 
    ErrorReturn(1,ZERO);
  if(lnX>=4.31e0)
    return ePOW(lnX);
  return LOGe(ePOW(ePOW(lnX))-ONE);
}
