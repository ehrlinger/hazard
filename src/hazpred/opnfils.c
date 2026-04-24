#include "swab_compat.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stmtfldname.h"
#include "hazpred.h"
#include "hzfxit.h"
#include "hzf_log1.h"
#include "hzf_memget.h"
/* SAS_TRANSPORT_BYTESWAP is defined by swab_compat.h on little-endian hosts. */

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif
/* Path buffer size used for $TMPDIR/hzp.<study>.<dset>.<ext> construction.
 * The legacy 80-byte buffers overflow via strcat when $TMPDIR > ~53 chars. */
#define HZ_PATH_BUF PATH_MAX

/* Copy up to 8 chars from a space-padded SAS field and truncate at the
 * first space, matching the legacy strncat + strchr(' ')='\0' pattern. */
static void copy_field8(char out[9], const char *src){
  int i;
  for(i=0; i<8 && src[i] != '\0' && src[i] != ' '; i++) out[i] = src[i];
  out[i] = '\0';
}

void opnfils(void){
  char bfr[HZ_PATH_BUF], pfx[HZ_PATH_BUF], *ptr;
  char study[9], dset[9];
  const char *tmpdir;
  size_t i;
  int field_count, n;

  if(NULL==(ptr = getenv("TMPDIR")))
    if(NULL==(ptr = getenv("TEMPDIR")))
      if(NULL==(ptr = getenv("TMPQDIR")))
        if(NULL==(ptr = getenv("TEMP")))
          ptr = getenv("HAZTEMP");
  tmpdir = (ptr != NULL) ? ptr : "";

  copy_field8(study, stmtfldname(14));
  copy_field8(dset,  stmtfldname(15));
#if defined(__CYGWIN__) || defined(_WIN32)
  n = snprintf(pfx, sizeof pfx, "%s/hzp_%s_%s", tmpdir, study, dset);
#else
  n = snprintf(pfx, sizeof pfx, "%s/hzp.%s.%s",  tmpdir, study, dset);
#endif
  if(n < 0 || (size_t)n >= sizeof pfx){
    fprintf(stderr, "ERROR: TMPDIR-based path prefix exceeds %zu bytes: %s\n",
            sizeof pfx, tmpdir);
    hzfxit("TMPDIR path too long");
  }

  n = snprintf(bfr, sizeof bfr, "%s.dta", pfx);
  if(n < 0 || (size_t)n >= sizeof bfr){
    fprintf(stderr, "ERROR: input data file path exceeds %zu bytes\n", sizeof bfr);
    hzfxit("input file path too long");
  }
  if(NULL==(infile = fopen(bfr,"rb"))) {
    fprintf(stderr, "ERROR: cannot open input data file: %s\n", bfr);
    hzfxit("infile");
  }

  n = snprintf(bfr, sizeof bfr, "%s.haz", pfx);
  if(n < 0 || (size_t)n >= sizeof bfr){
    fprintf(stderr, "ERROR: haz file path exceeds %zu bytes\n", sizeof bfr);
    hzfxit("haz file path too long");
  }
  /* Open the INHAZ file produced by an earlier hazard run.  A missing
   * .haz on disk means the upstream hazard example hasn't been captured
   * or staged into $TMPDIR; subsequent fread(hazfile, ...) calls would
   * deref a NULL FILE* and segfault (observed as the 8/8 SIGSEGV on the
   * hazpred corpus — FINDINGS.md §2b).  Fail fast with a clear message. */
  if(NULL==(hazfile = fopen(bfr,"rb"))) {
    fprintf(stderr, "ERROR: cannot open INHAZ file: %s\n", bfr);
    hzfxit("hazfile");
  }

  n = snprintf(bfr, sizeof bfr, "%s.out", pfx);
  if(n < 0 || (size_t)n >= sizeof bfr){
    fprintf(stderr, "ERROR: output file path exceeds %zu bytes\n", sizeof bfr);
    hzfxit("output file path too long");
  }
  /* Open the output file for binary writing */
  if(NULL==(outputDataFile = outfile = fopen(bfr,"wb")))
    hzfxit("outfile");

  for(i=7; i; i--) {
    if(fread(bfr,80,1,infile)!=1) {
      /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
    }
    fwrite(bfr,80,1,outfile);
    if(fread(bfr,80,1,hazfile)!=1) {
      /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
    }
  }

  if(fread(bfr,80,1,infile)!=1) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }
  if(sscanf(&bfr[54],"%4d",&field_count)!=1 || field_count<0) {
    hzf_log1("ERROR: INFILE data set header has invalid field count.");
    hzfxit("DATA");
  }
  infilect = (size_t)field_count;
  if(fread(bfr,80,1,hazfile)!=1) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }
  if(sscanf(&bfr[54],"%4d",&field_count)!=1 || field_count<0) {
    hzf_log1("ERROR: INHAZ data set header has invalid field count.");
    hzfxit("DATA");
  }
  nvars = (size_t)field_count;

  data_ns = (struct namestr *)hzf_memget(sizeof(struct namestr)*infilect);
  inhaz_ns = (struct namestr *)hzf_memget(sizeof(struct namestr)*nvars);

  if(fread(data_ns,sizeof(struct namestr),infilect,infile)!=infilect) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }
  if(fread(inhaz_ns,sizeof(struct namestr),nvars,hazfile)!=nvars) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }

  for(i=0; i<infilect; i++) {
#ifdef SAS_TRANSPORT_BYTESWAP
    /* SAS transport files are big-endian; convert to host byte order. */
    data_ns[i].nlng  = hzd_bswap_short(data_ns[i].nlng);
    data_ns[i].ntype = hzd_bswap_short(data_ns[i].ntype);
    data_ns[i].nvar0 = hzd_bswap_short(data_ns[i].nvar0);
    data_ns[i].npos  = hzd_bswap_int(data_ns[i].npos);
#endif
    data_ln += data_ns[i].nlng;
  }

  for(i=0; i<nvars; i++) {
#ifdef SAS_TRANSPORT_BYTESWAP
    inhaz_ns[i].nlng  = hzd_bswap_short(inhaz_ns[i].nlng);
    inhaz_ns[i].ntype = hzd_bswap_short(inhaz_ns[i].ntype);
    inhaz_ns[i].npos  = hzd_bswap_int(inhaz_ns[i].npos);
#endif
    inhaz_ln += inhaz_ns[i].nlng;
  }

  i = 640+infilect*sizeof(struct namestr);
  i = 80-i%80;
  if(i!=80)
    if(fread(bfr,i,1,infile)!=1) {
      /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
    }
  if(fread(bfr,80,1,infile)!=1) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }
  i = 640+nvars*sizeof(struct namestr);
  i = 80-i%80;
  if(i!=80)
    if(fread(bfr,i,1,hazfile)!=1) {
      /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
    }
  if(fread(bfr,80,1,hazfile)!=1) {
    /* Preserve legacy flow; downstream read/parse checks remain unchanged. */
  }
}
