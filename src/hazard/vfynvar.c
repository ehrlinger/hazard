#include <string.h>
#include "hazard.h"
#include <hzf_log2.h>

/****************************************************************/
/* VFYNVAR called from TIMEPRC, EVENTPRC, RCNSPRC, ICNSPRC,     */
/*          WGHTPRC, and GETRISK                                */
long vfynvar(struct namestr *v,char *n){
  char vn[VAR_NAME_LENGTH + 1];

  vn[VAR_NAME_LENGTH] = '\0';
  if(v==NULL) {
    memcpy(vn,n,VAR_NAME_LENGTH);
    hzf_log2("ERROR: VARIABLE NOT FOUND - ",vn);
    semerr = TRUE;
    return 0;
  }
  if(v->ntype!=1) {
    memcpy(vn,n,VAR_NAME_LENGTH);
    hzf_log2("ERROR: VARIABLE NOT NUMERIC - ",vn);
    semerr = TRUE;
    return 0;
  }
  return 1;
}
