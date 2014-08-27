#include "hazard.h"
#include <hzfxpc.h>
#include <hzfskp.h>

void outflag(char *flgnam,logical value){
  short int len;

  hzfxpc("|",1,11);
  len = strlen(flgnam);
  hzfxpc(flgnam,len,-8);
  hzfxpc("                      ",22-len,0);
  if(value)
    hzfxpc("Yes",3,-3);
  else
    hzfxpc("No ",3,-3);
  hzfxpc("|",1,-13);
  hzfskp(1);
}
