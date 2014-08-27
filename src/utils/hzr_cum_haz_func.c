#include "common.h"
#include "hzr_set_error.h"
#include <math.h>
#include "hzd_ln_G1_and_SG1.h"
#include "hzd_ln_G3_and_SG3.h"

logical hzr_cum_haz_func(double t,double *cfn,double **cps,
			 double *hfn,double **hps,logical upd_mu){
  if(Common.phase[1]) {
    if(upd_mu)
      HZRstr.lnmu1[G.I] = G.lmu[1];
    if(G.entry!=SETCOE && HZRstr.fixg1) {
      HazG1.lnG1 = (*cps)[G.I];
      if(hps!=NULL)
	HazG1.lnSG1 = (*hps)[G.I];
    } else {
      HazG1.T = t;
      hzd_ln_G1_and_SG1();
      if(Common.errorno)
	return FALSE;
      if(G.entry!=DLLIKE) {
	(*cps)[G.I] = HazG1.lnG1;
	if(hps!=NULL)
	  (*hps)[G.I] = HazG1.lnSG1;
      }
    }
    cfn[1] = G.lmu[1]+HazG1.lnG1;
    if(hfn!=NULL)
      hfn[1] = G.lmu[1]+HazG1.lnSG1;
  }

  cps++;
  if(Common.phase[2]==1) {
    if(upd_mu)
      HZRstr.lnmu2[G.I] = G.lmu[2];
    if(G.entry==SETCOE) {
      if(t>=Machn.absMax || t<=Machn.absMin) {
	hzr_set_error("(SETCOE940)",1);
	return FALSE;
      }
      (*cps)[G.I] = LOGe(t);
    }
    cfn[2] = G.lmu[2]+(*cps)[G.I];
    if(hfn!=NULL)
      hfn[2] = G.lmu[2];
  }

  cps++;
  if(hps!=NULL)
    hps++;
  if(Common.phase[3]) {
    if(upd_mu)
      HZRstr.lnmu3[G.I] = G.lmu[3];
    if(G.entry!=SETCOE && HZRstr.fixg3) {
      HazG3.lnG3 = (*cps)[G.I];
      if(hps!=NULL)
	HazG3.lnSG3 = (*hps)[G.I];
    } else {
      HazG3.T = t;
      hzd_ln_G3_and_SG3();
      if(Common.errorno)
	return FALSE;
      if(G.entry!=DLLIKE) {
	(*cps)[G.I] = HazG3.lnG3;
	if(hps!=NULL)
	  (*hps)[G.I] = HazG3.lnSG3;
      }
    }
    cfn[3] = G.lmu[3]+HazG3.lnG3;
    if(hfn!=NULL)
      hfn[3] = G.lmu[3]+HazG3.lnSG3;
  } else {
    cfn[3] = ZERO;
    if(hfn!=NULL)
      hfn[3] = ZERO;
  }
  return TRUE;
}
