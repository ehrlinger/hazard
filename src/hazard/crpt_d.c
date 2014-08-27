#include <hzfxpc.h>
#include "cput_v.h"
#include "cput_d.h"

void crpt_d(void){
  hzfxpc("Phase",5,26);
  hzfxpc("Variable",8,-5);
  hzfxpc("Missing",7,-7);
  hzfxpc("Mean",4,-7);
  hzfxpc("Min",3,-10);
  hzfxpc("Max",3,-10);
  cput_v(74,26,cput_d);
}
