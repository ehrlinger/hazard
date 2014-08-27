#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>

void vput(char *spcnam,char *varnam,double minval,double maxval,
	  size_t spclen){
  char spcprt[18];

  if(*varnam==' ')
    return;
  CLEAR(spcprt);
  memcpy(spcprt,spcnam,spclen);
  hzfxpc(spcprt,18,37);
  hzfxpc(varnam,8,-3);
  if(minval!=Machine->absMax || maxval!=Machine->absMin) {
    hzfxpf(minval,12,99,-3);
    hzfxpf(maxval,12,99,-3);
  }
  hzfskp(1);
}
