#include "structures.h"
#include "hzd_Error.h"

void hzd_late_p2t(double *sTheta){
  double ge_2,ge_a_2;

#ifdef DEBUG
  printf("hzd_late_p2t\n");
#endif /*DEBUG*/

  if(Common.status[4]==1){
    if(Late.tau<=ZERO){
      hzd_Error("DTRSFM970",2);
    }else{
      sTheta[4] = LOGe(Late.tau);
    }
  }
  if(Common.status[5]==1){
    if(Late.g_two || Late.g3flag>=3 || Common.status[7]==1){
      if(Late.gamma<=ZERO){
	hzd_Error("DTRSFM990",2);
      }else{
	sTheta[5] = LOGe(Late.gamma);
      }
    }else {
      ge_2 = Late.gamma*Late.eta-Machn.two;
      if(ge_2<=ZERO){
	hzd_Error("DTRSM1010",2);
      }
      sTheta[5] = LOGe(ge_2);
    }
  }
  if(Common.status[7]==1){
    if(Late.g_two || Late.g3flag>=3){
      if(Late.eta<=ZERO){
	hzd_Error("DTRSFM1030",2);
      }else{
	sTheta[7] = LOGe(Late.eta);
      }
    }else{
      ge_2 = Late.gamma*Late.eta-Machn.two;
      if(ge_2<=ZERO){
	hzd_Error("DTRSFM1050",2);
      }
      sTheta[7] = LOGe(ge_2);
    }
  }
  if(Common.status[6]==1){
    if((Late.ga_two && !Late.g_two) || Late.g3flag>=3){
      if(Late.alpha<=ZERO){
	hzd_Error("DTRSFM1070",2);
      }else{
	sTheta[6] = LOGe(Late.alpha);
      }
    }else{
      ge_a_2 = (Late.gamma*Late.eta/Late.alpha)-Machn.two;
      if(ge_a_2<=ZERO){
	hzd_Error("DTRSFM1090",2);
      }
      sTheta[6] = LOGe(ge_a_2);
    }
  }
}
