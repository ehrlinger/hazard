#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfxpi.h>
#include <hzfskp.h>

void cput_dl(char *phasnm,int j){
  hzfxpc(phasnm,9,5);
  hzfxpc(risk[j],8,-1);
  hzfxpc(H->labl[j+9],40,-6);
  hzfxpi(conmis[j],5,-2);
  hzfxpf(comean[j],12,99,-4);
  hzfxpf(conmin[j],12,99,-2);
  hzfxpf(conmax[j],12,99,-2);
  hzfskp(1);
}
