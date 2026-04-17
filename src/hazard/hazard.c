#include <string.h>
/**
   The hazard program has about 16000 lines of code.

    REVISIONS

    Hazard-4.1.0 (2000)
    Start to comment and recast the code into standard C format
    using GNU coding standards. Divide monolithic files into
    subroutine size files, and recast the global variables into
    usable groupings. JE

    HZD4.036-1 (10/21/95)
    Duplicate option numbers being used for NONOTES and ROBUST
    corrected.

    HZD4.036a (11/24/95)
    exit() calls changed to XEXIT to allow better determination
    of error conditions by SAS code after program ends.

    037 (02/29/96)
    Updates to setopt.c umstop.c umstp0 linesr.c.  linesr.c
    recieved the primary modification, which enabled code
    previously commented out.  umstop and umstp0 had counters in
    "for" loops that were incorrectly initialized to 1, these
    were changed to 0.  setopt.c was modified to more accurately
    follow the fortran source. At the begining of setopt.c
    HZRstr.opt.iret and HZR.opt.trmcod are both set to 0.  This
    does not appear in the fortran code.  If they are left out of
    the C code, the routine fails when shaping parameters are not
    fixed.
**/

/* Comment out NDEBUG to enable debug messages to stderr */
#define NDEBUG

#define HAZARD
#define GLOBAL_DEFS

/****************************************************************/
/* Define STRUCT to alloc storage for structures.h */
#define STRUCT

#include <stdlib.h>

#include <common.h>
#include "hazard.h"

/****************************************************************/
/* hazrd1.h - Split this into multiple files                    */
#include "varsrpt.h"
#include "concrpt.h"
#include "results.h"
#include "outmods.h"

#include "hazrd2.h"
#include "hazrd3.h"
#include "writeOutputDatafile.h"

#include "hazrd4.h"

#include <yyparse.h>

/****************************************************************/
/* hzrcor.h - Split this into multiple files                    */
#include "initprz.h"
#include "opnfils.h"
#include "stmtprc.h"
#include "vfynvar.h"
#include "getrisk.h"
#include "cntobs.h"
#include "getconc.h"
#include "alocmem.h"
#include "setrisk.h"
#include "setconc.h"
#include "set_restrict.h"
#include "readobs.h"
#include "alocwrk.h"
#include "hzrbomb.h"

#include <xinit.h>
#include <hzf_log1.h>
#include <hzfxit.h>
#include <hzfxpc.h>
#include <hzfskp.h>
#include <xexit.h>

#include "hzrg.h"
#include "hzri.h"

/* Debug macro: remove NDEBUG above (or compile with -UNDEBUG) to enable */
#ifdef NDEBUG
#define DBG(fmt, ...) ((void)0)
#else
#define DBG(fmt, ...) fprintf(stderr, "[hazard:%d] " fmt "\n", __LINE__, ##__VA_ARGS__)
#endif

/****************************************************************/
int main(void){
  /* volatile: survives longjmp in error trap */
  volatile char in_file_name[80];
  volatile const char *last_step = "init";
  int err;

  DBG("Starting Hazard executable");

  /* Initialize some variables */
  hzf_mem_need=0;
  xvobslen=0;
  xvgetcnt=0;
  xvputcnt=0;
  xvputlen=0;
  curr_xlmode=2;
  curr_xpmode=1;
  last_xltype=' ';

  /* Initialize the Params structure */
  Params.dnmrlo=4.441e-16;
  Params.dnmrhi = 1.304e+19;
  Params.fdheta = 6.055454452393345e-06;
  Params.lsralp = 1.0e-4;
  Params.lsrbta = 0.9e0;
  Params.um0scl = 1.0e-3;
  Params.normlb[0] = 0.319381530e0;
  Params.normlb[1] = -0.356563782e0;
  Params.normlb[2] = 1.781477937e0;
  Params.normlb[3] = -1.821255978e0;
  Params.normlb[4] = 1.330274429e0;
  Params.normlr = 0.2316419e0;
  Params.lsr2pi = 9.189385332046727417803296e-01;

  /* Initialize the Common Structure */
  C = hzri();                                /* hzrcor.c */
  HZ4I(C);                                   /* hazrd4.c */

  /* Create reference pointers into the Common structure */
  E = C->early;
  H = C->hzrstr;
  L = C->late;
  Machine = C->machn;

  /* Create a reference pointer into the HZRSTR structure */
  O = &H->opt;

  /* initialize the error flag */
  semerr = FALSE;

  /* XINIT performs some sanity checks for the current platform */
  xinit();                                  /* common/hazcfn.c */

  /* Error trap: catches any longjmp from hzd_Error() */
  if ((err = setjmp(C->errtrap)) != 0) {
    fprintf(stderr, "HAZARD ERROR %d %s: abnormal termination after step \"%s\"\n",
            err, C->errflg, last_step);
    hzrbomb();
    xexit(1);
  }

  last_step = "initprz";
  DBG("Reading input");
  initprz();

  last_step = "opnfils";
  DBG("Opening input and output files");
  opnfils((char *)in_file_name);
  DBG("Input file: %s", in_file_name);

  last_step = "stmtprc";
  DBG("Initialize variables");
  stmtprc();

  last_step = "cntobs";
  DBG("cntobs");
  cntobs();

  last_step = "getconc";
  DBG("getconc");
  getconc();

  last_step = "getrisk";
  DBG("getrisk");
  getrisk();

  /* Check for errors to this point */
  if(semerr) {
    hzf_log1("Note: PROCEDURE TERMINATING DUE TO ABOVE ERRORS.");
    hzfxit("SEMANTIC");
  }

  last_step = "alocmem";
  DBG("alocmem");
  alocmem();

  /* Dump analysis header */
  hzfxpc(" T H R E E   P H A S E   P A R A M E T R I C   H A Z A R D   P R O C E D U R E",0,26);
  hzfskp(2);

  last_step = "setrisk";
  DBG("setrisk");
  setrisk();

  last_step = "setconc";
  DBG("setconc");
  setconc();

  last_step = "set_restrict";
  DBG("set_restrict");
  set_restrict();

  last_step = "readobs";
  DBG("readobs");
  readobs();

  last_step = "varsrpt";
  DBG("varsrpt");
  varsrpt();

  last_step = "concrpt";
  DBG("concrpt");
  concrpt();

  last_step = "alocwrk";
  DBG("alocwrk");
  alocwrk();

  last_step = "hzrg";
  DBG("hzrg");
  hzrg();

  last_step = "outmods";
  DBG("outmods");
  outmods();

  /* */
  if(C->errorno==1 || C->errorno==2)
    hzfxit("ERROR");
  else if(C->errorno==1001 || C->errorno==1002)
    hzfxit("SEMANTIC");

  last_step = "results";
  DBG("results");
  results();

  /* Close the input data file */
  fclose(inputDataFile);

#ifdef NDEBUG
  /* By default the inputDataFile is removed on program exit */
  /*  remove(in_file_name);*/
#endif /* NDEBUG */

  last_step = "HAZRD2";
  DBG("Output");
  HAZRD2();

  if(outhaz) {
    last_step = "writeOutputDatafile";
    writeOutputDatafile();
    fclose(outputDataFile);
  }

  /* Dump end message and exit */
  hzf_log1("Note: Procedure HAZARD completed successfully.");
  DBG("Note: Procedure HAZARD completed successfully.");
  xexit(0);

  /* This return is unecessary, but shuts up compiler warnings */
  exit(0);
}
