/**
 * @file hzd_ln_A_div_B.c
 * @brief Compute \f$\ln(A/B)\f$ with domain error gating.
 *
 * Direct evaluation without the Mercator series approximation used by
 * dlgaob(). Appropriate for ratios not near 1.0 or when the additional
 * series accuracy is not needed.
 *
 * **FORTRAN origin:** `math91/dlgaob` (simplified C wrapper).
 *
 * @see hzd_ln_A_div_B.h
 * @see dlgaob.c for the high-accuracy series variant.
 */
#include <string.h>
#include "structures.h"

double hzd_ln_A_div_B(double A,double B){
  double ratio;

  if(A<=ZERO || B<=ZERO) ErrorReturn(2,ZERO);

  ratio = A/B;
  if(ratio<=ZERO) ErrorReturn(2,ZERO);

  if(ratio<=Machn.absMin || ratio>=Machn.absMax)
    ErrorReturn(1,ZERO);

  return LOGe(ratio);
}
