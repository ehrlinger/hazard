/**
 * @file hzd_ln_G1_and_SG1.c
 * @brief Early-phase shaping function G1 and hazard shaping function SG1.
 *
 * Implements all six special cases of the early-phase cumulative hazard
 * shaping function \f$G_1(t)\f$ and its time derivative \f$SG_1(t)\f$
 * from the three-phase parametric hazard model of Blackstone, Naftel,
 * and Turner (1986).
 *
 * The general 4-parameter form (in log domain) uses B(t) as an
 * exponentially-transformed time:
 * \f[
 *   B(t) = \frac{e^{\delta t} - 1}{\delta}, \quad
 *   \frac{dB}{dt} = e^{\delta t}
 * \f]
 * (or \f$B(t)=t\f$, \f$dB/dt=1\f$ when \f$\delta=0\f$).
 *
 * Case g1flag=1 (M>0, ν>0 — Negative Generic):
 * \f[
 *   G_1 = \left[1 + \left(\frac{B(t)}{\rho}\right)^{-1/\nu}\right]^{-1/M}
 * \f]
 *
 * Case g1flag=2 (M=0, ν>0 — Limiting/Gompertz):
 * \f[
 *   G_1 = \exp\!\left[-\left(\frac{B(t)}{\rho}\right)^{-1/\nu}\right]
 * \f]
 *
 * Cases 3–6 follow analogously (see FORTRAN source `model91/dlg1` for the
 * full formulas for each flag).
 *
 * **FORTRAN origin:** `model91/dlg1` (DLG1 subroutine) — six-case switch
 * matching the FORTRAN's IF-ladder. Verified in the C port.
 * **TemporalHazard R equivalent:** `hzd_G1()` in the R package.
 *
 * @see hzd_ln_G1_and_SG1.h
 * @see setg1() for g1flag assignment.
 * @see hzd_ln_G1_and_SG1_m.c for the variant with M as a free parameter.
 */
#include <string.h>
#include "structures.h"
#include "hzd_Error.h"
#include "hzd_ln_eBXmin1divB.h"
#include "hzd_ln_A_div_B.h"
#include "hzd_ln_eX_plus1.h"
#include "hzd_ln_1min_enegX.h"
#include "hzd_ln_1min_e_negeX.h"

void hzd_ln_G1_and_SG1(void){
  double Bt,dBt,lnX,mInvNu,mInvNu_1,mInvM,mInvM_1,termNu,termM;

  HazG1.lnG1 = ZERO;
  HazG1.lnSG1 = ZERO;

  if(HazG1.T<=ZERO)
    hzd_Error("DLG1900",2);

  if(Early.g1flag<1 || Early.g1flag>6)
    hzd_Error("DLG1910",2);

  if(Early.delta0) {
    Bt = HazG1.T;
    dBt = ZERO;
  } else {
    dBt = Early.delta*HazG1.T;
    lnX = hzd_ln_eBXmin1divB(Early.delta,HazG1.T);
    ErrorPassBack("DLG1920");
    Bt = ePOW(lnX);
  }

  switch(Early.g1flag) {

  case 1:
    mInvNu = -ONE/Early.nu;
    mInvNu_1 = mInvNu-ONE;
    mInvM = -ONE/Early.m;
    mInvM_1 = mInvM-ONE;
    termNu = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG1930");
    lnX = mInvNu*termNu;
    if(lnX<LnLim.absMin || lnX>LnLim.absMax)
      hzd_Error("DLG1935",1);
    termM = hzd_ln_eX_plus1(lnX);
    HazG1.lnG1 = mInvM*termM;
    HazG1.lnSG1 = mInvNu_1*termNu+mInvM_1*termM+dBt-
      LOGe(Early.m*Early.nu*Early.rho);
    break;

  case 2:
    mInvNu = -ONE/Early.nu;
    mInvNu_1 = mInvNu-ONE;
    termNu = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG1940");
    lnX = mInvNu*termNu;
    if(lnX<LnLim.absMin || lnX>LnLim.absMax)
      hzd_Error("DLG1950",1);
    HazG1.lnG1 = -ePOW(lnX);
    HazG1.lnSG1 = HazG1.lnG1+mInvNu_1*termNu+dBt-
      LOGe(Early.nu*Early.rho);
    break;

  case 3:
    mInvNu = -ONE/Early.nu;
    mInvNu_1 = mInvNu-ONE;
    mInvM = -ONE/Early.m;
    mInvM_1 = mInvM-ONE;
    lnX = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG1960");
    termNu = hzd_ln_eX_plus1(lnX);
    lnX = termNu/Early.nu;
    termM = hzd_ln_1min_enegX(lnX);
    ErrorPassBack("DLG1970");
    HazG1.lnG1 = mInvM*termM;
    HazG1.lnSG1 = mInvNu_1*termNu+mInvM_1*termM+dBt-
      LOGe(-Early.m*Early.nu*Early.rho);
    break;

  case 4:
    mInvM = -ONE/Early.m;
    mInvM_1 = mInvM-ONE;
    lnX = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG1980");
    termM = hzd_ln_1min_e_negeX(lnX);
    HazG1.lnG1 = mInvM*termM;
    HazG1.lnSG1 = mInvM_1*termM-(Bt/Early.rho)+dBt-
      LOGe(-Early.m*Early.rho);
    break;

  case 5:
    mInvNu = -ONE/Early.nu;
    mInvNu_1 = mInvNu-ONE;
    mInvM = -ONE/Early.m;
    mInvM_1 = mInvM-ONE;
    termNu = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG1990");
    lnX = mInvNu*termNu;
    termM = hzd_ln_eX_plus1(lnX);
    lnX = termM/Early.m;
    HazG1.lnG1 = hzd_ln_1min_enegX(lnX);
    ErrorPassBack("DLG11000");
    HazG1.lnSG1 = mInvM_1*termM+mInvNu_1*termNu+dBt-
      LOGe(-Early.m*Early.nu*Early.rho);
    break;

  case 6:
    mInvNu = -ONE/Early.nu;
    mInvNu_1 = mInvNu-ONE;
    termNu = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG11010");
    lnX = mInvNu*termNu;
    HazG1.lnG1 = hzd_ln_1min_e_negeX(lnX);
    HazG1.lnSG1 = mInvNu_1*termNu-ePOW(lnX)+dBt-
      LOGe(-Early.nu*Early.rho);
    break;

  }
}
