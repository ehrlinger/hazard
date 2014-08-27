#include "structures.h"
#include "hzd_Error.h"

void hzd_early_p2t(double *sTheta){
  double absDelta;
#ifdef DEBUG
     printf("hzd_early_p2t\n");
#endif /*DEBUG*/

  if(Common.status[0]==1) {
    absDelta = fabs(Early.delta);
    if(absDelta>=ONE)
      hzd_Error("DTRSFM910",2);
    else
      sTheta[0] = LOGe(-LOGe(absDelta));
  }
  if(Common.status[1]==1){
    if(Early.tHalf<=ZERO){
      hzd_Error("DTRSFM930",2);
    }else{
      sTheta[1] = LOGe(Early.tHalf);
    }
  }
  if(Common.status[2]==1)
    sTheta[2] = LOGe(fabs(Early.nu));
  if(Common.status[3]==1)
    sTheta[3] = LOGe(fabs(Early.m));
}
