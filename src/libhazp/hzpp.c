#include "hazpred.h"
#include "hzd_calc_norinv.h"
/* 
   hzpe 

   Used by: Called by main program

   Uses: hzd_calc_norinv   

   Globals: HZPstr.noCL
   HZPstr.Znormal
   HZPstr.CLimit
   HZPstr.Pvalue
   Machn.two
   
   Passed: 

   Returns: 

*/
void hzpp(void){
#ifdef DEBUG
     printf("hzpp\n");
#endif /*DEBUG*/

  if(HZPstr.noCL)
    HZPstr.Znormal = ZERO;
  else
    if(HZPstr.CLimit<=ZERO || HZPstr.CLimit>=ONE)
      HZPstr.Znormal = ONE;
    else {
      HZPstr.Pvalue = (ONE-HZPstr.CLimit)/Machn.two;
      HZPstr.Znormal = fabs(hzd_calc_norinv(HZPstr.Pvalue));
    }
}
