/**
 * @file hzd_ln_G3_and_SG3.c
 * @brief Late-phase shaping function G3 and hazard shaping function SG3.
 *
 * Implements four cases of the late-phase cumulative hazard shaping function
 * \f$G_3(t)\f$ and its time derivative \f$SG_3(t)\f$.
 *
 * The general 4-parameter form:
 * \f[
 *   G_3(t) = \left[\left(1 + \left(\frac{t}{\tau}\right)^\gamma\right)^{1/\alpha} - 1\right]^\eta
 * \f]
 *
 * \f[
 *   SG_3(t) = G_3(t)^{1-1/\eta} \cdot
 *     \left(1+\left(\frac{t}{\tau}\right)^\gamma\right)^{(1-\alpha)/\alpha} \cdot
 *     \left(\frac{t}{\tau}\right)^{\gamma-1} \cdot \frac{\gamma\eta}{\tau\alpha}
 * \f]
 *
 * When \f$\alpha \to 0\f$ (g3flag=2 or 4):
 * \f[
 *   G_3(t) = \left[e^{(t/\tau)^\gamma} - 1\right]^\eta
 * \f]
 *
 * **FORTRAN origin:** `model91/dlg3` (DLG3 subroutine).
 * **TemporalHazard R equivalent:** `hzd_G3()` in the R package.
 *
 * @see hzd_ln_G3_and_SG3.h
 * @see setg3() for g3flag assignment.
 */
#include <string.h>
#include "structures.h"
#include "hzd_Error.h"
#include "hzd_ln_A_div_B.h"
#include "hzd_ln_eX_plus1.h"
#include "hzd_ln_eX_min1.h"
#include "hzd_ln_eeX_min1.h"

void hzd_ln_G3_and_SG3(void){
  double lnTTau,lnTTauG,lnScale,tGamma,tEta;

  HazG3.lnG3 = ZERO;
  HazG3.lnSG3 = ZERO;

  if(HazG3.T<=ZERO)
    hzd_Error("DLG3900",2);

  if(Late.g3flag<1 || Late.g3flag>4)
    hzd_Error("DLG3910",2);

  lnTTau = hzd_ln_A_div_B(HazG3.T,Late.tau);
  lnTTauG = lnTTau*Late.gamma;

  lnScale = LOGe(Late.gamma)+LOGe(Late.eta)-LOGe(Late.tau);
  if(Late.g3flag==1 || Late.g3flag==3) {
    lnScale = lnScale-LOGe(Late.alpha);
    tGamma = hzd_ln_eX_plus1(lnTTauG);
    tEta = hzd_ln_eX_min1(tGamma/Late.alpha);
    ErrorPassBack("DLG3930");
    HazG3.lnG3 = Late.eta*tEta;
    HazG3.lnSG3 = lnScale+(Late.eta-ONE)*tEta+
      ((ONE-Late.alpha)/Late.alpha)*tGamma+
      (Late.gamma-ONE)*lnTTau;
  } else {
    tGamma = hzd_ln_eeX_min1(lnTTauG);
    ErrorPassBack("DLG3940");
    HazG3.lnG3 = Late.eta*tGamma;
    HazG3.lnSG3 = lnScale+(Late.eta-ONE)*tGamma+
      ePOW(lnTTauG)+(Late.gamma-ONE)*lnTTau;
  }
}

