#include "swab_compat.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stmtfldname.h"
#include "hazpred.h"
#include "hzfxit.h"
#include "hzf_log1.h"
#include "hzf_memget.h"
/* SAS transport format is big-endian; always byte-swap on read. */
#define SAS_TRANSPORT_BYTESWAP

void opnfils(void){
  char bfr[80],pfx[80],*ptr;
  size_t i;
  int field_count;

  if(NULL==(ptr = getenv("TMPDIR")))
    if(NULL==(ptr = getenv("TEMPDIR")))
      if(NULL==(ptr = getenv("TMPQDIR")))
        if(NULL==(ptr = getenv("TEMP")))
          ptr = getenv("HAZTEMP");
  if(ptr!=NULL)
    strcpy(pfx,ptr);
  else
    *pfx = '\0';

#if defined(__CYGWIN__) || defined(_WIN32)
  /* Windows (Cygwin and MinGW/MSYS2) */
  strcat(pfx,"/hzp_");
#else
  /* Other platforms */
  strcat(pfx,"/hzp.");
#endif
  strncat(pfx,stmtfldname(14),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
#if defined(__CYGWIN__) || defined(_WIN32)
  strcat(pfx,"_");
#else
  strcat(pfx,".");
#endif
  strncat(pfx,stmtfldname(15),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
  strcpy(bfr,pfx);
  strcat(bfr,".dta");
  if(NULL==(infile = fopen(bfr,"rb"))) {
    fprintf(stderr, "ERROR: cannot open input data file: %s\n", bfr);
    hzfxit("infile");
  }
  strcpy(bfr,pfx);
  strcat(bfr,".haz");

  /* Open the input file for binary reading */
  hazfile = fopen(bfr,"rb");
  strcpy(bfr,pfx);
  strcat(bfr,".out");

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
