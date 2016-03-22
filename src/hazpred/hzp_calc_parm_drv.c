#include <string.h>
#include "hazpred.h"
#include "hzp_time_parm_drv.h"
#include "hzd_Error.h"
#include "hzp_calc_intcp_subr.h"

void hzp_calc_parm_drv(int phase){
  if(optmz&Chgd_Time)
    hzp_time_parm_drv();
  lnCf_ = lnMu[phase]+DlnG[j];
  lnHf_ = lnMu[phase]+DlnSG[j];
  if(lnCf_>LnLim.absMax || lnHf_>LnLim.absMax)
    hzd_Error("HZPO940",1);
  if(lnCf_>LnLim.absMin)
    Cf_ = ePOW(lnCf_);
  else
    Cf_ = ZERO;
  if(lnHf_>LnLim.absMin)
    Hf_ = ePOW(lnHf_);
  else
    Hf_ = ZERO;
  if(HZPstr.Sflag)
    dCf[j] = (Cf_-Cf[phase])/step[j];
  if(HZPstr.Hflag)
    dHf[j] = (Hf_-Hf[phase])/step[j];
}
