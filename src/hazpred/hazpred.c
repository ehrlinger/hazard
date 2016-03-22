#include <string.h>
/**
   The hazpred program has about 2900 lines of code 

   REVISIONS

   Hazpred-4.1.0 (2000)
   Start to comment and recast the code into standard C format
   using GNU coding standards. Divide monolithic files into
   subroutine size files, and recast the global variables into 
   usable groupings. JE

     HZP4.035a1 (7/26/95)
         Added support for NOLOG/NONOTES.

**/
#include <stdlib.h>
#include <string.h>
/* Define STRUCT to alloc storage for structures.h */
#define STRUCT
#define GLOBAL_DEFS /* hzdinc.h storage alloc */
#include "hzdinc.h"

#include "xinit.h"
#include "xoget.h"
#include "xvget.h"
#include "xvgetd.h"
#include "xvputd.h"
#include "xvname.h"
#include "hzf_log1.h"
#include "hzfxit.h"
#include "xvpute.h"

/*
#include "stmtopts.h"
#include "xlmode.h"
#include "stmtfldname.h"
#include "hzf_memget.h"

#include "notspec.h"
#include "stmtfld.h"
#include "xvputi.h"
#include "namfix.h"
#include "hzfmis.h"
#include "xvput.h"
*/
#define HAZPRED         /* To exclude hazard only functions */
#include "common.h"
#include "structures.h"
#include "hazpred.h"
#include "common.h"

#include "initprz.h"
#include "opnfils.h"
#include "stmtprc.h"
#include "hazpprc.h"
#include "cntobsi.h"
#include "cntobsh.h"
#include "alocmem.h"
#include "gethazr.h"
#include "addvars.h"
#include "obsloop.h"

#include "hzpi.h"
#include "hzpm.h"

int main(void){
  int i,j;
  struct namestr *k;

  curr_xlmode=2;

  C = hzpi();
  H = C->hzpstr;
  E = C->early;
  L = C->late;
  Machine = C->machn; /*added by dale 10/30/97 */
  xinit();
  initprz();
  opnfils();
  stmtprc();
  cntobsh();
  if(!setjmp(C->errtrap))
    hzpm(totalobs);
  else if(C->errorno) {
    hzf_log1("ERROR: INHAZ data set has invalid observation count.");
    hzfxit("DATA");
  }
  if((C->Ntheta+3)!=nvars){
    if(nvars==3){
      H->noCL = TRUE;
    }else {
      hzf_log1("ERROR: INHAZ data set has incorrect number of variables.");
      hzfxit("DATA");
    }
  }
  cntobsi();
  alocmem();
  gethazr();
  termin = FALSE;
  xvgetptr = data_xv;
  xvgetcnt = 0;
  xvobslen = data_ln;
  inputDataFile = infile;
  ns = data_ns;
  numvars = infilect;
  buftyp[T_ns->nvar0-1] = 0;
  xvgetd(T_ns,&C->obs[0]);
  out_ns[0] = *T_ns;
  xvputd(&out_ns[0],&C->obs[0]);
  if(gotID)
    xvgetd(ID_ns,id_curr);
  for(i=9,j=1; i<C->p+9; i++) {
    k = xvname(names[i]);
    if(k==NULL) {
      sprintf(msgbfr,"ERROR: Concomitant variable %8.8s is not "
	      "in the input data set.",names[i]);
      hzf_log1(msgbfr);
      termin = TRUE;
    } else {
      if(k->ntype!=1) {
	sprintf(msgbfr,"ERROR: Concomitant variable %8.8s is "
		"not numeric.",names[i]);
	hzf_log1(msgbfr);
	termin = TRUE;
      }
      buftyp[k->nvar0-1] = 0;
      xvgetd(k,&C->obs[j]);
      out_ns[j] = *k;
      xvputd(&out_ns[j],&C->obs[j]);
      j = j+1;
    }
  }
  if(termin)
    hzfxit("DATA");
  for(i=0; i<infilect; i++)
    if(buftyp[i]!=0) {
      xvgetd(&ns[i],buffer[i]);
      out_ns[j] = ns[i];
      xvputd(&out_ns[j],buffer[i]);
      j = j+1;
    }
  addvars();
  xvpute();
  errlast = TRUE;
  for(obscnt=1; obscnt<=totalobs; obscnt++)
    obsloop();
  fclose(infile);
  fclose(hazfile);
  fclose(outfile);
  hzf_log1("Note: Procedure HAZPRED completed successfully.");
  exit(0);
  
  /* This return is unecessary, but shuts up compiler warnings */
  return(0);
}
