#include <string.h>
#include "hazard.h"
#include <stmtfldname.h>
#include "vfynvar.h"
#include <xvname.h>
#include <notspec.h>

/****************************************************************/
/* WGHTPRC called from STMTPRC                                  */
void wghtprc(void){
  if(notspec(37))
    return;
  WT_ns = xvname(stmtfldname(37));
  if(!vfynvar(WT_ns,stmtfldname(37)))
    return;
  memcpy(wtname,WT_ns->nname, VAR_NAME_LENGTH);
  memcpy(wtlabl,WT_ns->nlabel, LABEL_LENGTH);
}
