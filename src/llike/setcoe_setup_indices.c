#include <common.h>

void SETCOE_setup_indices(void){
  long i,j,ji,j1,j1p1,j1pp,jind;
 
  /* SET UP INDICES TO SPEED PASSES THROUGH THE DATA */
  i = -1;
  for(j=1; j<=3; j++) {
    if(Common.phase[j]!=1)
      continue;
    i = i+1;
    /**
       INDXX IS LOCATION IN OBS OF THE FIRST VALUE OF EACH
       CONCOMMITANT VARIABLE; IF AN INTERCEPT, THIS IS 0
    **/
    HZRstr.indxx[i] = 0;
    /* INDXP IS THE PHASE */
    HZRstr.indxp[i] = j;
    j1 = (HZRstr.pj1+HZRstr.pp1*j)-1;
    /**
       INDXT IS THE LOCATION IN THETA OF THE INFORMATION.  INITIALLY
       SET IT TO THE INTERCEPT (MODIFIED LATER FOR CONCOMMITANT
       INFORMATION)
    **/
    HZRstr.indxt[i] = j1;
    if(Common.p==0)
      continue;
    j1p1 = j1+1;
    j1pp = j1+Common.p;
    jind = 7-j1;
    for(ji=j1p1; ji<=j1pp; ji++)
      if(Common.status[ji]==1) {
	i = i+1;
	HZRstr.indxx[i] = ji+jind;
	HZRstr.indxp[i] = j;
	HZRstr.indxt[i] = ji;
      }
  }
 
  /* NUMBER OF VARIABLES (INCLUDING INTERCEPTS) */
  HZRstr.ivar = i+1;
}
 
