#include <string.h>
#include "structures.h"
#include "hzd_early_p2t.h"
#include "hzd_late_p2t.h"


/*------------------------------------------------------------------*/

void hzd_parms_to_theta(void){
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
