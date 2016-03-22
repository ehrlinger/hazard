#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfxpi.h>
#include <hzfxpf.h>
#include <hzfskp.h>

void vput_dl(char *spcnam,char *varnam,char *label,int misval,int delval,
	     double minval,double maxval,size_t spclen){
  char spcprt[18];

  if(*varnam==' ')
    return;
  CLEAR(spcprt);
  memcpy(spcprt,spcnam,spclen);
  hzfxpc(spcprt,18,5);
  hzfxpc(varnam,8,-3);
  hzfxpc(label,40,-3);
  hzfxpi(misval,5,-2);
  hzfxpi(delval,5,-5);
  if(minval!=Machine->absMax || maxval!=Machine->absMin) {
    hzfxpf(minval,12,99,-9);
    hzfxpf(maxval,12,99,-2);
  }
  hzfskp(1);
}
