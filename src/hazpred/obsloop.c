#include "hazpred.h"
#include "common.h"
#include "xoget.h"
#include "xvget.h"
#include "xvput.h"
#include "hzfmis.h"
#include <string.h>

#include "hzf_log1.h"
#include "hzpo.h"
#include "hzdinc.h"
void obsloop(void){
  long i;

  C->errorno = 0;
  xoget(); xvget();
  for(i=0; i<=C->p; i++)
    if(ISMISS(C->obs[i])) {
      MKMISS(H->Surviv); MKMISS(H->CL_L_S); MKMISS(H->CL_U_S);
      MKMISS(H->earlyS); MKMISS(H->constS); MKMISS(H->lateS);
      MKMISS(H->Hazard); MKMISS(H->CL_L_H); MKMISS(H->CL_U_H);
      MKMISS(H->earlyH); MKMISS(H->constH); MKMISS(H->lateH);
      xvput();
      return;
    }
  if(setjmp(C->errtrap))
    errlast = TRUE;
  else if(!gotID)
    hzpo(Chgd_Both,!H->noCL);
  else {
    if(!errlast && !memcmp(id_curr,id_prev,idleng))
      hzpo(Chgd_Time,!H->noCL);
    else
      hzpo(Chgd_Both,!H->noCL);
    memcpy(id_prev,id_curr,idleng);
    errlast = FALSE;
  }
  if(C->errorno==1) {
    sprintf(msgbfr,"ERROR: In observation %ld, a computation exceeded "
	    "its limits %12.12s.\n",obscnt,C->errflg);
    hzf_log1(msgbfr);
    hzf_log1("Note: Processing will continue.");
  } else if(C->errorno==2) {
    sprintf(msgbfr,"ERROR: In observation %ld, internal sign violation, "
	    "possibly data dependent %12.12s.\n",obscnt,C->errflg);
    hzf_log1(msgbfr);
    hzf_log1("Note: Processing will continue.");
  }
  if((C->errorno!=0 && C->errorno!=3) || !H->Sflag) {
    MKMISS(H->Surviv); MKMISS(H->CL_L_S); MKMISS(H->CL_U_S);
    MKMISS(H->earlyS); MKMISS(H->constS); MKMISS(H->lateS);
  }
  if((C->errorno!=0 && C->errorno!=3) || !H->Hflag) {
    MKMISS(H->Hazard); MKMISS(H->CL_L_H); MKMISS(H->CL_U_H);
    MKMISS(H->earlyH); MKMISS(H->constH); MKMISS(H->lateH);
  }
  if(H->noCL) {
    MKMISS(H->CL_L_S); MKMISS(H->CL_U_S);
    MKMISS(H->CL_L_H); MKMISS(H->CL_U_H);
  }
  xvput();
}
