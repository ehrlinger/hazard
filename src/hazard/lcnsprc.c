#include <string.h>
#include "hazard.h"
#include "vfynvar.h"
#include <xvname.h>
#include <stmtfldname.h>
#include <notspec.h>

/****************************************************************/
/* LCNSPRC called from STMTPRC                                  */
void lcnsprc(void){
  if(notspec(36))
    return;
  LC_ns = xvname(stmtfldname(36));
  if(!vfynvar(LC_ns,stmtfldname(36)))
    return;
  memcpy(lcname,LC_ns->nname, VAR_NAME_LENGTH);
  memcpy(lclabl,LC_ns->nlabel,LABEL_LENGTH);
}
