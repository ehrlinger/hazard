#include "hazpred.h"
#include "hzd_set_rho.h"
#include "hzd_parms_to_theta.h"
#include "hzp_init_CL.h"
/*
  hzpe - further memory mapping (?)

  Used by:  called by main functions

  Uses: common/hzd_set_rho();
  common/hzd_parms_to_theta();
  hzp_init_CL();

  Globals: Early.delta
  Early.tHalf 
  Early.nu    
  Early.m     
  Late.tau    
  Late.gamma  
  Late.alpha  
  Late.eta    
  Common.theta
  Common.phase
  Common.status
  Common.p
  HZPstr.noCL
  
  Passed: 

  Returns: 

*/
void hzpe(void){
#ifdef DEBUG
     printf("hzpe\n");
#endif /*DEBUG*/

  /* Assign values that we read in from the file to the
   * correct structures.
   *
   * This could get recursive. We already have pointers
   * to the Early and Late structures in Common. The question
   * is where do we set the Common,theta[i]'s? They are all
   * model parameters.
   */
  Early.delta = Common.theta[0];
  Early.tHalf = Common.theta[1];
  Early.nu    = Common.theta[2];
  Early.m     = Common.theta[3];
  Late.tau    = Common.theta[4];
  Late.gamma  = Common.theta[5];
  Late.alpha  = Common.theta[6];
  Late.eta    = Common.theta[7];
  
  /* 
   * 
   */
  for(i=0; i<8; i++)
    Common.theta[i] = ZERO;
  Common.phase[1] = (Common.status[8]==1);
  Common.phase[2] = (Common.status[Common.p+9]==1);
  Common.phase[3] = (Common.status[Common.p*2+10]==1);
  if(Common.phase[1])
    hzd_set_rho(&Early);
  hzd_parms_to_theta();
  if(!HZPstr.noCL)
    hzp_init_CL();
}
