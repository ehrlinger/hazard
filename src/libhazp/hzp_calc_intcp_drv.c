#include "hazpred.h"
#include "hzp_calc_intcp_subr.h"

/*
  hzp_calc_intcp_drv

  Used by: hzpo

  Uses: 
  hzp_calc_intcp_subr

  Globals:
  dCf
  dHf
  Common.status
  Common.phase[j+1]
  HZPstr.Sflag
  Cf[j]
  HZPstr.Hflag
  Hf[j]
  pp1

  Passed: 

  Returns: 



*/
void hzp_calc_intcp_drv(void){
#ifdef DEBUG
  printf("hzp_calc_intcp_drv\n");
#endif /*DEBUG*/

  double *Cr,*Hr;
  short int *Cs,*Hs;

  Cr = dCf+8;
  Hr = dHf+8;
  Cs = Hs = Common.status+8;
  for(j=0; j<3; j++)
    if(Common.phase[j+1]==1) {
      if(HZPstr.Sflag)
	hzp_calc_intcp_subr(Cf[j],&Cr,&Cs);
      if(HZPstr.Hflag)
	hzp_calc_intcp_subr(Hf[j],&Hr,&Hs);
    } else {
      Cr += pp1;
      Hr += pp1;
      Hs = (Cs += pp1);
    }
}
