#include "common.h"

logical hzr_parm_is_fixed(short int parmno){
  return (logical)(Common.status[parmno]==0);
}

void hzr_parm_set_fixed(short int parmno){
  Common.status[parmno] = 0;
}

void hzr_parm_changed(short int parmno){
  HZRstr.chgflg[parmno] = '*';
}
