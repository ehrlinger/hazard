#include <string.h>
#include "hazard.h"
#include <xvname.h>
#include <stmtfld.h>
#include <stmtopts.h>
#include <notspec.h>

/****************************************************************/
/* SETPRMF called from PARMPRC                                  */
void setprmf(int parmno,int ispecno,long statno,long optno,
	     double *fixpvar){
  if(notspec(parmno))
    ispec(ispecno) = 2;
  else {
    ispec(ispecno) = 1;
    *fixpvar = stmtfld(parmno);
  }
  /*  I would realy like to enum these stmtopts parameters to 
      make them human legible.  */
  if(stmtopts(optno))
    t_status(statno) = 0;
}
