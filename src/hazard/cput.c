#include <string.h>
#include <hzfxpc.h>
#include <hzfxpf.h>
#include <hzfskp.h>

#include "hazard.h"

void cput(char *phasnm,int j){
  hzfxpc(phasnm,9,31);
  hzfxpc(risk[j],8,-1);
  hzfxpf(comean[j],12,99,-6);
  hzfxpf(conmin[j],12,99,-2);
  hzfxpf(conmax[j],12,99,-2);
  hzfskp(1);
}
