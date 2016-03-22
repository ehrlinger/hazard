#include <string.h>
#include "hazard.h"
#include <isblanck.h>

/****************************************************************/
/* READT called from READOBS                                    */
void readt(void){
  if(!ISBLANCK(tname))
    if(ISMISS(T)) {
      mtdel = mtdel+1;
      mdel = 1;
    } else if(T<=ZERO) {
      tdel = tdel+1;
      del = 1;
    } else {
      tmin = tmin<T ? tmin : T;
      H->Tmax = H->Tmax>T ? H->Tmax : T;
    }
  else
    T = ZERO;
}
