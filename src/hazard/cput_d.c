#include <string.h>
#include <hzfxpc.h>
#include <hzfxpi.h>
#include <hzfxpf.h>
#include <hzfskp.h>
#include "hazard.h"

void cput_d(char *phasnm,int j){
  hzfxpc(phasnm,9,26);
  hzfxpc(risk[j],8,-1);
  hzfxpi(conmis[j],5,-6);
  hzfxpf(comean[j],12,99,-4);
  hzfxpf(conmin[j],12,99,-2);
  hzfxpf(conmax[j],12,99,-2);
  hzfskp(1);
}
