#include "structures.h"
#include "hzd_Error.h"
#include "hzd_ln_A_div_B.h"
#include "hzd_ln_eX_plus1.h"
#include "hzd_ln_eX_min1.h"
#include "hzd_ln_eeX_min1.h"

void hzd_ln_G3_and_SG3(void){
#ifdef DEBUG
     printf("hzd_ln_G3_and_SG3\n");
#endif /*DEBUG*/
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

