#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "hzf_log1.h"
#include "yyparse.h"

/* This file is used by both hazard and hazpred.
   So we define the extern variables rather than include them */
extern char msgbfr[80];

int yyerror(char *str){
  sprintf(stderr,"ERROR: %s parse error (%s)\n",PACKAGE,str);
  hzf_log1(msgbfr);
  yysynerr = 1;
  return(yysynerr);
}

