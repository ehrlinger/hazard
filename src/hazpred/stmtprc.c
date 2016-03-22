#include <string.h>
#include "hazpred.h"
#include "common.h"

#include "hzdinc.h" /* For CLEAR define */
#include "hazpprc.h"
#include "hzf_log1.h"
#include "hzfxit.h"
#include "timeprc.h"
#include "idproc.h"

void stmtprc(void){
  termin = gotID = FALSE;
  H->CLimit = ZERO;
  H->noCL = FALSE;
  H->Sflag = TRUE; H->Hflag = TRUE;
  CLEAR(tname);
  hazpprc();
  timeprc();
  idproc();
  if(!H->Sflag && !H->Hflag) {
    hzf_log1("ERROR: Neither survivorship nor hazard functions "
	     "were selected.");
    termin = TRUE;
  }
  if(termin)
    hzfxit("SEMANTIC");
}
