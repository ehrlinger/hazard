#include <string.h>
#include "hazpred.h"
#include "xvputd.h"

void addvars(void){
  struct namestr oxns;

  memset(&oxns,0,sizeof(oxns));
  oxns.ntype = 1;
  oxns.nlng = 8;
  memset(oxns.nlabel,' ',LABEL_LENGTH);
  memset(oxns.nform,' ',VAR_NAME_LENGTH);
  memset(oxns.niform,' ',VAR_NAME_LENGTH);
  memset(oxns.nname,' ',VAR_NAME_LENGTH);  
  memcpy(oxns.nname,"_SURVIV ",8);
  xvputd(&oxns,&H->Surviv);
  memcpy(oxns.nname,"_CLLSURV",8);
  xvputd(&oxns,&H->CL_L_S);
  memcpy(oxns.nname,"_CLUSURV",8);
  xvputd(&oxns,&H->CL_U_S);
  memcpy(oxns.nname,"_EARLYS ",8);
  xvputd(&oxns,&H->earlyS);
  memcpy(oxns.nname,"_CONSTS ",8);
  xvputd(&oxns,&H->constS);
  memcpy(oxns.nname,"_LATES  ",8);
  xvputd(&oxns,&H->lateS);
  memcpy(oxns.nname,"_HAZARD ",8);
  xvputd(&oxns,&H->Hazard);
  memcpy(oxns.nname,"_CLLHAZ ",8);
  xvputd(&oxns,&H->CL_L_H);
  memcpy(oxns.nname,"_CLUHAZ ",8);
  xvputd(&oxns,&H->CL_U_H);
  memcpy(oxns.nname,"_EARLYH ",8);
  xvputd(&oxns,&H->earlyH);
  memcpy(oxns.nname,"_CONSTH ",8);
  xvputd(&oxns,&H->constH);
  memcpy(oxns.nname,"_LATEH  ",8);
  xvputd(&oxns,&H->lateH);
}
