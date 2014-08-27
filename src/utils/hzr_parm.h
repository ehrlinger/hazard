#ifndef HZR_PARM_H
#define HZR_PARM_H
#include "common.h"

logical hzr_parm_is_fixed(short int parmno);
void hzr_parm_set_fixed(short int parmno);
void hzr_parm_changed(short int parmno); 
#endif /* HZR_PARM_H */
