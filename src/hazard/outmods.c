#include "hazard.h"
#include "outemod.h"
#include "outcmod.h"
#include "outlmod.h"
#include "outflgs.h"
#include "cmpmeth.h"
#include "modterm.h"
#include <hzfpag.h>
#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzfxpr.h>


void outmods(void){
  logical iflag;
  long j,k,ctr;

  hzfpag(40);
  hzfskp(2);
  hzfxpc("Model Specifications:",21,5);
  hzfskp(1);
  hzfxpc("=====================",21,5);
  hzfskp(2);
  hzfxpc("Initial Parameter Values",24,40);
  hzfskp(1);
  hzfxpc("------------------------",24,40);
  hzfxpc("Starting",8,-52);
  hzfskp(1);
  hzfxpc("Phase",5,11);
  hzfxpc("Parameter",9,-8);
  hzfxpc("Specified",9,-7);
  hzfxpc("Used",4,-8);
  hzfxpc("|",1,-7);
  hzfxpc("Theta",5,-2);
  hzfxpc("Estimated?",10,-26);
  hzfxpc("Value",5,-4);
  hzfskp(1);
  hzfxpr('-',115,11);
  hzfskp(1);
  if(C->phase[1]==1)
    outemod();
  if(C->phase[1]==1 && (C->phase[2]==1 ||
			C->phase[3]==1)) {
    hzfxpr('-',102,24);
    hzfskp(1);
  }
  if(C->phase[2]==1)
    outcmod();
  if(C->phase[2]==1 && C->phase[3]==1) {
    hzfxpr('-',102,24);
    hzfskp(1);
  }
  if(C->phase[3]==1)
    outlmod();
  hzfxpr('-',115,11);
  hzfskp(1);
  if(H->mode==0) {
    hzfxpc("# This MU parameter is estimated in closed",42,5);
    hzfxpc(" form.",6,0);
    hzfskp(1);
  }
  iflag = FALSE;
  for(k=0; k<12 && !iflag; k++)
    if(H->chgflg[k]!=' ')
      iflag = TRUE;
  if(iflag) {
    hzfxpc("* The value used is different from the",38,5);
    hzfxpc(" value specified.",17,0);
    hzfskp(1);
  }
  for(j=12,iflag=FALSE; j<48 && !iflag; j++)
    if(C->errflg[j]!=' ')
      iflag = TRUE;
  if(!iflag) {
    outflgs();
    cmpmeth();
    ctr = 0;
    for(j=0; j<C->Ntheta; j++)
      if(C->status[j]==1)
	ctr++;
    if(H->mode==0 || (H->mode==1 && ctr==1)) {
      hzfxpc("If only one scaling parameter,",30,7);
      hzfxpc(" estimation is in closed form.",30,0);
      hzfskp(3);
    } else
      hzfskp(2);
  }
  if((C->phase[1]==0 && C->phase[2]==0 && C->phase[3]==0) ||
     C->errorno!=0 || H->ierr!=0 || iflag)
    modterm(iflag);
}
