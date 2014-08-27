#include "hazard.h"
#include "outparm.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include "ismuchg.h"
#include <hzfskp.h>

void outlmod(void){
  short int len;

  outparm("Late:",HZ_TAU,4,H->l.tau,L->tau,"= Ln(Tau)");
  if(L->g_two || L->g3flag>=3 || C->status[7]==1)
    outparm("",HZ_GAMMA,5,H->l.gamma,L->gamma,"= Ln(Gamma)");
  else
    outparm("",HZ_GAMMA,5,H->l.gamma,L->gamma,"= Ln((Gamma*Eta)-2)");
  if((L->ga_two && !L->g_two) || L->g3flag>=3)
    outparm("",HZ_ALPHA,6,H->l.alpha,L->alpha,"= Ln(Alpha)");
  else
    outparm("",HZ_ALPHA,6,H->l.alpha,L->alpha,
	    "= Ln((Gamma*Eta/Alpha)-2)");
  if(L->g_two || L->g3flag>=3)
    outparm("",HZ_ETA,7,H->l.eta,L->eta,"= Ln(Eta)");
  else
    outparm("",HZ_ETA,7,H->l.eta,L->eta,"= Ln((Gamma*Eta)-2)");
  hzfxpc("         ",9,11);
  hzfxpc(C->errflg+(HZ_MUL*3+12),3,0);
  len = strlen(parms[HZ_MUL]);
  hzfxpc(parms[HZ_MUL],len,-1);
  hzfxpc("       ",8-len,0);
  hzfxpf(H->l.muL,12,99,-6);
  hzfxpf(L->muL,12,99,-2);
  ismuchg(H->l.muL,L->muL,H->chgflg+HZ_MUL);
  hzfxpc(&H->chgflg[HZ_MUL],1,-1);
  hzfxpc("|",1,-2);
  hzfxpc(H->names[2*C->p+10],2,-2);
  hzfxpc("= Ln(muL)                ",25,-1);
  if(C->status[2*C->p+10]==0)
    hzfxpc("Yes #",5,-3);
  else
    hzfxpc("Yes  ",5,-3);
  hzfxpf(C->theta[2*C->p+10],12,99,-7);
  hzfskp(1);
}
