#include "structures.h"
#include "hzd_early_p2t.h"
#include "hzd_late_p2t.h"

/*
  Called by hzpe in lib_hazpred 
*/
/*------------------------------------------------------------------*/

void hzd_parms_to_theta(void){
#ifdef DEBUG
     printf("hzd_parms_to_theta\n");
#endif /*DEBUG*/

  double sTheta[8];
  int i;

  if(Common.phase[1]==1)
    hzd_early_p2t(sTheta);
  if(Common.phase[3]==1)
    hzd_late_p2t(sTheta);
  for(i=0; i<8; i++)
    if(Common.status[i]==1)
      Common.theta[i] = sTheta[i];
}
