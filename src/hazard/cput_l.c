#include <string.h>
#include "hazard.h"
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>

void cput_l(char *phasnm,int j){
  hzfxpc(phasnm,9,10);
  hzfxpc(risk[j],8,-1);
  hzfxpc(H->labl[j+9],40,-6);
  hzfxpf(comean[j],12,99,-2);
  hzfxpf(conmin[j],12,99,-2);
  hzfxpf(conmax[j],12,99,-2);
  hzfskp(1);
}
