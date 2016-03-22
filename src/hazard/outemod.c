#include <string.h>
#include "hazard.h"
#include "ismuchg.h"
#include "outparm.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>

void outemod(void){
  short int len;

  outparm("Early:",HZ_DELTA,0,H->e.delta,E->delta,"= Ln(-Ln(|Delta|))");
  outparm("",HZ_THALF,1,H->e.tHalf,E->tHalf,"= Ln(Thalf)");
  hzfxpc((C->errflg+(HZ_RHO*3+12)),3,21);
  hzfxpc("(RHO",4,-1);
  hzfxpf(E->rho,12,99,-23);
  hzfxpc(")",1,0);
  hzfxpc("|",1,-3);
  hzfskp(1);
  if(E->mNuOne) {
    outparm("",HZ_NU,2,H->e.nu,E->nu,"= Ln(|M*Nu|)");
    outparm("",HZ_M,3,H->e.m,E->m,"= Ln(|M*Nu|)");
  } else {
    outparm("",HZ_NU,2,H->e.nu,E->nu,"= Ln(|Nu|)");
    outparm("",HZ_M,3,H->e.m,E->m,"= Ln(|M|)");
  }
  hzfxpc("         ",9,11);
  hzfxpc(C->errflg+(HZ_MUE*3+12),3,0);
  len = strlen(parms[HZ_MUE]);
  hzfxpc(parms[HZ_MUE],len,-1);
  hzfxpc("       ",8-len,0);
  hzfxpf(H->e.muE,12,99,-6);
  hzfxpf(E->muE,12,99,-2);
  ismuchg(H->e.muE,E->muE,H->chgflg+HZ_MUE);
  hzfxpc(&H->chgflg[HZ_MUE],1,-1);
  hzfxpc("|",1,-2);
  hzfxpc(H->names[8],2,-2);
  hzfxpc("= Ln(muE)                ",25,-1);
  if(C->status[8]==0)
    hzfxpc("Yes #",5,-3);
  else
    hzfxpc("Yes  ",5,-3);
  hzfxpf(C->theta[8],12,99,-7);
  hzfskp(1);
}
