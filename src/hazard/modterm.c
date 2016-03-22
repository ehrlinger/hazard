#include <string.h>
#include "hazard.h"
#include <hzfpag.h>
#include <hzfskp.h>
#include <hzfxpc.h>
#include <hzflg1.h>
#include <hzflg2.h>

void modterm(logical iflag){
  hzfpag(6);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  if(C->phase[1]==0 && C->phase[2]==0 && C->phase[3]==0) {
    hzfxpc("No phase selected",17,7);
    hzfskp(1);
    hzflg1("ERROR: No phase selected.",25);
    C->errorno = 1001;
  } else if(iflag) {
    hzfxpc("***  A parameter has been fixed in a way",40,5);
    hzfxpc(" that violates model constraints based on",41,0);
    hzfxpc(" its sign or the specified flags.",33,0);
    hzfskp(1);
    hzflg1("ERROR: Fixed parameter violates model constraints.",50);
    C->errorno = 1002;
  } else if(C->errorno==1 || H->ierr==1) {
    hzfxpc("A computation has exceeded its limits",37,5);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Computation has exceeded limits ",39,
	   C->errflg,12);
    C->errorno = 1;
  } else if(C->errorno==2 || H->ierr==2) {
    hzfxpc("Internal sign violation, possibly data or",41,5);
    hzfxpc("specification dependent",23,-1);
    hzfxpc(C->errflg,12,-1);
    hzfskp(1);
    hzflg2("ERROR: Internal sign violation ",31,C->errflg,12);
    C->errorno = 2;
  }
}
