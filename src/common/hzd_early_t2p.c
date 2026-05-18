/**
 * @file hzd_early_t2p.c
 * @brief Unconstrained optimizer theta[] → early-phase model parameters.
 *
 * Inverts the p2t transform: recovers (DELTA, THALF, NU, M) from
 * the BFGS optimizer's unconstrained theta[0..3]. Results are written
 * to `TempE` (not `Early`) so the caller can validate before committing.
 *
 * **FORTRAN origin:** `model91/dtrsfm` IOP=2 (E.H. Blackstone / C.R. Katholi,
 * November 1985; delta inverse revised October 1986).
 *
 * @see hzd_early_t2p.h
 * @see hzd_early_p2t.c for the forward transform.
 */
#include <string.h>
#include "structures.h"
#include "hzd_Error.h"

void hzd_early_t2p(void){
  if(Common.status[0]==1){
    if(Common.theta[0]>=LOGe(LnLim.absMax)){
      hzd_Error("DTRSFM1110",1);
    }else{
      if(Common.theta[0]<LnLim.absMin){
	TempE.delta = ONE;
      }else{
	TempE.delta = ePOW(-ePOW(Common.theta[0]));
      }
      if(Early.deltaN){
	TempE.delta = -TempE.delta;
      }
    }
  }
  if(Common.status[1]==1){
    if(Common.theta[1]>=LnLim.absMax ||
       Common.theta[1]<=LnLim.absMin){
      hzd_Error("DTRSFM1120",1);
    }else{
      TempE.tHalf = ePOW(Common.theta[1]);
    }
  }
  if(Common.status[2]==1 || Common.status[3]==1) {
    if(Common.status[2]==1){
      if(Common.theta[2]>=LnLim.absMax ||
	 Common.theta[2]<=LnLim.absMin){
	hzd_Error("DTRSFM1130",1);
      }else{
	TempE.nu = ePOW(Common.theta[2]);
	if(Early.g1flag>=5){
	  TempE.nu = -TempE.nu;
	}
      }
    }
    if(Common.status[3]==1){
      if(Common.theta[3]>=LnLim.absMax ||
	 Common.theta[3]<=LnLim.absMin){
	hzd_Error("DTRSFM1140",1);
      }else {
	TempE.m = ePOW(Common.theta[3]);
	if(Early.g1flag==3 || Early.g1flag==4){
	  TempE.m = -TempE.m;
	}
      }
    }
    if(Early.mNuOne) {
      if(Common.status[2]==1) {
	TempE.m = ONE/TempE.nu;
	if(Early.g1flag==3 || Early.g1flag==5){
	  TempE.m = -TempE.m;
	}
      }
      if(Common.status[3]==1) {
	TempE.nu = ONE/TempE.m;
	if(Early.g1flag==3 || Early.g1flag==5){
	  TempE.nu = -TempE.nu;
	}
      }
    }
  }
}
