#include "hazard.h"
#include <string.h>
#include <hzdinc.h>
#include <hzf_memget.h>
#include <hzf_log1.h>
#include <hzfxit.h>
#include "przconc.h"
/****************************************************************/
/* GETCONC called from main                                      */
void getconc(void){
  long j = 0;

  /* Initialize some variables to 0 */
  k = 0;                 /* A global counter variable */

  /* O is redirected to H->opt in hazard.c*/
  O->orderv = 0;
  O->restct = 0;
  O->bdlvar = 0;
  O->bdlcnt = 0;
  O->ordmax = Machine->absMax;
  
  /* */
  stmtptr = stmtptr->next;

  /* Count the number of analysis variables required */
  while(stmtptr!=NULL) {
    switch(stmtptr->size){
    case 10: 
      k++;
      break;
    case 5:
      O->bdlcnt++;
      *phasenm = 'B';
      break;
    case 1:
      *phasenm = *stmtptr->name;
      break;
    case 2:
      if(*phasenm=='B'){
	O->bdlvar++;
      }else if(*phasenm=='R'){
	O->restct++;
      }
      break;
    default:
      break;
    }
    stmtptr = stmtptr->next;
  }
  stmtptr = anchor;
  
  /* Allocate memory based on the previous counts */
  risk = hzf_memget(k*VAR_NAME_LENGTH);
  faze = hzf_memget(k*VAR_NAME_LENGTH);
  varnam = hzf_memget(k*VAR_NAME_LENGTH);
  inex = hzf_memget(k*VAR_NAME_LENGTH);
  stval = hzf_memget(k*sizeof(double));
  O->swordr = hzf_memget(k*sizeof(double));
  nomovs = hzf_memget(k*sizeof(long));
  risk_ns = hzf_memget(k*sizeof(struct namestr *));
  if(O->restct>0){
    O->rsttbl = hzf_memget(O->restct*sizeof(struct rsttbl));
  }
  if(hzf_mem_need>0) {
    sprintf(msgbfr,"ERROR: Insufficient memory for Stage I; "
	    "need %ldK additional storage",hzf_mem_need/1024);
    hzf_log1(msgbfr);
    hzfxit("MEMORY");
  }
  O->restct = 0;
  k = 0;
  stmtptr = stmtptr->next;
 
  while(stmtptr!=NULL) {
    switch(stmtptr->size){
    case 10:
      /* Initialize the printout labels */
      przconc();
      break;
    case 5:
      *phasenm = 'B';
      break;
    case 1:
      memcpy(phasenm,stmtptr->name, VAR_NAME_LENGTH);
      j = 0;
      break;
    case 2:
      if(*phasenm=='R') {
	/* Deal with restricted variables */
	memcpy(O->rsttbl[O->restct].rstnam,stmtptr->name, VAR_NAME_LENGTH);
	O->rsttbl[O->restct].rstctr = ++j;
	O->restct++;
      }
      break;
    default:
      break;
    }
    stmtptr = stmtptr->next;
  }
  stmtptr = anchor;
  if(O->orderv!=0 && (H->bw==1 || H->sw==0)) {
    hzf_log1("Note: ORDER= parms are ignored unless "
	     "Forward Selection is requested.");
    O->orderv = 0;
  }
  if(O->restct!=0 && H->bw==0 && H->sw==0 && H->f==0) {
    hzf_log1("Note: Restriction statement(s) are ignored "
	     "unless Selection is requested.");
    O->restct = 0;
  }
  if(O->bdlcnt!=0) {
    hzf_log1("Note: BUNDLE statements are accepted, "
	     "but not yet functional.");
    O->bdlcnt = 0;
  }
}
