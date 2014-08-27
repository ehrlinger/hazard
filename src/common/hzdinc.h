#ifndef HZDINC_H
#define HZDINC_H

/******************************************************************** 
    HAZARD/HAZPRED Common Global Definitions                             
    All ANSI C Compilers and Operating Environments                    
    01/05/95 -- 4.0(0)                                                 
  ********************************************************************/

/* Since we use structures in the header file, 
   include the structure definitions */
#include "structures.h"

/* Why do we need to undef a defined constant and hard code our own? */
#ifdef MSDOS
#undef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif

/* we need to determine what these are for. */
#define HZ_DELTA 0
#define HZ_THALF 1
#define HZ_NU 2
#define HZ_M 3
#define HZ_TAU 4
#define HZ_GAMMA 5
#define HZ_ALPHA 6
#define HZ_ETA 7
#define HZ_MUE 8
#define HZ_MUC 9
#define HZ_MUL 10
#define HZ_RHO 11

/* Common Global variables */
#ifdef GLOBAL_DEFS
#define EXTERN

/* Define this here so we can initialize it, should this be an enum? */
char parms[12][9]={"DELTA","THALF","NU","M", "TAU","GAMMA","ALPHA",
		   "ETA","MUE","MUC","MUL","RHO"};

#else
#define EXTERN extern

/* For all other definitions, provide a global pointer */
extern char parms[12][9];

#endif /* GLOBAL_DEFS */

EXTERN long hzf_mem_need;
EXTERN long numvars;
EXTERN long xvobslen;
EXTERN long xvgetcnt;
EXTERN long xvputcnt;
EXTERN long xvputlen;
EXTERN int curr_xlmode;
EXTERN int  curr_xpmode;
EXTERN char last_xltype;
EXTERN char *xvobsbfr;
EXTERN FILE *inputDataFile;
EXTERN FILE *outputDataFile;

#undef EXTERN

#endif /* HZDINC_H */
