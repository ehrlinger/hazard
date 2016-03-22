#include <string.h>
#include <hzfxpc.h>
#include <hzfskp.h>

void outequn(char *equnam){
  short int len;

  hzfxpc("|",1,11);
  len = strlen(equnam);
  hzfxpc(equnam,len,-8);
  hzfxpc("                                         ",41-len,0);
  hzfxpc("|",1,0);
  hzfskp(1);
}
