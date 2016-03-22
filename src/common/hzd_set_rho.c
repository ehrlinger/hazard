#include <string.h>
#include "structures.h"
#include "hzd_Error.h"
#include "hzd_ln_eX_min1.h"
#include "hzd_ln_1min_enegX.h"
#include "hzd_ln_eBXmin1divB.h"

void hzd_set_rho(struct early *Early){
  double Bt,lnBt,lnRho=0,mln2,ln_mTerm;

  if(Early->delta0) {
    Bt = Early->tHalf;
    lnBt = LOGe(Bt);
  } else {
    lnBt = hzd_ln_eBXmin1divB(Early->delta,Early->tHalf);
    ErrorPassBack("DG1RHO900");
    Bt = ePOW(lnBt);
  }

  switch(Early->g1flag) {

  case 1: case 5:
    mln2 = Early->m*LnLim.two;
    lnRho = lnBt+Early->nu*hzd_ln_eX_min1(mln2);
    ErrorPassBack("DG1RHO910");
    break;

  case 2: case 6:
    lnRho = lnBt+Early->nu*LnLim.twotwo;
    break;

  case 3:
    mln2 = -Early->m*LnLim.two;
    ln_mTerm = -Early->nu*hzd_ln_1min_enegX(mln2);
    ErrorPassBack("DG1RHO940");
    lnRho = lnBt-hzd_ln_eX_min1(ln_mTerm);
    ErrorPassBack("DG1RHO950");
    break;

  case 4:
    mln2 = -Early->m*LnLim.two;
    Early->rho = -Bt/hzd_ln_1min_enegX(mln2);
    ErrorPassBack("DG1RHO970");
    return;

  }

  if(lnRho<LnLim.absMin || lnRho>LnLim.absMax)
    hzd_Error("DG1RHO960",1);
  Early->rho = ePOW(lnRho);
}
