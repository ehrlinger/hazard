#include <string.h>
#include "hazard.h"
#include "vfynvar.h"
#include "notspec.h"
#include "xvname.h"
#include "stmtfldname.h"

/****************************************************************/
/* RCNSPRC called from STMTPRC                                  */
void rcnsprc(void){
  if(notspec(13))
    return;
  C2_ns = xvname(stmtfldname(13));
  if(!vfynvar(C2_ns,stmtfldname(13)))
    return;
  memcpy(c2name,C2_ns->nname, VAR_NAME_LENGTH);
  memcpy(c2labl,C2_ns->nlabel,LABEL_LENGTH);

}
