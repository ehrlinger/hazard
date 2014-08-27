#include "structures.h"
#include "hzd_early_t2p.h"
#include "hzd_late_t2p.h"
#include "hzd_set_rho.h"

void hzd_theta_to_parms(int chgd_E,int chgd_L){
#ifdef DEBUG
     printf("hzd_theta_to_parms\n");
#endif /*DEBUG*/

  if(Common.phase[1]==1 && chgd_E) {
    TempE = Early;
    hzd_early_t2p();
  }
  if(Common.phase[3]==1) {
    TempL = Late;
    hzd_late_t2p();
  }
  if(Common.phase[1]==1 && chgd_E && (Common.status[0]==1 ||
				      Common.status[1]==1 || Common.status[2]==1 ||
				      Common.status[3]==1))
    hzd_set_rho(&TempE);
  if(Common.phase[1]==1 && chgd_E)
    Early = TempE;
  if(Common.phase[3]==1 && chgd_L)
    Late = TempL;
}
