/**
 * @file hzd_ln_1min_e_negeX.c
 * @brief Numerically stable computation of \f$\ln(1 - e^{-x})\f$, \f$x > 0\f$.
 *
 * **FORTRAN origin:** `math91/dl1mex`, `math91/dl1mex91` (DL1MEX).
 * Verified December 4, 1989 against the FORTRAN reference.
 *
 * @see hzd_ln_1min_e_negeX.h
 */
#include <string.h>
#include "structures.h"

/*------------------------------------------------------------------*/

double hzd_ln_1min_e_negeX(double lnX){/* for now */
  if(lnX>=-2.25e0 && lnX<=0.834e0)
    return LOGe(ONE-ePOW(-ePOW(lnX)));
  if(lnX<-35.351e0)
    return lnX;
  if(lnX>5.159e0)
    return ZERO;
  return LOGe(ONE-ePOW(-ePOW(lnX)));
}
