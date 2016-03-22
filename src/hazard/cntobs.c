#include <string.h>
#include "hazard.h"
#include <stmtfld.h>

/****************************************************************/
/* CNTOBS called from main                                      */
void cntobs(void){
  C->Nobs = stmtfld(43);
}
