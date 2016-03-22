#include <string.h>
#include "hazard.h"
#include <isblanck.h>

/****************************************************************/
/* READLC called from READOBS                                   */
void readlc(void){
  if(!ISBLANCK(lcname)){
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
