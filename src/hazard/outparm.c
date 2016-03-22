#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>

void outparm(char *phase,int chgno,int parmno,double spcval,
	     double useval,char *estnam){
  short int len;

  len = strlen(phase);
  hzfxpc(phase,len,11);
  hzfxpc("         ",9-len,0);
  hzfxpc(C->errflg+(chgno*3+12),3,0);
  len = strlen(parms[chgno]);
  hzfxpc(parms[chgno],len,-1);
  hzfxpc("       ",8-len,0);
  hzfxpf(spcval,12,99,-6);
  hzfxpf(useval,12,99,-2);
  hzfxpc(&H->chgflg[chgno],1,-1);
  hzfxpc("|",1,-2);
  hzfxpc(H->names[parmno],2,-2);
  if(C->status[parmno]==0) {
    hzfxpc(" ",1,-1);
    hzfxpc("No",2,-27);
  } else {
    len = strlen(estnam);
    hzfxpc(estnam,len,-1);
    hzfxpc("                          ",26-len,0);
    hzfxpc("Yes  ",5,-2);
    hzfxpf(C->theta[parmno],12,99,-7);
  }
  hzfskp(1);
}
