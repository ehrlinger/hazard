#include <string.h>
#include <hzdinc.h>
#include "hazard.h"

/****************************************************************/
/* SETRISK called from main                                     */
void setrisk(void){
  int j;
  struct namestr *nsj;

  for(j=0; j<11+3*C->p; j++) {
    CLEAR(H->names[j]);
    names_ns[j] = NULL;
    CLEAR(H->labl[j]);
  }
  memcpy(H->names[0],"E1      ",8);
  memcpy(H->names[1],"E2      ",8);
  memcpy(H->names[2],"E3      ",8);
  memcpy(H->names[3],"E4      ",8);
  memcpy(H->names[4],"L1      ",8);
  memcpy(H->names[5],"L2      ",8);
  memcpy(H->names[6],"L3      ",8);
  memcpy(H->names[7],"L4      ",8);
  memcpy(H->names[8],"E0      ",8);
  memcpy(H->names[C->p+9],"C0      ",8);
  memcpy(H->names[2*C->p+10],"L0      ",8);
  for(j=0; j<C->p; j++) {
    memcpy(H->names[j+9],risk[j],VAR_NAME_LENGTH);
    memcpy(H->names[j+C->p+10],risk[j],VAR_NAME_LENGTH);
    memcpy(H->names[j+2*C->p+11],risk[j],VAR_NAME_LENGTH);
    nsj = risk_ns[j];
    names_ns[j+9] = nsj;
    names_ns[j+C->p+10] = nsj;
    names_ns[j+2*C->p+11] = nsj;
    memcpy(H->labl[j+9],nsj->nlabel,LABEL_LENGTH);
    memcpy(H->labl[j+C->p+10],nsj->nlabel,LABEL_LENGTH);
    memcpy(H->labl[j+2*C->p+11],nsj->nlabel,LABEL_LENGTH);
  }
  pp7 = C->p+7;
  C->Ntheta = 11+3*C->p;
  if(C->phase[1]==1)
    C->status[8] = 1;
  if(C->phase[2]==1)
    C->status[C->p+9] = 1;
  if(C->phase[3]==1)
    C->status[2*C->p+10] = 1;
}
