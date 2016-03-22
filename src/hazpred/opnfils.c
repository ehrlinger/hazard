#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stmtfldname.h"
#include "hazpred.h"
#include "hzfxit.h"
#include "hzf_memget.h"
#define __Linux__

void opnfils(void){
  char bfr[80],pfx[80],*ptr;
  size_t i;

  if(NULL==(ptr = getenv("TMPDIR")))
    if(NULL==(ptr = getenv("TEMPDIR")))
      if(NULL==(ptr = getenv("TMPQDIR")))
	ptr = getenv("HAZTEMP");
  if(ptr!=NULL)
    strcpy(pfx,ptr);
  else
    *pfx = '\0';

#ifdef __CYGWIN__ 
  strcat(pfx,"/hzp_");
#else
  strcat(pfx,"/hzp.");
#endif 
  strncat(pfx,stmtfldname(14),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
#ifdef __CYGWIN__ 
  strcat(pfx,"_");
#else
  strcat(pfx,".");
#endif
  strncat(pfx,stmtfldname(15),8);
  if(NULL!=(ptr = strchr(pfx,' ')))
    *ptr = '\0';
  strcpy(bfr,pfx);
  strcat(bfr,".dta");
  if(NULL==(infile = fopen(bfr,"rb")))
    hzfxit("infile");
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
    fread(bfr,80,1,infile);
    fwrite(bfr,80,1,outfile);
    fread(bfr,80,1,hazfile);
  }

  fread(bfr,80,1,infile);
  sscanf(&bfr[54],"%4d",&infilect);
  fread(bfr,80,1,hazfile);
  sscanf(&bfr[54],"%4d",&nvars);

  data_ns = (struct namestr *)hzf_memget(sizeof(struct namestr)*infilect);
  inhaz_ns = (struct namestr *)hzf_memget(sizeof(struct namestr)*nvars);

  fread(data_ns,sizeof(struct namestr),infilect,infile);
  fread(inhaz_ns,sizeof(struct namestr),nvars,hazfile);

  for(i=0; i<infilect; i++) {

#ifdef __Linux__
  /*
    Because of the way that Windows (MSDOS) apps store binary data,
    we need to modify the bitwise representation before writing.

    This keeps the files written (maybe) cross platform compatible
  */
    swab((void *)&data_ns[i].nlng,(void *)&data_ns[i].nlng,sizeof(short));
    swab((void *)&data_ns[i].ntype,(void *)&data_ns[i].ntype,sizeof(short));
    swab((void *)&data_ns[i].nvar0,(void *)&data_ns[i].nvar0,sizeof(short));
    memcpy(bfr,(void *)&data_ns[i].npos,4);
    swab(bfr,bfr+6,2);
    swab(bfr+2,bfr+4,2);
    memcpy((void *)&data_ns[i].npos,bfr+4,4);
#endif
    data_ln += data_ns[i].nlng;
  }

  for(i=0; i<nvars; i++) {
#ifdef __Linux__
    swab((void *)&inhaz_ns[i].nlng,(void *)&inhaz_ns[i].nlng,sizeof(short));
    swab((void *)&inhaz_ns[i].ntype,(void *)&inhaz_ns[i].ntype,sizeof(short));
    memcpy(bfr,(void *)&inhaz_ns[i].npos,4);
    swab(bfr,bfr+6,2);
    swab(bfr+2,bfr+4,2);
    memcpy((void *)&inhaz_ns[i].npos,bfr+4,4);
#endif
    inhaz_ln += inhaz_ns[i].nlng;
  }

  i = 640+infilect*sizeof(struct namestr);
  i = 80-i%80;
  if(i!=80)
    fread(bfr,i,1,infile);
  fread(bfr,80,1,infile);
  i = 640+nvars*sizeof(struct namestr);
  i = 80-i%80;
  if(i!=80)
    fread(bfr,i,1,hazfile);
  fread(bfr,80,1,hazfile);
}
