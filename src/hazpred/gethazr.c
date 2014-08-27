#include "hazpred.h"
#include "hzdinc.h"

#include "defvars.h"
#include "hazverr.h"
#include "xoget.h"
#include "xvget.h"
#include "namfix.h"

#include "fmtverr.h"
#include "prederr.h"
#include "getflag.h"
#include "hzpe.h"
#include "hzpp.h"
#include <string.h>



void gethazr(void){
  long i,namidx,k;
  double *jp,*kp;

  xvgetptr = inhaz_xv;
  xvgetcnt = 0;
  xvobslen = inhaz_ln;
  inputDataFile = hazfile;
  ns = inhaz_ns;
  numvars = nvars;
  defvars();
  E->g1flag = getflag("G1FLAG  ",1);
  E->delta0 = getflag("FIXDEL0 ",2);
  E->mNuOne = getflag("FIXMNU1 ",3);
  L->g3flag = getflag("G3FLAG  ",4);
  L->g_two  = getflag("FIXGE2  ",5);
  L->ga_two = getflag("FIXGAE2 ",6);
  for(i=0; i<C->Ntheta; i++) {
    xoget(); xvget();
    memcpy(names[i],in_name,VAR_NAME_LENGTH);
    C->theta[i] = in_est;
    C->status[i] = in_status;
    if(!H->noCL)
      for(kp=C->obs,jp=C->cov+i*C->Ntheta,k=0; k<C->Ntheta; k++)
	*jp++ = *kp++;
  }
  if(!H->noCL)
    for(i=0; i<C->Ntheta; i++)
      if(memcmp(names[i],namev[i],VAR_NAME_LENGTH))
	fmtverr(i,names[i],namev[i]);
  if(memcmp(names[C->p+9],"C0      ",8))
    hazverr(C->p+13);
  if(memcmp(names[2*C->p+10],"L0      ",8))
    hazverr(2*C->p+14);
  for(i=C->p+10,namidx=1; i<2*C->p+10; i++,namidx++) {
    if(namidx<10)
      memcpy(in_name,"C0      ",8);
    else
      memcpy(in_name,"C       ",8);
    namfix(in_name,namidx);
    if(memcmp(in_name,names[i],VAR_NAME_LENGTH))
      hazverr(i+3);
  }
  for(i=2*C->p+11,namidx=1; i<3*C->p+11; i++,namidx++) {
    if(namidx<10)
      memcpy(in_name,"L0      ",8);
    else
      memcpy(in_name,"L       ",8);
    namfix(in_name,namidx);
    if(memcmp(in_name,names[i],VAR_NAME_LENGTH))
      hazverr(i+3);
  }
  if(!setjmp(C->errtrap))
    hzpe();
  else
    prederr();
  hzpp();
}
