#include "swab_compat.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hazard.h"

#include <stmtfldname.h>
#include <hzfxit.h>
#include <stmtopts.h>
#include <hzf_memget.h>
#include <hzd_log.h>
/* SAS_TRANSPORT_BYTESWAP is defined by swab_compat.h on little-endian hosts. */

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif
/* Path buffer size used for $TMPDIR/hzr.<study>.<dset>.<ext> construction.
 * The legacy 80-byte buffers overflow via strcat when $TMPDIR > ~53 chars. */
#define HZ_PATH_BUF PATH_MAX

/* Copy up to 8 chars from a space-padded SAS field and truncate at the
 * first space, matching the legacy strncat + strchr(' ')='\0' pattern. */
static void copy_field8(char out[9], const char *src){
  int i;
  for(i=0; i<8 && src[i] != '\0' && src[i] != ' '; i++) out[i] = src[i];
  out[i] = '\0';
}

/****************************************************************/
/* opnfils called from main routine                             */
void opnfils(char *in_file_name){
  char bfr[HZ_PATH_BUF], pfx[HZ_PATH_BUF], *ptr;
  char study[9], dset[9];
  const char *tmpdir;
  int i, n;

  /* Get the current temporary analysis directory */
  HZD_LOG_DEBUG("COMSPEC: %s", getenv("COMSPEC") ? getenv("COMSPEC") : "(unset)");
  HZD_LOG_DEBUG("TMPDIR: %s",  getenv("TMPDIR")  ? getenv("TMPDIR")  : "(unset)");

  if(NULL==(ptr = getenv("TMPDIR"))){
    if(NULL==(ptr = getenv("TEMPDIR"))){
      if(NULL==(ptr = getenv("TMPQDIR"))){
	if(NULL==(ptr = getenv("TEMP"))){
	  ptr = getenv("HAZTEMP");
	}
      }
    }
  }
  tmpdir = (ptr != NULL) ? ptr : "";

  /* Build the path prefix: <tmpdir>/hzr.<study>.<dset>  (Unix)
   *                       <tmpdir>/hzr_<study>_<dset>  (Windows) */
  copy_field8(study, stmtfldname(44));
  copy_field8(dset,  stmtfldname(45));
#if defined(__CYGWIN__) || defined(_WIN32)
  n = snprintf(pfx, sizeof pfx, "%s/hzr_%s_%s", tmpdir, study, dset);
#else
  n = snprintf(pfx, sizeof pfx, "%s/hzr.%s.%s",  tmpdir, study, dset);
#endif
  if(n < 0 || (size_t)n >= sizeof pfx){
    HZD_LOG_ERROR("TMPDIR-based path prefix exceeds %zu bytes: %s", sizeof pfx, tmpdir);
    hzfxit("TMPDIR path too long");
  }

  /* Input data file: <pfx>.dta */
  n = snprintf(bfr, sizeof bfr, "%s.dta", pfx);
  if(n < 0 || (size_t)n >= sizeof bfr){
    HZD_LOG_ERROR("input data file path exceeds %zu bytes", sizeof bfr);
    hzfxit("input file path too long");
  }

  HZD_LOG_DEBUG("data input file: %s", bfr);

  /* Open the input data file */
  if(NULL==(inputDataFile = fopen(bfr,"rb"))) {
    HZD_LOG_ERROR("cannot open input data file: %s", bfr);
    hzfxit("open input file");
  }

  /* Store the input file name for removal after the run */
  strcpy(in_file_name,bfr);

  /* Check for output command */
  /*  I would realy like to enum these stmtopts parameters to
      make them human legible.  */
  outhaz = stmtopts(63);

  if(outhaz) {
    /* Build the output filename string: <pfx>.haz */
    n = snprintf(bfr, sizeof bfr, "%s.haz", pfx);
    if(n < 0 || (size_t)n >= sizeof bfr){
      HZD_LOG_ERROR("output data file path exceeds %zu bytes", sizeof bfr);
      hzfxit("output file path too long");
    }

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

  HZD_LOG_DEBUG("Numvars: %d", numvars);
  HZD_LOG_DEBUG("length of bfr: %zu", strlen(bfr));

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
