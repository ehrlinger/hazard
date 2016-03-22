#include <string.h>
#include <hzfxpr.h>
#include <hzfskp.h>
#include "cput_vv.h"
#include "hazard.h"

void cput_v(int dashct,int dashat,void (*cputfn)(char *,long)){
  hzfskp(1);
  hzfxpr('-',dashct,dashat);
  hzfskp(1);
  if(H->phase1==1)
    cput_vv("Early:   ",iearly,cputfn);
  if(H->phase1==1 && (H->phase2==1 || H->phase3==1)) {
    hzfxpr('-',dashct-10,dashat+10);
    hzfskp(1);
  }
  if(H->phase2==1)
    cput_vv("Constant:",iconst,cputfn);
  if(H->phase2==1 && H->phase3==1) {
    hzfxpr('-',dashct-10,dashat+10);
    hzfskp(1);
  }
  if(H->phase3==1)
    cput_vv("Late:    ",ilate,cputfn);
  hzfxpr('-',dashct,dashat);
  hzfskp(1);
}
