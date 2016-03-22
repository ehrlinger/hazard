#include <string.h>
#include "hazard.h"
#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzfxpr.h>
#include "outflag.h"
#include "outequn.h"

void outflgs(void){
  hzfskp(1);
  hzfxpc("Based on the values used, the following",39,5);
  hzfxpc(" flags and equations have been set:",35,0);
  hzfskp(1);
  hzfxpc("*",1,11);
  hzfxpr('-',49,0);
  hzfxpc("*",1,0);
  hzfskp(1);
  if(C->phase[1]==1) {
    hzfxpc("|",1,11);
    hzfxpc("Early:",6,-2);
    hzfxpc("|",1,-41);
    hzfskp(1);
    outflag("Delta = 0",E->delta0);
    outflag("M*Nu = |1|",E->mNuOne);
    if(E->g1flag==1)
      outequn("M>0, Nu>0, Generic equations 6 - 8");
    else if(E->g1flag==2)
      outequn("M=0, Nu>0, Exponential equations 9 - 11");
    else if(E->g1flag==3)
      outequn("M<0, Nu>0, Generic equations 12 - 14");
    else if(E->g1flag==4)
      outequn("M<0, Nu=0, Exponential equations 15 - 17");
    else if(E->g1flag==5)
      outequn("M>0, Nu<0, Generic equations 18 - 20");
    else if(E->g1flag==6)
      outequn("M=0, Nu<0, Exponential equations 21 - 23");
  }
  if(C->phase[2]==1) {
    hzfxpc("|",1,11);
    hzfxpc("Constant:",9,-2);
    hzfxpc("|",1,-38);
    hzfskp(1);
    outequn("Equations 24 - 25");
  }
  if(C->phase[3]==1) {
    hzfxpc("|",1,11);
    hzfxpc("Late: ",5,-2);
    hzfxpc("|",1,-4);
    outflag("Gamma*Eta = 2",L->g_two);
    outflag("Gamma*Eta/Alpha = 2",L->ga_two);
    outflag("Weibull",H->weibul);
    if(L->g3flag==1 || L->g3flag==3)
      outequn("Alpha>0, Generic equations 26 - 27");
    else if(L->g3flag==2 || L->g3flag==4)
      outequn("Alpha=0, Exponential equations 32 - 33");
  }
  hzfxpc("*",1,11);
  hzfxpr('-',49,0);
  hzfxpc("*",1,0);
  hzfskp(1);
}
