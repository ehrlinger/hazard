#include <string.h>
#include "hazard.h"
#include <notspec.h>
#include <stmtfld.h>
#include <hzf_log1.h>
#include <stmtopts.h>


/****************************************************************/
/* PRZCONC called from GETCONC                                  */
 void przconc(void){

  /* Initialize the printout labels */
  memcpy(faze[k],phasenm, VAR_NAME_LENGTH);
  memcpy(varnam[k],stmtptr->name, VAR_NAME_LENGTH);
  if(notspec(2))
    stval[k] = ZERO;
  else
    stval[k] = stmtfld(2);
  if(notspec(3))
    nomovs[k] = H->mov;
  else {
    nomovs[k] = stmtfld(3);
    if(nomovs[k]>1 && O->nosw) {
      nomovs[k] = 1;
      sprintf(msgbfr,"Note: MOVE for %s in %s phase "
	      "reset to 1 due to NOSTEPWISE.",varnam[k],
	      faze[k]);
      hzf_log1(msgbfr);
    }
  }
  /*  I would realy like to enum these stmtopts parameters to 
      make them human legible.  */
  if(stmtopts(1))
    memcpy(inex[k],"EXCLUDE ",8);
  else if(stmtopts(2))
    memcpy(inex[k],"INCLUDE ",8);
  else if(stmtopts(3)) {
    memcpy(inex[k],"START   ",8);
    if(O->nosw && H->sw)
      nomovs[k] = 0;
  } else
    CLEAR(inex[k]);
  if(notspec(4))
    O->swordr[k] = O->ordmax;
  else
    if(*inex[k]!=' ') {
      sprintf(msgbfr,"ERROR: ORDER= and %s are mutually exclusive, "
	      "variable %s in phase %s\n.",inex[k],varnam[k],phasenm);
      hzf_log1(msgbfr);
      semerr = TRUE;
    } else {
      O->swordr[k] = stmtfld(4);
      O->orderv++;
    }
  k++;
}
