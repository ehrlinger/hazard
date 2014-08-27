#include "hazard.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include "ismuchg.h"
#include <hzfskp.h>

void outcmod(void){
  hzfxpc("Constant:",9,11);
  hzfxpc(C->errflg+(HZ_MUC*3+12),3,0);
  hzfxpc(parms[9],8,-1);
  hzfxpf(H->c.muC,12,99,-6);
  hzfxpf(C->constant->muC,12,99,-2);
  ismuchg(H->c.muC,C->constant->muC,H->chgflg+HZ_MUC);
  hzfxpc(&H->chgflg[HZ_MUC],1,-1);
  hzfxpc("|",1,-2);
  hzfxpc(H->names[C->p+9],2,-2);
  hzfxpc("= Ln(muC)                ",25,-1);
  if(C->status[C->p+9]==0)
    hzfxpc("Yes #",5,-3);
  else
    hzfxpc("Yes  ",5,-3);
  hzfxpf(C->theta[C->p+9],12,99,-7);
  hzfskp(1);
}
