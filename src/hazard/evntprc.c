#include "hazard.h"
#include <string.h>

#include "vfynvar.h"
#include <notspec.h>
#include <stmtfldname.h>
#include <xvname.h>

/****************************************************************/
#include <string.h>
/* EVNTPRC called from STMTPRC                                  */
void evntprc(void){
  if(notspec(12))
    return;
  C1_ns = xvname(stmtfldname(12));
  if(!vfynvar(C1_ns,stmtfldname(12)))
    return;
  memcpy(c1name,C1_ns->nname,VAR_NAME_LENGTH);
  memcpy(c1labl,C1_ns->nlabel,LABEL_LENGTH);

}
