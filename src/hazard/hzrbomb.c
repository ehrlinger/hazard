#include "hazard.h"
#include "hazrerr.h"
#include <hzfpag.h>
#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzflg2.h>
#include <hzf_log1.h>
#include <xexit.h>

/****************************************************************/
/* HZRBOMB called from main                                     */
void hzrbomb(void){
  hzfpag(6);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  if(C->errorno==1 || H->ierr==1) {
    hzfxpc("A computation has exceeded its limits",37,5);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Computation has exceeded limits ",39,
	   C->errflg,12);
    C->errorno = 1;
    hazrerr();
  } else if(C->errorno==2 || H->ierr==2) {
    hzfxpc("Internal sign violation, possibly data or",41,5);
    hzfxpc("specification dependent",23,-1);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Internal sign violation ",31,C->errflg,12);
    C->errorno = 2;
    hazrerr();
  } else {
    sprintf(msgbfr,"ERROR: Low-level error trap sprung, catching %12.12s",
	    C->errflg);
    hzf_log1(msgbfr);
    hzf_log1("      Please report this problem to authors immediately!");
    xexit(16);
  }
}
