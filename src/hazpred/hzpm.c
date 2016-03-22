#include <string.h>
#include "hazpred.h"
#include "hzd_Error.h"

void hzpm(int obsCnt){
  Common.Nobs = obsCnt;
  Common.Ntheta = Common.Nobs-6;
  Common.p = (Common.Ntheta-11)/3;
  if((Common.p*3+11)!=Common.Ntheta)
    hzd_Error("HZPM",1);
  Common.status = Intfc.malloc((size_t)Common.Ntheta*
			       sizeof(short int));
  Common.theta = Intfc.malloc((size_t)Common.Ntheta*sizeof(double));
  Common.obs = Intfc.malloc((size_t)(Common.Ntheta)*sizeof(double));
  if(HZPstr.Sflag)
    dCf = Intfc.malloc((size_t)Common.Ntheta*sizeof(double));
  if(HZPstr.Hflag)
    dHf = Intfc.malloc((size_t)Common.Ntheta*sizeof(double));
  Common.cov = Intfc.malloc((size_t)(Common.Ntheta*Common.Ntheta)*
			    sizeof(double));
}
