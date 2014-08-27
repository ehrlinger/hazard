#include <hzfxpc.h>
#include "cput_v.h"
#include "cput_l.h"

void crpt_l(void){
  hzfxpc("Phase",5,10);
  hzfxpc("Variable",8,-5);
  hzfxpc("Label",5,-6);
  hzfxpc("Mean",4,-42);
  hzfxpc("Min",3,-11);
  hzfxpc("Max",3,-10);
  cput_v(107,10,cput_l);
}
