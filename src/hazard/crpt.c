#include <hzfxpc.h>
#include "cput_v.h"
#include "cput.h"

void crpt(void){
  hzfxpc("Phase",5,31);
  hzfxpc("Variable",8,-5);
  hzfxpc("Mean",4,-11);
  hzfxpc("Min",3,-10);
  hzfxpc("Max",3,-11);
  cput_v(65,31,cput);
}
