#include <string.h>
#include "hazard.h"
#include <hzdinc.h>
#include <isblanck.h>

/****************************************************************/
/* READWT called from READOBS                                   */
void readwt(void){
  if(!ISBLANCK(wtname))
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
