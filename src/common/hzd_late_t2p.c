#include "structures.h"
#include "hzd_Error.h"

void hzd_late_t2p(void){
#ifdef DEBUG
     printf("hzd_late_t2p\n");
#endif /*DEBUG*/

  double gamma_eta;

  if(Common.status[4]==1){
    if(Common.theta[4]>=LnLim.absMax ||
       Common.theta[4]<=LnLim.absMin){
      hzd_Error("DTRSFM1150",1);
    }else{
      TempL.tau = ePOW(Common.theta[4]);
    }
  }
  if(Common.status[5]==1){
    if(Late.g_two || Late.g3flag>=3 || Common.status[7]==1){
      if(Common.theta[5]>=LnLim.absMax ||
	 Common.theta[5]<=LnLim.absMin){
	hzd_Error("DTRSFM1160",1);
      }else{
	TempL.gamma = ePOW(Common.theta[5]);
	if(Late.g_two){
	  TempL.eta = Machn.two/TempL.gamma;
	}
      }
    }else{
      if(Common.theta[5]>=LnLim.absMax ||
	 Common.theta[5]<=LnLim.absMin){
	hzd_Error("DTRSFM1170",1);
      }else{
	TempL.gamma = (ePOW(Common.theta[5])+Machn.two)/Late.eta;
      }
    }
  }
  if(Common.status[7]==1){
    if(Late.g_two || Late.g3flag>=3){
      if(Common.theta[7]>=LnLim.absMax ||
	 Common.theta[7]<=LnLim.absMin){
	hzd_Error("DTRSFM1180",1);
      }else{
	TempL.eta = ePOW(Common.theta[7]);
	if(Late.g_two){
	  TempL.gamma = Machn.two/TempL.eta;
	}
      }
    }else{
      if(Common.theta[7]>=LnLim.absMax ||
	 Common.theta[7]<=LnLim.absMin){
	hzd_Error("DTRSFM1190",1);
      }else{
	TempL.eta = (ePOW(Common.theta[7])+Machn.two)/Late.gamma;
      }
    }
  }
  if(Common.status[6]==1){
    if((Late.ga_two && !Late.g_two) || Late.g3flag>=3){
      if(Common.theta[6]>=LnLim.absMax ||
	 Common.theta[6]<=LnLim.absMin){
	hzd_Error("DTRSFM1200",1);
      }else{
	TempL.alpha = ePOW(Common.theta[6]);
      }
    }else{
      gamma_eta = TempL.gamma*TempL.eta;
      if(gamma_eta<=ZERO){
	hzd_Error("DTRSFM1210",2);
      }
      if(Common.theta[6]>=LnLim.absMax ||
	 Common.theta[6]<=LnLim.absMin){
	hzd_Error("DTRSFM1220",1);
      }else{
	TempL.alpha = gamma_eta/(ePOW(Common.theta[6])+Machn.two);
      }
    }
  }else{
    if(Late.ga_two){
      TempL.alpha = TempL.gamma*TempL.eta/Machn.two;
    }
  }
}

