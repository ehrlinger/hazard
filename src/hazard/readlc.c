#include "hazard.h"
#include "isblank.h"

/****************************************************************/
/* READLC called from READOBS                                   */
void readlc(void){
  if(!ISBLANK(lcname)){
    if(ISMISS(LC)) {
      mlcdel++;
      mdel = 1;
    } else if(LC<ZERO || LC>=T) {
      lcdel++;
      del = 1;
    } else if(LC>ZERO) {
      lcens++;
      lcmin = lcmin<LC ? lcmin : LC;
      lcmax = lcmax>LC ? lcmax : LC;
    }
  }else{
    LC = ZERO;
  }
}
