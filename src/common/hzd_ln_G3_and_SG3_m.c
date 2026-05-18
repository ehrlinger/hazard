/**
 * @file hzd_ln_G3_and_SG3_m.c
 * @brief G3/SG3 variant with unconstrained parameters.
 *
 * Mirrors hzd_ln_G3_and_SG3.c for estimation contexts where alpha
 * is treated as a continuous parameter rather than a flag-selected case.
 *
 * **FORTRAN origin:** `model91/dlg3` variant paths.
 * @see hzd_ln_G3_and_SG3.c for the flag-dispatch version.
 */
#include <string.h>
#include "structures.h"

#include "hzd_Error.h"
#include "hzd_ln_A_div_B.h"
#include "hzd_ln_eX_plus1.h"
#include "hzd_ln_eX_min1.h"
#include "hzd_ln_eeX_min1.h"

void hzd_ln_G3_and_SG3_m(double T, double *lnG3, double *lnSG3){
  double lnTTau,lnTTauG,lnScale,tGamma,tEta;


  if(T<=ZERO) hzd_Error("DLG3900",2);


  if(Late.g3flag==1 || Late.g3flag==3) {
    lnTTau = hzd_ln_A_div_B(T,Late.tau);
    lnTTauG = lnTTau*Late.gamma;
    lnScale = LOGe(Late.gamma)+LOGe(Late.eta)-LOGe(Late.tau);
    lnScale = lnScale-LOGe(Late.alpha);
    tGamma = hzd_ln_eX_plus1(lnTTauG);
    tEta = hzd_ln_eX_min1(tGamma/Late.alpha);
    if(Common.errorno){ 
      *lnG3 = ZERO;
      *lnSG3 = ZERO;
      hzd_Error("DLG3930",0);
    }
    *lnG3 = Late.eta*tEta;
    *lnSG3 = lnScale+(Late.eta-ONE)*tEta+
      ((ONE-Late.alpha)/Late.alpha)*tGamma+
      (Late.gamma-ONE)*lnTTau;
  } else if(Late.g3flag==2) {
    lnTTau = hzd_ln_A_div_B(T,Late.tau);
    lnTTauG = lnTTau*Late.gamma;
    lnScale = LOGe(Late.gamma)+LOGe(Late.eta)-LOGe(Late.tau);
    tGamma = hzd_ln_eeX_min1(lnTTauG);
    if(Common.errorno){ 
      *lnG3 = ZERO;
      *lnSG3 = ZERO;
      hzd_Error("DLG3930",0);
    }
    *lnG3 = Late.eta*tGamma;
    *lnSG3 = lnScale+(Late.eta-ONE)*tGamma+
      ePOW(lnTTauG)+(Late.gamma-ONE)*lnTTau;
  } else { 
    *lnG3 = ZERO;
    *lnSG3 = ZERO;
    hzd_Error("DLG3910",2);
    lnTTau = hzd_ln_A_div_B(T,Late.tau);
    lnTTauG = lnTTau*Late.gamma;
    lnScale = LOGe(Late.gamma)+LOGe(Late.eta)-LOGe(Late.tau);
  }
}
