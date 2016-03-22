#include <string.h>
#include "hazard.h"
#include "vfynvar.h"
#include <stmtfldname.h>
#include <xvname.h>
#include <notspec.h>

/****************************************************************/
/* ICNSPRC called from STMTPRC                                  */
void icnsprc(void){
  if(notspec(14))
    return;
  C3_ns = xvname(stmtfldname(14));
  if(vfynvar(C3_ns,stmtfldname(14))) {  
    memcpy(c3name,C3_ns->nname, VAR_NAME_LENGTH);
    memcpy(c3labl,C3_ns->nlabel, LABEL_LENGTH);
  }
  CT_ns = xvname(stmtfldname(15));
  if(!vfynvar(CT_ns,stmtfldname(15)))
    return;
  memcpy(ctname,CT_ns->nname, VAR_NAME_LENGTH);
  memcpy(ctlabl,CT_ns->nlabel, LABEL_LENGTH);
}
