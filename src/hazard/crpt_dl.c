#include <string.h>
#include <hzfxpc.h>
#include "cput_v.h"
#include "cput_dl.h"

void crpt_dl(void){
  hzfxpc("Phase",5,5);
  hzfxpc("Variable",8,-5);
  hzfxpc("Label",5,-6);
  hzfxpc("Missing",7,-38);
  hzfxpc("Mean",4,-6);
  hzfxpc("Min",3,-10);
  hzfxpc("Max",3,-11);
  cput_v(115,5,cput_dl);
}
