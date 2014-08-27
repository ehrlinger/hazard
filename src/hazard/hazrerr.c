#include "hazard.h"

#include "hazrd4.h"
#include "writeOutputDatafile.h"

#include <hzfskp.h>
#include <xexit.h>
/****************************************************************/
/* HAZERR called from HZRBOMB, hazrd2.c:HZ2LOOP,                */
/*        hazrd2.c:DOOPTIM, hazrd2.c:HAZ2TRM, hazrd3.c:HAZ3TRM, */
/*        hazrd4.c:HAZ4TRM, hazrd4.c:FASTTRM                    */
void hazrerr(void){
  hzfskp(2);
  HZ5G();
  if(outhaz) {
    H->nocov = 1;
    writeOutputDatafile();
    fclose(outputDataFile);
  }
  xexit(1);
}
