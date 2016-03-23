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
#define __Linux__
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
#ifdef __CYGWIN__
  /* on windows */
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
  if(NULL==(inputDataFile = fopen(bfr,"rb")))
    /* Exit on opening error */
    hzfxit("open input file");

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
    fread(bfr,80,1,inputDataFile);
    if(outhaz)
      fwrite(bfr,80,1,outputDataFile);
  }

  fread(bfr,80,1,inputDataFile);

  /* Get the number of relevent variables */
  sscanf(&bfr[54],"%4ld",&numvars);

#ifndef NDEBUG
  fprintf(stderr, "Numvars: %ld\n", numvars);
  fprintf(stderr, "length of bfr: %d\n", strlen(bfr));
#endif

  ns = (struct namestr *)hzf_memget(sizeof(struct namestr)*numvars);

  /*
    Read in the data set
    An array of NAMESTR[numvar]
  */
  fread(ns,sizeof(struct namestr),(size_t)numvars,inputDataFile);

  for(i=0; i<numvars; i++) {
#ifdef __Linux__
    /*
      Because of the way that Windows (MSDOS) (and 64 bits Linux) pps store binary data,
      we need to modify the bitwise representation after reads.

      This keeps the files written (maybe) cross platform compatible
    */
    swab((void *)&ns[i].nlng,(void *)&ns[i].nlng,sizeof(short));
    swab((void *)&ns[i].ntype,(void *)&ns[i].ntype,sizeof(short));
    memcpy(bfr,(void *)&ns[i].npos,4);
    swab(bfr,bfr+6,2);
    swab(bfr+2,bfr+4,2);
    memcpy((void *)&ns[i].npos,bfr+4,4);
#endif

    xvobslen += ns[i].nlng;
  }

  /* Another sanity check? */
  i = 640+numvars*sizeof(struct namestr);
  i = 80-i%80;
  if(i!=80)
    fread(bfr,(size_t)i,1,inputDataFile);
  fread(bfr,80,1,inputDataFile);
}
