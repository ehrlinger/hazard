#include <string.h>
#include <limits.h>
#include "hazard.h"
#include <hzdinc.h>

#include <stmtopts.h>
#include <notspec.h>
#include <stmtfld.h>
#include <hzf_log1.h>
#include <hzfxit.h>

/****************************************************************/
/* STPWPRC called from STMTPRC                                  */
void stpwprc(void){
  /*  I would realy like to enum these stmtopts parameters to 
      make them human legible.  */
  if(stmtopts(21) || stmtopts(33))
    H->sw = 1;
  if(stmtopts(22)) {
    H->sw = 0;
    H->bw = 1;
  }
  if(stmtopts(23))
    H->f = 1;
  if(notspec(31)) {
    ispec(14) = 2;
    H->sle = 0.3e0;
  } else {
    ispec(14) = 1;
    temp_fl = stmtfld(31);
    if(temp_fl<=ZERO || temp_fl>=ONE)
      H->sle = 0.3e0;
    else
      H->sle = temp_fl;
  }
  if(notspec(32))
    ispec(15) = 0;
  else {
    ispec(15) = 1;
    temp_fl = stmtfld(32);
    if(temp_fl<=ZERO || temp_fl>=ONE)
      ispec(15) = 0;
    else
      H->sls = temp_fl;
  }
  if(stmtopts(34))
    O->nosw = 1;
  if(notspec(33))
    ispec(16) = 2;
  else {
    ispec(16) = 1;
    if(stmtfld(16)>=ZERO)
      H->mov = stmtfld(33);
    if(H->mov>1 && O->nosw) {
      H->mov = 1;
      hzf_log1("Note: NOSTEPWISE overrides MOVE= "
	       "specification, now set to 1.");
    }
  }
  if(stmtopts(32)) {
    ispec(19) = 1;
    swnewt = FALSE;
    swhess = TRUE;
  }
  if(stmtopts(31)) {
    ispec(20) = 1;
    swnewt = FALSE;
    swhess = FALSE;
  }
  if(stmtopts(24))
    H->nps = 1;
  if(stmtopts(25))
    H->npq = 1;
  if(notspec(34))
    H->mxstep = LONG_MAX;
  else {
    temp_fl = stmtfld(34);
    if(temp_fl<ZERO) {
      hzf_log1("ERROR: NEGATIVE MAXSTEPS SPECIFIED.");
      hzfxit("SEMANTIC");
    } else
      H->mxstep = temp_fl;
  }
  if(notspec(35))
    H->mxvars = 0;
  else {
    temp_fl = stmtfld(35);
    if(temp_fl<=ZERO) {
      hzf_log1("ERROR: NON-POSITIVE MAXVARS SPECIFIED.");
      hzfxit("SEMANTIC");
    }
    H->mxvars = temp_fl;
  }
}
