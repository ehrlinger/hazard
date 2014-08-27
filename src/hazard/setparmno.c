#include "hazard.h"
#include <notspec.h>
#include <stmtfld.h>

/****************************************************************/
/* SETPARMNO called from PARMPRC                                  */
void setparmno(long parmno,long ispecno,long phaseno,double *fixpvar){
  if(notspec(parmno))
    ispec(ispecno) = 2;
  else if(stmtfld(parmno)>ZERO) {
    ispec(ispecno) = 1;
    *fixpvar = stmtfld(parmno);
    C->phase[phaseno] = 1;
  }
}
