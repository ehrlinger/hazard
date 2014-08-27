#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include "hazpred.h"
#include "hzdinc.h"

#include "stmtopts.h"
#include "xlmode.h"
#include "hzf_log1.h"
#include "hzfxit.h"
#define NDEBUG

#include "yyparse.h"

void initprz(void){
  /**
     We would like to be able to define this at compile time
     to do this, we could define some variables (MAJOR_VERSION,
     MINOR_VERSION and MICRO_VERSION) and get them at compile
     time.
  */
  char version[128];
  char version_text[128] = "Note: Procedure HAZPRED (C-Version ";
  sprintf(version, "%s)", VERSION);
  /*  sprintf(version, "%d.%d.%d)", MAJOR_VERSION, MINOR_VERSION,
      MICRO_VERSION); */
  
  /*
    yyparse parses the input command file (.sas) supplied to the 
    stdin. 
  */
  yyparse();
  stmtptr = anchor;
  C->errorno = 0;
  if(stmtopts(4))
    xlmode(0);
  if(stmtopts(5))
    xlmode(1);
  /* Dump a run header */
  hzf_log1(" ");
  hzf_log1(strcat(version_text, version));
#ifndef NDEBUG
  hzf_log1("      DEBUGGING VERSION");
#endif /* NDEBUG */
  hzf_log1("      The Cleveland Clinic Foundation");
  hzf_log1("      9500 Euclid Avenue");
  hzf_log1("      Cleveland, Ohio 44195");
  hzf_log1("      Phone: (216) 444-6712");
  hzf_log1("      Email: hazard@bio.ri.ccf.org");
  if(yysynerr) {
    hzf_log1("Note: Procedure terminating due to above errors.");
    hzfxit("SYNTAX");
  }
}
