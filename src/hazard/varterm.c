#include "hazard.h"

#include <hzfskp.h>
#include <hzfxpc.h>
#include "isblank.h"
#include <hzflg1.h>
#include <hzflg2.h>
#include <hzfxit.h>

void varterm(void){
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(1);
  hzfxpc("| Termination |",15,4);
  hzfskp(1);
  hzfxpc("*-------------*",15,4);
  hzfskp(2);
  if(ISBLANK(tname)) {
    hzfxpc("The TIME variable was not specified. ",37,7);
    hzfxpc(" procedure is terminated.",25,0);
    hzfskp(1);
    hzflg1("ERROR: The TIME variable was not specified.",43);
  }
  if(ISBLANK(c1name) && ISBLANK(c3name)) {
    hzfxpc("The EVENT or ICENSOR variable must be",37,7);
    hzfxpc(" specified.  Procedure is terminated.",37,0);
    hzfskp(1);
    hzflg2("ERROR: The EVENT or ICENSOR variable ",37,
	   "must be specified.",18);
  }
  if(C->Nobs<=0) {
    hzfxpc("No observations met the criteria for",36,7);
    hzfxpc(" processing.  Procedure is terminated.",38,0);
    hzfskp(1);
    hzflg2("ERROR: No observations met the criteria ",40,
	   "for processing.",15);
  }
  if(ISBLANK(tname) || (ISBLANK(c1name) && ISBLANK(c3name)))
    hzfxit("SEMANTIC");
  else
    hzfxit("DATA");
}
