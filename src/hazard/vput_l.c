#include <string.h>
#include "hazard.h"

#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>

void vput_l(char *spcnam,char *varnam,char *label,double minval,
	    double maxval, size_t spclen){
  char spcprt[18];

  if(*varnam==' ')
    return;
  CLEAR(spcprt);
  memcpy(spcprt,spcnam,spclen);
  hzfxpc(spcprt,18,17);
  hzfxpc(varnam,8,-3);
  hzfxpc(label,40,-3);
  if(minval!=Machine->absMax || maxval!=Machine->absMin) {
    hzfxpf(minval,12,99,-2);
    hzfxpf(maxval,12,99,-3);
  }
  hzfskp(1);
}
