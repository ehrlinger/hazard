#include <string.h>
#include "hazard.h"
#include <isblanck.h>

/****************************************************************/
/* READCT called from READOBS                                   */
void readct(void){
  if(!ISBLANCK(c3name) && C3>0){
    if(ISMISS(CT)) {
      mctdel = mctdel+1;
      mdel = 1;
    }else if(CT<ZERO) {
      ctdel = ctdel+1;
      del = 1;
    }else if(CT>T) {
      ctdel = ctdel+1;
      del = 1;
    }else if(CT==T) {
      C1 = C1+C3;
      C3 = CT = ZERO;
      ivent = ivent+1;
      c1min = c1min<C1 ? c1min : C1;
      c1max = c1max>C1 ? c1max : C1;
    }else if(C3>ZERO) {
      ctmin = ctmin<CT ? ctmin : CT;
      ctmax = ctmax>CT ? ctmax : CT;
    }
  }
}
