#include "swab_compat.h"
#include <string.h>
/* Initilize the run */
/* Comment out this statement to print debugging messages to the
   stderr stream. This statement effects this file only */
#define NDEBUG

#include <stdlib.h>
#include <string.h>
#include "hazard.h"

#include <stmtfldname.h>
#include <hzfxit.h>
#include <stmtopts.h>
#include <hzf_memget.h>
/* SAS transport format is big-endian; always byte-swap on read. */
#define SAS_TRANSPORT_BYTESWAP
/* #define NDEBUG */

/****************************************************************/
/* opnfils called from main routine                             */
void opnfils(char *in_file_name){
  char bfr[80],pfx[80],*ptr;
  int i;

  /* Get the current temporary analysis directory */
#ifndef NDEBUG
  fprintf(stderr, "COMSPEC: %s\n", getenv("COMSPEC"));
  fprintf(stderr, "TMPDIR: %s\n", getenv("TMPDIR"));
#endif

  if(NULL==(ptr = getenv("TMPDIR"))){
    if(NULL==(ptr = getenv("TEMPDIR"))){
      if(NULL==(ptr = getenv("TMPQDIR"))){
	if(NULL==(ptr = getenv("TEMP"))){
	  ptr = getenv("HAZTEMP");
	}
      }
    }
  }
  if(ptr!=NULL)
    strcpy(pfx,ptr);
  else
    *pfx = '\0';

  /* Start building the filename string */
#if defined(__CYGWIN__) || defined(_WIN32)
  /* Windows (Cygwin and MinGW/MSYS2) */
  strcat(pfx,"/hzr_");
  strncat(pfx,stmtfldname(44),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
  strcat(pfx,"_");
#else
  /* Other platforms */
  strcat(pfx,"/hzr.");
  strncat(pfx,stmtfldname(44),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
  strcat(pfx,".");
#endif
  strncat(pfx,stmtfldname(45),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
  strcpy(bfr,pfx);
  strcat(bfr,".dta");

#ifndef NDEBUG
  fprintf(stderr, "data input file: %s\n", bfr);
#endif /* NDEBUG */

  /* Open the input data file */
  if(NULL==(inputDataFile = fopen(bfr,"rb"))) {
    fprintf(stderr, "ERROR: cannot open input data file: %s\n", bfr);
    hzfxit("open input file");
  }

  /* Store the input file name for removal after the run */
  strcpy(in_file_name,bfr);

  /* Check for output command */
  /*  I would realy like to enum these stmtopts parameters to
      make them human legible.  */
  outhaz = stmtopts(63);

  if(outhaz) {
    /* Build the output filename string */
    strcpy(bfr,pfx);
    strcat(bfr,".haz");

    /* Open the output file data file */
    if(NULL==(outputDataFile = fopen(bfr,"wb")))
      /* Exit on open error */
      hzfxit("outfile");
  }

  /* Start reading/(writing) the data file header */
  for(i=7; i; i--){
    if(fread(bfr,80,1,inputDataFile)!=1) {
      /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
    }
    if(outhaz)
      fwrite(bfr,80,1,outputDataFile);
  }

  if(fread(bfr,80,1,inputDataFile)!=1) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }

  /* numvars is an int global (hzdinc.h), so parse it as %d. */
  sscanf(&bfr[54],"%4d",&numvars);

#ifndef NDEBUG
  fprintf(stderr, "Numvars: %d\n", numvars);
  fprintf(stderr, "length of bfr: %d\n", strlen(bfr));
#endif

  ns = (struct namestr *)hzf_memget(sizeof(struct namestr)*numvars);

  /*
    Read in the data set
    An array of NAMESTR[numvar]
  */
  if(fread(ns,sizeof(struct namestr),(size_t)numvars,inputDataFile)!=(size_t)numvars) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }

  for(i=0; i<numvars; i++) {
#ifdef SAS_TRANSPORT_BYTESWAP
    /* SAS transport files are big-endian; convert to host byte order. */
    ns[i].nlng  = hzd_bswap_short(ns[i].nlng);
    ns[i].ntype = hzd_bswap_short(ns[i].ntype);
    ns[i].npos  = hzd_bswap_int(ns[i].npos);
#endif

    xvobslen += ns[i].nlng;
  }

  /* Another sanity check? */
  i = 640+numvars*sizeof(struct namestr);
  i = 80-i%80;
  if(i!=80)
    if(fread(bfr,(size_t)i,1,inputDataFile)!=1) {
      /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
    }
  if(fread(bfr,80,1,inputDataFile)!=1) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }
}
