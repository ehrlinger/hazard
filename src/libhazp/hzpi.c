#include "hzd_set_Machn.h"
#include "hzd_set_LnLim.h"
#include "hzd_init_Common.h"
#include "hazpred.h"
/*
  hzpi - initializes machine figures
  (Should be in autoconf)
  - Sets up common memory.

	 
  Used by: main function of programs

  Uses: hzd_set_Machn(); -libcommon
  hzd_set_LnLim();       -libcommon
  hzd_init_Common()      -libcommon
  HZDPOSTI()

  I'm not sure why this isn't also a common function
  hazard may need more memory/initial setup than 
  hazpred, but we should be able to do that as an
  additional function call after setting up the 
  main memory areas.

  Globals: Common.hzpstr
  HZPstr
  Version.core_ver
  HZPVER

  Passed: 

  Returns: struct common

*/

struct common *hzpi(void){
#ifdef DEBUG
     printf("hzpi\n");
#endif /*DEBUG*/

  hzd_set_Machn();
  hzd_set_LnLim();
  hzd_init_Common();
  Common.hzpstr = &HZPstr;
  Version.core_ver = HZPVER;
#ifdef HZDPOSTI
  HZDPOSTI();
#endif
  return &Common;
}
