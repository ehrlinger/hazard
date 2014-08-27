#include "structures.h"
#include "hzd_ln_eBXmin1divB.h"
#include "hzd_Error.h"
#include "hzd_ln_A_div_B.h"
#include "hzd_ln_eX_plus1.h"
#include "hzd_ln_1min_enegX.h"
#include "hzd_ln_1min_e_negeX.h"

void hzd_ln_G1_and_SG1_m(double T,double *lnG1,double *lnSG1){
  double Bt,dBt,lnX,mInvNu,mInvNu_1,mInvM,mInvM_1,termNu,termM;

#ifdef DEBUG
     printf("hzd_ln_g1_and_SG1_m\n");
#endif /*DEBUG*/

  if(Early.delta0) {
    Bt = T;
    dBt = ZERO;
  } else {
    dBt = Early.delta*T;
    lnX = hzd_ln_eBXmin1divB(Early.delta,T);
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
    *lnG1 = mInvM*termM;
    *lnSG1 = mInvNu_1*termNu+mInvM_1*termM+dBt-
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
    *lnG1 = -ePOW(lnX);
    *lnSG1 = *lnG1+mInvNu_1*termNu+dBt-
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
    *lnG1 = mInvM*termM;
    *lnSG1 = mInvNu_1*termNu+mInvM_1*termM+dBt-
      LOGe(-Early.m*Early.nu*Early.rho);
    break;

  case 4:
    mInvM = -ONE/Early.m;
    mInvM_1 = mInvM-ONE;
    lnX = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG1980");
    termM = hzd_ln_1min_e_negeX(lnX);
    *lnG1 = mInvM*termM;
    *lnSG1 = mInvM_1*termM-(Bt/Early.rho)+dBt-
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
    *lnG1 = hzd_ln_1min_enegX(lnX);
    ErrorPassBack("DLG11000");
    *lnSG1 = mInvM_1*termM+mInvNu_1*termNu+dBt-
      LOGe(-Early.m*Early.nu*Early.rho);
    break;

  case 6:
    mInvNu = -ONE/Early.nu;
    mInvNu_1 = mInvNu-ONE;
    termNu = hzd_ln_A_div_B(Bt,Early.rho);
    ErrorPassBack("DLG11010");
    lnX = mInvNu*termNu;
    *lnG1 = hzd_ln_1min_e_negeX(lnX);
    *lnSG1 = mInvNu_1*termNu-ePOW(lnX)+dBt-
      LOGe(-Early.nu*Early.rho);
    break;

  default:
    *lnG1 = ZERO;
    *lnSG1 = ZERO;
    hzd_Error("DLG1910",2);

  }
}
