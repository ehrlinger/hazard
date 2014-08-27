#include "hazard.h"
#include <hzdinc.h>
#include "isblank.h"

/****************************************************************/
/* READWT called from READOBS                                   */
void readwt(void){
  if(!ISBLANK(wtname))
    if(ISMISS(WT)) {
      mwtdel++;
      mdel = 1;
    } else if(WT<ZERO) {
      wtdel++;
      del = 1;
    } else {
      wtmin = wtmin<WT ? wtmin : WT;
      wtmax = wtmax>WT ? wtmax : WT;
    }
  else
    WT = ONE;
}
