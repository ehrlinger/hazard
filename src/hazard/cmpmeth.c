#include "hazard.h"
#include <hzfskp.h>
#include <hzfxpc.h>

void cmpmeth(void){
  hzfskp(2);
  hzfxpc("Computational Methods:",22,5);
  hzfskp(1);
  hzfxpc("======================",22,5);
  hzfskp(2);
  hzfxpc("Optimization: ",14,7);
  if(H->newton && ((H->angrad && H->anhess) || (!H->angrad &&
						!H->anhess)))
    hzfxpc("Newton",6,0);
  else
    hzfxpc("Quasi-Newton",12,0);
  hzfxpc("method using ",13,-1);
  if(H->angrad && H->anhess)
    hzfxpc("analytic",8,0);
  else
    hzfxpc("numeric",7,0);
  hzfxpc("derivatives",11,-1);
  hzfskp(1);
  if(H->truhes)
    hzfxpc("initiated using an initial hessian matrix",41,21);
  else {
    hzfxpc("initiated using the method of steepest",38,21);
    hzfxpc(" descent",8,0);
  }
  hzfskp(2);
  hzfxpc("Conservation of events:",23,7);
  if(H->mode==0)
    hzfxpc("Invoked at each iteration",25,31);
  else if(H->mode==1)
    hzfxpc("Invoked initially",17,31);
  else
    hzfxpc("Not invoked",11,31);
  hzfskp(1);
}
