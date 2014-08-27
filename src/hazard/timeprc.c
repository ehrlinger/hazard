#include <string.h>
#include <common.h>
#include "hazard.h"
#include "vfynvar.h"
#include <notspec.h>
#include <stmtfldname.h>
#include <xvname.h>

/****************************************************************/
/* TIMEPRC called from STMTPRC                                  */
void timeprc(void){

  if(notspec(11))
    return;
  T_ns = xvname(stmtfldname(11));
  if(!vfynvar(T_ns,stmtfldname(11)))
    return;
  memcpy(tname,T_ns->nname,VAR_NAME_LENGTH);
  memcpy(tlabl,T_ns->nlabel,LABEL_LENGTH);

}
